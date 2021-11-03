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

std::set<int> DesignExtractor::GetLastStmts(StatementNode *node) {
  std::set<int> ans;
  if (node->get_kind() == NodeType::If) {
    auto *if_node = dynamic_cast<IfNode *>(node);

    // Sort then statement list in ascending statement numbers
    std::vector<StatementNode *> then_stmt_lst = if_node->get_then_stmt_lst();
    std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
                return a->get_stmt_no() > b->get_stmt_no();
              });
    for (auto &stmt : GetLastStmts(*(then_stmt_lst.begin()))) {
      ans.insert(stmt);
    }

    // Sort else statement list in ascending statement numbers
    std::vector<StatementNode *> else_stmt_lst = if_node->get_else_stmt_lst();
    std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
                return a->get_stmt_no() > b->get_stmt_no();
              });
    for (auto &stmt : GetLastStmts(*(else_stmt_lst.begin()))) {
      ans.insert(stmt);
    }
  } else {
    ans.insert(node->get_stmt_no());
  }

  return ans;
}

void DesignExtractor::LastStmtsOfProcedure(int u, std::set<int> &visited, std::set<int> &ans) {
  if (visited.find(u) != visited.end()) return;
  visited.insert(u);

  if (pkb_->stmt_table_.get_statement(u)->get_kind() == NodeType::While
      && pkb_->cfg_al_.find(u) != pkb_->cfg_al_.end()
      && pkb_->cfg_al_[u].size() == 1) {
    ans.insert(u);
    return;
  }

  if (pkb_->cfg_al_.find(u) == pkb_->cfg_al_.end()) {
    ans.insert(u);
    return;
  }

  for (auto &v : pkb_->cfg_al_[u]) {
    LastStmtsOfProcedure(v, visited, ans);
  }
}

void DesignExtractor::CFGProcessProcedureNode(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);

  // Sort statement list in ascending statement numbers
  std::vector<StatementNode *> stmt_lst = procedure_node->get_stmt_lst();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
              return a->get_stmt_no() < b->get_stmt_no();
            });

  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto &last_stmt : GetLastStmts(stmt_lst[i - 1])) {
      pkb_->cfg_al_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      pkb_->reverse_cfg_al_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);

      if (stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        pkb_->cfg_bip_al_[last_stmt].insert({stmt_lst[i]->get_stmt_no(), PKB::kNoBranch});
        pkb_->reverse_cfg_bip_al_[stmt_lst[i]->get_stmt_no()].insert({last_stmt, PKB::kNoBranch});
      }
    }
  }

  for (auto &stmt : stmt_lst) {
    if (stmt->get_kind() == NodeType::Call) {
      auto *call_node = dynamic_cast<CallNode *>(stmt);
      std::string proc_name = call_node->get_proc()->get_name();
      int call_stmt_no = call_node->get_stmt_no();
      int first_stmt_no = pkb_->proc_table_.get_procedure(proc_name)->get_stmt_no();

      // Add edge from call node to start of procedure
      pkb_->cfg_bip_al_[call_stmt_no].insert({first_stmt_no, call_stmt_no});
      pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({call_stmt_no, -first_stmt_no});
    }
  }
}

void DesignExtractor::CFGProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  int if_stmt_no = if_node->get_stmt_no();

  // Sort then statement list in ascending statement numbers
  std::vector<StatementNode *> then_stmt_lst = if_node->get_then_stmt_lst();
  std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
              return a->get_stmt_no() < b->get_stmt_no();
            });

  // Connect if node to first node of then_stmt_lst
  if (!then_stmt_lst.empty()) {
    int then_first_stmt_no = then_stmt_lst[0]->get_stmt_no();

    pkb_->cfg_al_[if_stmt_no].insert(then_first_stmt_no);
    pkb_->reverse_cfg_al_[then_first_stmt_no].insert(if_stmt_no);
    pkb_->cfg_bip_al_[if_stmt_no].insert({then_first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[then_first_stmt_no].insert({if_stmt_no, PKB::kNoBranch});
  }

  // Connect GetLastStmts of stmt_lst to next in then_stmt_lst
  for (int i = 1; i < then_stmt_lst.size(); i++) {
    int stmt_no = then_stmt_lst[i]->get_stmt_no();
    for (auto &last_stmt : GetLastStmts(then_stmt_lst[i - 1])) {
      pkb_->cfg_al_[last_stmt].insert(stmt_no);
      pkb_->reverse_cfg_al_[stmt_no].insert(last_stmt);

      if (then_stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        pkb_->cfg_bip_al_[last_stmt].insert({stmt_no, PKB::kNoBranch});
        pkb_->reverse_cfg_bip_al_[stmt_no].insert({last_stmt, PKB::kNoBranch});
      }
    }
  }

  for (auto &stmt : then_stmt_lst) {
    if (stmt->get_kind() == NodeType::Call) {
      auto *call_node = dynamic_cast<CallNode *>(stmt);
      std::string proc_name = call_node->get_proc()->get_name();
      int call_stmt_no = call_node->get_stmt_no();
      int first_stmt_no = pkb_->proc_table_.get_procedure(proc_name)->get_stmt_no();

      // Add edge from call node to start of procedure
      pkb_->cfg_bip_al_[call_stmt_no].insert({first_stmt_no, call_stmt_no});
      pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({call_stmt_no, -first_stmt_no});
    }
  }

  // Sort else statement list in ascending statement numbers
  std::vector<StatementNode *> else_stmt_lst = if_node->get_else_stmt_lst();
  std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
              return a->get_stmt_no() < b->get_stmt_no();
            });

  // Connect if node to first node of else_stmt_lst
  if (!else_stmt_lst.empty()) {
    int else_first_stmt_no = else_stmt_lst[0]->get_stmt_no();

    pkb_->cfg_al_[if_stmt_no].insert(else_first_stmt_no);
    pkb_->reverse_cfg_al_[else_first_stmt_no].insert(if_stmt_no);
    pkb_->cfg_bip_al_[if_stmt_no].insert({else_first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[else_first_stmt_no].insert({if_stmt_no, PKB::kNoBranch});
  }

  // Connect GetLastStmts of stmt_lst to next in else_stmt_lst
  for (int i = 1; i < else_stmt_lst.size(); i++) {
    int stmt_no = else_stmt_lst[i]->get_stmt_no();

    for (auto &last_stmt : GetLastStmts(else_stmt_lst[i - 1])) {
      pkb_->cfg_al_[last_stmt].insert(stmt_no);

      if (else_stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        pkb_->cfg_bip_al_[last_stmt].insert({stmt_no, PKB::kNoBranch});
        pkb_->reverse_cfg_bip_al_[stmt_no].insert({last_stmt, PKB::kNoBranch});
      }
    }
  }

  for (auto &stmt : else_stmt_lst) {
    if (stmt->get_kind() == NodeType::Call) {
      auto *call_node = dynamic_cast<CallNode *>(stmt);
      std::string proc_name = call_node->get_proc()->get_name();
      int call_stmt_no = call_node->get_stmt_no();
      int first_stmt_no = pkb_->proc_table_.get_procedure(proc_name)->get_stmt_no();

      // Add edge from call node to start of procedure
      pkb_->cfg_bip_al_[call_stmt_no].insert({first_stmt_no, call_stmt_no});
      pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({call_stmt_no, -first_stmt_no});
    }
  }
}

void DesignExtractor::CFGProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  int while_stmt_no = while_node->get_stmt_no();

  // Sort statement list in ascending statement numbers
  std::vector<StatementNode *> stmt_lst = while_node->get_stmt_list();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
              return a->get_stmt_no() < b->get_stmt_no();
            });

  // Connect while node to first node of stmt_lst
  if (!stmt_lst.empty()) {
    int first_stmt_no = stmt_lst[0]->get_stmt_no();
    pkb_->cfg_al_[while_stmt_no].insert(first_stmt_no);
    pkb_->reverse_cfg_al_[first_stmt_no].insert(while_stmt_no);
    pkb_->cfg_bip_al_[while_stmt_no].insert({first_stmt_no, PKB::kNoBranch});
    pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({while_stmt_no, PKB::kNoBranch});
  }

  // Connect GetLastStmts of stmt_lst to next in stmt_lst
  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto &last_stmt : GetLastStmts(stmt_lst[i - 1])) {
      int stmt_no = stmt_lst[i]->get_stmt_no();

      pkb_->cfg_al_[last_stmt].insert(stmt_no);
      pkb_->reverse_cfg_al_[stmt_no].insert(last_stmt);

      if (stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        pkb_->cfg_bip_al_[last_stmt].insert({stmt_no, PKB::kNoBranch});
        pkb_->reverse_cfg_bip_al_[stmt_no].insert({last_stmt, PKB::kNoBranch});
      }
    }
  }

  // Connect GetLastStmts of stmt_lst to while node
  if (!stmt_lst.empty()) {
    for (auto &last_stmt : GetLastStmts(stmt_lst[stmt_lst.size() - 1])) {
      pkb_->cfg_al_[last_stmt].insert(while_stmt_no);
      pkb_->reverse_cfg_al_[while_stmt_no].insert(last_stmt);

      if (pkb_->stmt_table_.get_statement(last_stmt)->get_kind() != NodeType::Call) {
        // TODO(Nicholas): Dangerous, might mess something up
        pkb_->cfg_bip_al_[last_stmt].insert({while_stmt_no, PKB::kNoBranch});
        pkb_->reverse_cfg_bip_al_[while_stmt_no].insert({last_stmt, PKB::kNoBranch});
      }
    }
  }

  for (auto &stmt : stmt_lst) {
    if (stmt->get_kind() == NodeType::Call) {
      auto *call_node = dynamic_cast<CallNode *>(stmt);
      std::string proc_name = call_node->get_proc()->get_name();
      int call_stmt_no = call_node->get_stmt_no();
      int first_stmt_no = pkb_->proc_table_.get_procedure(proc_name)->get_stmt_no();

      // Add edge from call node to start of procedure
      pkb_->cfg_bip_al_[call_stmt_no].insert({first_stmt_no, call_stmt_no});
      pkb_->reverse_cfg_bip_al_[first_stmt_no].insert({call_stmt_no, -first_stmt_no});
    }
  }
}
void DesignExtractor::LinkProcedures() {
  for (auto &call_stmt : pkb_->stmt_table_.get_statements(NodeType::Call)) {
    std::string called_proc_name = call_stmt->get_called_proc_name();
    int first_stmt_of_proc = pkb_->proc_table_.get_procedure(called_proc_name)->get_stmt_no();
    int call_stmt_no = call_stmt->get_stmt_no();

    std::set<int> visited, ans;
    LastStmtsOfProcedure(first_stmt_of_proc, visited, ans);

    for (auto &last_stmt : ans) {
      for (auto &next_stmt : pkb_->cfg_al_[call_stmt_no]) {
        pkb_->cfg_bip_al_[last_stmt].insert({next_stmt, -call_stmt_no});
        pkb_->reverse_cfg_bip_al_[next_stmt].insert({last_stmt, call_stmt_no});
      }
    }
  }
}

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
  auto stmt = pkb_->stmt_table_.get_statement(u);
  stmt->AddCallStack(&call_stack);

  int branch;
  if (call_stack.empty()) {
    branch = PKB::kNoBranch;
  } else {
    branch = call_stack.back();
  }

  for (auto &[v, v_branch] : pkb_->cfg_bip_al_[u]) {
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
