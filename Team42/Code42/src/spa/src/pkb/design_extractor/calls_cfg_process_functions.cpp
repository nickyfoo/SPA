#include <design_extractor.h>
#include <algorithm>
#include <vector>
#include "ast_utils.hpp"
#include "pkb_exception.h"

void DesignExtractor::CallsProcessCallNode(Node *node, std::vector<Node *> &ancestors) {
  auto *call_node = dynamic_cast<CallNode *>(node);
  for (Node *n : ancestors) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *caller_proc_node = dynamic_cast<ProcedureNode *>(n);

      std::string caller_proc_name = caller_proc_node->get_name();
      std::string called_proc_name = call_node->get_proc()->get_name();
      auto *pkb_calling_proc = pkb_->proc_table_.get_procedure(caller_proc_name);
      auto *pkb_called_proc = pkb_->proc_table_.get_procedure(called_proc_name);

      if (pkb_called_proc == nullptr) {
        throw PKBException("Called an undefined procedure: " + called_proc_name);
      }

      pkb_calling_proc->AddCalls(called_proc_name);
      pkb_called_proc->AddCallers(caller_proc_name);
    }
  }
}

std::set<std::pair<int,int>> DesignExtractor::GetLastStmts(StatementNode *node, bool forCFG) {
  std::set<std::pair<int,int>> ans;
  if (node->get_kind() == NodeType::If) {
    auto *if_node = dynamic_cast<IfNode *>(node);

    // Sort then statement list in ascending statement numbers
    std::vector<StatementNode *> then_stmt_lst = if_node->get_then_stmt_lst();
    std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
                return a->get_stmt_no() > b->get_stmt_no();
              });
    for (auto &[stmt,branch] : GetLastStmts(*(then_stmt_lst.begin()), forCFG)) {
      ans.insert({ stmt, branch });
    }

    // Sort else statement list in ascending statement numbers
    std::vector<StatementNode *> else_stmt_lst = if_node->get_else_stmt_lst();
    std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
                return a->get_stmt_no() > b->get_stmt_no();
              });
    for (auto &[stmt,branch] : GetLastStmts(*(else_stmt_lst.begin()), forCFG)) {
      ans.insert({ stmt, branch });
    }
  }
  else if (node->get_kind() == NodeType::Call) {
    if (forCFG) {
      ans.insert({ node->get_stmt_no(), pkb_->kNoBranch });
    }
    else {
      Statement* call_stmt = pkb_->stmt_table_.get_statement(node->get_stmt_no());
      int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
      ans.insert({-first_stmt_of_called_proc, -call_stmt->get_stmt_no()});
    }
  }
  else {
    ans.insert({ node->get_stmt_no(), pkb_->kNoBranch });
  }
  return ans;
}

/*
void DesignExtractor::LastStmtsOfProcedure(std::string proc_name, std::set<int> &ans) {
  std::set<int> *stmt_lst = pkb_->proc_table_.get_procedure(proc_name)->get_stmt_lst();
  int last_stmt_no = *stmt_lst->rbegin();
  Statement *stmt = pkb_->stmt_table_.get_statement(last_stmt_no);
  
  std::set<int> *last_stmts_of_stmt = stmt->get_last_stmts();
  for (auto &stmt_no : *last_stmts_of_stmt) {
    if (pkb_->stmt_table_.get_statement(stmt_no)->get_kind() == NodeType::Call) {
      LastStmtsOfProcedure(pkb_->stmt_table_.get_statement(stmt_no)->get_called_proc_name(), ans);
    }
    else {
      ans.insert(stmt_no);
    }
  }
}
*/

void DesignExtractor::CFGProcessStmtLst(std::vector<StatementNode*>& stmt_lst) {
  std::sort(stmt_lst.begin(), stmt_lst.end(),
    [](StatementNode* a, StatementNode* b) {
      return a->get_stmt_no() < b->get_stmt_no();
    });

  int n = stmt_lst.size();
  for (int i = 1; i < n; i++) {
    for (auto& [last_stmt, branch] : GetLastStmts(stmt_lst[i - 1], true)) {
      pkb_->cfg_al_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      pkb_->reverse_cfg_al_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);
    }
  }

  for (int i = 1; i < n; i++) {
    if (stmt_lst[i - 1]->get_kind() == NodeType::Call) {
      Statement* call_stmt = pkb_->stmt_table_.get_statement(stmt_lst[i - 1]->get_stmt_no());
      int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
      pkb_->cfg_bip_al_[stmt_lst[i - 1]->get_stmt_no()].insert({ first_stmt_of_called_proc, stmt_lst[i - 1]->get_stmt_no() });
      pkb_->cfg_bip_al_[-first_stmt_of_called_proc].insert({ stmt_lst[i]->get_stmt_no(), -stmt_lst[i - 1]->get_stmt_no() });
      pkb_->reverse_cfg_bip_al_[first_stmt_of_called_proc].insert({ stmt_lst[i - 1]->get_stmt_no(), stmt_lst[i - 1]->get_stmt_no() });
      pkb_->reverse_cfg_bip_al_[stmt_lst[i]->get_stmt_no()].insert({ -first_stmt_of_called_proc, -stmt_lst[i - 1]->get_stmt_no() });
    }
    else {
      for (auto& [last_stmt, branch] : GetLastStmts(stmt_lst[i - 1], false)) {
        pkb_->cfg_bip_al_[last_stmt].insert({ stmt_lst[i]->get_stmt_no(), PKB::kNoBranch });
        pkb_->reverse_cfg_bip_al_[stmt_lst[i]->get_stmt_no()].insert({ last_stmt, PKB::kNoBranch });
      }
    }
  }
}

void DesignExtractor::CFGProcessProcedureNode(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);

  // Sort statement list in ascending statement numbers
  std::vector<StatementNode *> stmt_lst = procedure_node->get_stmt_lst();
  CFGProcessStmtLst(stmt_lst);

  int n = stmt_lst.size();

  //Join last stmt to dummy end of proc node
  int first_stmt_of_current_proc = pkb_->proc_table_.get_procedure(procedure_node->get_name())->get_stmt_no();
  if (stmt_lst[n-1]->get_kind() == NodeType::Call) {
    Statement* call_stmt = pkb_->stmt_table_.get_statement(stmt_lst[n-1]->get_stmt_no());
    int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    pkb_->cfg_bip_al_[stmt_lst[n - 1]->get_stmt_no()].insert({ first_stmt_of_called_proc, stmt_lst[n - 1]->get_stmt_no() });
    pkb_->cfg_bip_al_[-first_stmt_of_called_proc].insert({ -first_stmt_of_current_proc, -stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[first_stmt_of_called_proc].insert({ stmt_lst[n - 1]->get_stmt_no(), stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[-first_stmt_of_current_proc].insert({ -first_stmt_of_called_proc, -stmt_lst[n - 1]->get_stmt_no() });
  }
  else {
    for (auto& [last_stmt,branch] : GetLastStmts(stmt_lst[n - 1], false)) {
      pkb_->cfg_bip_al_[last_stmt].insert({ -first_stmt_of_current_proc, branch });
      pkb_->reverse_cfg_bip_al_[-first_stmt_of_current_proc].insert({ last_stmt, branch });
    }
  }
}

void DesignExtractor::CFGProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  int if_stmt_no = if_node->get_stmt_no();

  // Sort then statement list in ascending statement numbers
  std::vector<StatementNode *> then_stmt_lst = if_node->get_then_stmt_lst();
  std::vector<StatementNode*> else_stmt_lst = if_node->get_else_stmt_lst();
  CFGProcessStmtLst(then_stmt_lst);
  CFGProcessStmtLst(else_stmt_lst);


  // Connect if node to first node of then_stmt_lst
  if (!then_stmt_lst.empty()) {
    int then_first_stmt_no = then_stmt_lst[0]->get_stmt_no();

    pkb_->cfg_al_[if_stmt_no].insert(then_first_stmt_no);
    pkb_->reverse_cfg_al_[then_first_stmt_no].insert(if_stmt_no);
    pkb_->cfg_bip_al_[if_stmt_no].insert({then_first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[then_first_stmt_no].insert({if_stmt_no, PKB::kNoBranch});
  }

  int then_n = then_stmt_lst.size();

  //Join last stmt to dummy end of proc node
  if (then_stmt_lst[then_n - 1]->get_kind() == NodeType::Call) {
    Statement* call_stmt = pkb_->stmt_table_.get_statement(then_stmt_lst[then_n - 1]->get_stmt_no());
    int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    pkb_->cfg_bip_al_[then_stmt_lst[then_n - 1]->get_stmt_no()].insert({ first_stmt_of_called_proc, then_stmt_lst[then_n - 1]->get_stmt_no() });
    //pkb_->cfg_bip_al_[-first_stmt_of_called_proc].insert({ -first_stmt_of_current_proc, -stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[first_stmt_of_called_proc].insert({ then_stmt_lst[then_n - 1]->get_stmt_no(), then_stmt_lst[then_n - 1]->get_stmt_no() });
    //pkb_->reverse_cfg_bip_al_[-first_stmt_of_current_proc].insert({ -first_stmt_of_called_proc, -stmt_lst[n - 1]->get_stmt_no() });
  }

  // Connect if node to first node of else_stmt_lst
  if (!else_stmt_lst.empty()) {
    int else_first_stmt_no = else_stmt_lst[0]->get_stmt_no();

    pkb_->cfg_al_[if_stmt_no].insert(else_first_stmt_no);
    pkb_->reverse_cfg_al_[else_first_stmt_no].insert(if_stmt_no);
    pkb_->cfg_bip_al_[if_stmt_no].insert({else_first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[else_first_stmt_no].insert({if_stmt_no, PKB::kNoBranch});
  }

  int else_n = else_stmt_lst.size();

  //Join last stmt to dummy end of proc node
  if (else_stmt_lst[else_n - 1]->get_kind() == NodeType::Call) {
    Statement* call_stmt = pkb_->stmt_table_.get_statement(else_stmt_lst[else_n - 1]->get_stmt_no());
    int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    pkb_->cfg_bip_al_[else_stmt_lst[else_n - 1]->get_stmt_no()].insert({ first_stmt_of_called_proc, else_stmt_lst[else_n - 1]->get_stmt_no() });
    //pkb_->cfg_bip_al_[-first_stmt_of_called_proc].insert({ -first_stmt_of_current_proc, -stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[first_stmt_of_called_proc].insert({ else_stmt_lst[else_n - 1]->get_stmt_no(), else_stmt_lst[else_n - 1]->get_stmt_no() });
    //pkb_->reverse_cfg_bip_al_[-first_stmt_of_current_proc].insert({ -first_stmt_of_called_proc, -stmt_lst[n - 1]->get_stmt_no() });
  }
}

void DesignExtractor::CFGProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  int while_stmt_no = while_node->get_stmt_no();

  // Sort statement list in ascending statement numbers
  std::vector<StatementNode *> stmt_lst = while_node->get_stmt_list();
  CFGProcessStmtLst(stmt_lst);

  // Connect while node to first node of stmt_lst
  if (!stmt_lst.empty()) {
    int first_stmt_no = stmt_lst[0]->get_stmt_no();
    pkb_->cfg_al_[while_stmt_no].insert(first_stmt_no);
    pkb_->reverse_cfg_al_[first_stmt_no].insert(while_stmt_no);
    pkb_->cfg_bip_al_[while_stmt_no].insert({first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({while_stmt_no, PKB::kNoBranch});
  }

  int n = stmt_lst.size();
  //Join last stmt back to while
  for (auto& [last_stmt,branch] : GetLastStmts(stmt_lst[n - 1], true)) {
    pkb_->cfg_al_[last_stmt].insert(while_stmt_no);
    pkb_->reverse_cfg_al_[while_stmt_no].insert(last_stmt);
  }

  //Join last stmt back to while bip
  if (stmt_lst[n - 1]->get_kind() == NodeType::Call) {
    Statement* call_stmt = pkb_->stmt_table_.get_statement(stmt_lst[n - 1]->get_stmt_no());
    int first_stmt_of_called_proc = pkb_->proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    pkb_->cfg_bip_al_[stmt_lst[n - 1]->get_stmt_no()].insert({ first_stmt_of_called_proc, stmt_lst[n - 1]->get_stmt_no() });
    pkb_->cfg_bip_al_[-first_stmt_of_called_proc].insert({ while_stmt_no, -stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[first_stmt_of_called_proc].insert({ stmt_lst[n - 1]->get_stmt_no(), stmt_lst[n - 1]->get_stmt_no() });
    pkb_->reverse_cfg_bip_al_[while_stmt_no].insert({ -first_stmt_of_called_proc, -stmt_lst[n - 1]->get_stmt_no() });
  }
  else {
    for (auto& [last_stmt,branch] : GetLastStmts(stmt_lst[n - 1], false)) {
      pkb_->cfg_bip_al_[last_stmt].insert({ while_stmt_no, branch });
      pkb_->reverse_cfg_bip_al_[while_stmt_no].insert({ last_stmt, branch });
    }
  }
}

/*
void DesignExtractor::LinkProcedures() {
  for (auto &call_stmt : pkb_->stmt_table_.get_statements(NodeType::Call)) {
    std::string called_proc_name = call_stmt->get_called_proc_name();
    int first_stmt_of_proc = pkb_->proc_table_.get_procedure(called_proc_name)->get_stmt_no();
    int call_stmt_no = call_stmt->get_stmt_no();

    std::set<int> ans;
    LastStmtsOfProcedure(called_proc_name, ans);
    std::cout << "last stmts of " << called_proc_name << " are: ";
    for (auto& x : ans)std::cout << x << ' ';
    std::cout << '\n';
    std::set<std::string> procs_visited;
    AddInterprocedureReturnLinks(procs_visited, ans, called_proc_name);
  }
}
void DesignExtractor::AddInterprocedureReturnLinks(std::set<std::string> &visited, std::set<int> &last_stmts, std::string proc_name) {
  for (auto &call_stmt : pkb_->stmt_table_.get_statements(NodeType::Call)) {
    std::string called_proc_name = call_stmt->get_called_proc_name();
    int call_stmt_no = call_stmt->get_stmt_no();
    if (called_proc_name == proc_name) {
        // Update links
        for (auto &last_stmt : last_stmts) {
          if (pkb_->cfg_al_[call_stmt_no].size()) {
            for (auto &next_stmt : pkb_->cfg_al_[call_stmt_no]) {
              pkb_->cfg_bip_al_[last_stmt].insert({ next_stmt, -call_stmt_no });
              pkb_->reverse_cfg_bip_al_[next_stmt].insert({ last_stmt, call_stmt_no });
            }
          }
          else {
            std::string nextproc = call_stmt->get_parent_proc();
            AddInterprocedureReturnLinks(visited, last_stmts, nextproc);
          }
        }
    }
  }
}

*/
void DesignExtractor::AddCallStacks() {
  // Using int for stmt_no, string for call stack
  std::set<std::pair<int, std::string>> visited;
  std::vector<int> call_stack;

  for (auto &stmt : pkb_->stmt_table_.get_all_statements()) {
    if (visited.find({stmt->get_stmt_no(), ""}) != visited.end()) continue;
    ProcessCallStacks(visited, call_stack, stmt->get_stmt_no());
  }
}

void DesignExtractor::ProcessCallStacks(std::set<std::pair<int, std::string>> &visited,
                                        std::vector<int> &call_stack, int u) {
  std::string hash = pkb_->CallStackToString(&call_stack);
  if (visited.find({u, hash}) != visited.end()) return;

  visited.insert({u, hash});
  if (u > 0) {
    auto stmt = pkb_->stmt_table_.get_statement(u);
    stmt->AddCallStack(&call_stack);
  }

  int branch;
  if (call_stack.empty()) {
    branch = PKB::kNoBranch;
  } else {
    branch = call_stack.back();
  }

  for (auto &[v, v_branch] : pkb_->cfg_bip_al_[u]) {
    // Next is dummy end of proc node
    if (v < 0) {
      ProcessCallStacks(visited, call_stack, v);
      continue;
    }
    // Was called from another procedure, and this edge is a returning edge that does not return to that procedure
    if (branch != PKB::kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }

    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      ProcessCallStacks(visited, call_stack, v);
      call_stack.pop_back();
    } else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != PKB::kNoBranch && v_branch == -branch) {
        call_stack.pop_back();
        ProcessCallStacks(visited, call_stack, v);
        call_stack.push_back(branch);
      }
    } else if (v_branch == 0) {
      ProcessCallStacks(visited, call_stack, v);
    }
  }
}
