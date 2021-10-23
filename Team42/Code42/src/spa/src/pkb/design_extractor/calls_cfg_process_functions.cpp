#include "pkb.h"
#include <algorithm>
#include <vector>
#include "ast_utils.hpp"
#include "pkb_exception.h"

void PKB::CallsProcessCallNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *call_node = dynamic_cast<CallNode *>(node);
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      auto *calling_procedure = proc_table_.get_procedure(procedure_node->get_name());
      calling_procedure->AddCalls(call_node->get_proc()->get_name());
      auto *called_procedure = proc_table_.get_procedure(call_node->get_proc()->get_name());
      if (called_procedure == nullptr) {
        throw PKBException("Called an undefined procedure: " + call_node->get_proc()->get_name());
      }
      called_procedure->AddCallers(procedure_node->get_name());
    }
  }
}

std::set<int> PKB::LastStmts(StatementNode *node) {
  std::set<int> ans;
  if(node->get_kind() == NodeType::If) {
    auto *if_node = dynamic_cast<IfNode*>(node);
    // Sort stmt list in ascending order,
    std::vector<StatementNode*> then_stmt_lst = if_node->get_then_stmt_lst();
    std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() > b->get_stmt_no();
    });
    for (auto &stmt : (LastStmts(*(then_stmt_lst.begin())))) {
      ans.insert(stmt);
    }

    std::vector<StatementNode*> else_stmt_lst = if_node->get_else_stmt_lst();
    std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
              [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() > b->get_stmt_no();
    });
    for (auto &stmt : (LastStmts(*(else_stmt_lst.begin())))) {
      ans.insert(stmt);
    }
  } else {
    ans.insert(node->get_stmt_no());
  }
  return ans;
}

void PKB::LastStmtsOfProcedure(int u, std::set<int>&visited, std::set<int>&ans) {
  if (visited.find(u) != visited.end()) return;
  visited.insert(u);
  if (stmt_table_.get_statement(u)->get_kind() == NodeType::While && cfg_al_.find(u)!=cfg_al_.end() && cfg_al_[u].size()==1){
    ans.insert(u);
    return;
  }
  if (cfg_al_.find(u) == cfg_al_.end()) {
    ans.insert(u);
    return;
  }
  for (auto& v : cfg_al_[u]) {
    LastStmtsOfProcedure(v, visited, ans);
  }
}

void PKB::CFGProcessProcedureNode(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode*>(node);
  std::vector<StatementNode*> stmt_lst = procedure_node->get_stmt_lst();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
    return a->get_stmt_no() < b->get_stmt_no();
  });
  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto& last_stmt : LastStmts(stmt_lst[i - 1])) {
      cfg_al_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      reverse_cfg_al_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);
      if (stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        cfg_bip_al_[last_stmt].insert({ stmt_lst[i]->get_stmt_no(), kNoBranch });
        reverse_cfg_bip_al_[stmt_lst[i]->get_stmt_no()].insert({ last_stmt, kNoBranch });
      }
    }
  }

  for (int i = 0; i < stmt_lst.size(); i++) {
    if (stmt_lst[i]->get_kind() == NodeType::Call) {
      auto *call_node = dynamic_cast<CallNode*>(stmt_lst[i]);
      std::string proc_name = call_node->get_proc()->get_name();
      int first_stmt = proc_table_.get_procedure(proc_name)->get_stmt_no();
      // Add edge from call node to start of procedure
      cfg_bip_al_[call_node->get_stmt_no()].insert({ first_stmt, call_node->get_stmt_no() });
      reverse_cfg_bip_al_[first_stmt].insert({ call_node->get_stmt_no(), -first_stmt });
    }
  }
}

void PKB::CFGProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode*>(node);
  std::vector<StatementNode*> then_stmt_lst = if_node->get_then_stmt_lst();
  std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
    return a->get_stmt_no() < b->get_stmt_no();
  });

  // Connect if node to first node of then_stmt_lst
  if (then_stmt_lst.size()) {
    cfg_al_[if_node->get_stmt_no()].insert(then_stmt_lst[0]->get_stmt_no());
    reverse_cfg_al_[then_stmt_lst[0]->get_stmt_no()].insert(if_node->get_stmt_no());
    cfg_bip_al_[if_node->get_stmt_no()].insert({ then_stmt_lst[0]->get_stmt_no(), kNoBranch });
    reverse_cfg_bip_al_[then_stmt_lst[0]->get_stmt_no()].insert({ if_node->get_stmt_no(),kNoBranch });
  }

  // Connect LastStmts of stmt_lst to next in then_stmt_lst
  for (int i = 1; i < then_stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(then_stmt_lst[i - 1])) {
      cfg_al_[last_stmt].insert(then_stmt_lst[i]->get_stmt_no());
      reverse_cfg_al_[then_stmt_lst[i]->get_stmt_no()].insert(last_stmt);
      if (then_stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        cfg_bip_al_[last_stmt].insert({ then_stmt_lst[i]->get_stmt_no(), kNoBranch });
        reverse_cfg_bip_al_[then_stmt_lst[i]->get_stmt_no()].insert({ last_stmt, kNoBranch });
      }
    }
  }

  for (int i = 0; i < then_stmt_lst.size(); i++) {
    if (then_stmt_lst[i]->get_kind() == NodeType::Call) {
      auto* call_node = dynamic_cast<CallNode*>(then_stmt_lst[i]);
      std::string proc_name = call_node->get_proc()->get_name();
      int first_stmt = proc_table_.get_procedure(proc_name)->get_stmt_no();
      // Add edge from call node to start of procedure
      cfg_bip_al_[call_node->get_stmt_no()].insert({ first_stmt, call_node->get_stmt_no() });
      reverse_cfg_bip_al_[first_stmt].insert({ call_node->get_stmt_no(), -first_stmt });
    }
  }

  std::vector<StatementNode*> else_stmt_lst = if_node->get_else_stmt_lst();
  std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
    return a->get_stmt_no() < b->get_stmt_no();
  });

  // Connect if node to first node of else_stmt_lst
  if (else_stmt_lst.size()) {
    cfg_al_[if_node->get_stmt_no()].insert(else_stmt_lst[0]->get_stmt_no());
    reverse_cfg_al_[else_stmt_lst[0]->get_stmt_no()].insert(if_node->get_stmt_no());
    cfg_bip_al_[if_node->get_stmt_no()].insert({ else_stmt_lst[0]->get_stmt_no(), kNoBranch });
    reverse_cfg_bip_al_[else_stmt_lst[0]->get_stmt_no()].insert({ if_node->get_stmt_no(),kNoBranch });
  }

  // Connect LastStmts of stmt_lst to next in else_stmt_lst
  for (int i = 1; i < else_stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(else_stmt_lst[i - 1])) {
      cfg_al_[last_stmt].insert(else_stmt_lst[i]->get_stmt_no());
      if (else_stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        cfg_bip_al_[last_stmt].insert({ else_stmt_lst[i]->get_stmt_no(), kNoBranch });
        reverse_cfg_bip_al_[else_stmt_lst[i]->get_stmt_no()].insert({ last_stmt, kNoBranch });
      }
    }
  }

  for (int i = 0; i < else_stmt_lst.size(); i++) {
    if (else_stmt_lst[i]->get_kind() == NodeType::Call) {
      auto* call_node = dynamic_cast<CallNode*>(else_stmt_lst[i]);
      std::string proc_name = call_node->get_proc()->get_name();
      int first_stmt = proc_table_.get_procedure(proc_name)->get_stmt_no();
      // Add edge from call node to start of procedure
      cfg_bip_al_[call_node->get_stmt_no()].insert({ first_stmt, call_node->get_stmt_no() });
      reverse_cfg_bip_al_[first_stmt].insert({ call_node->get_stmt_no(), -first_stmt });
    }
  }
}

void PKB::CFGProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode*>(node);
  std::vector<StatementNode*> stmt_lst = while_node->get_stmt_list();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
            [](StatementNode *a, StatementNode *b) {
    return a->get_stmt_no() < b->get_stmt_no();
  });

  // Connect while node to first node of stmt_lst
  if (stmt_lst.size()) {
    cfg_al_[while_node->get_stmt_no()].insert(stmt_lst[0]->get_stmt_no());
    reverse_cfg_al_[stmt_lst[0]->get_stmt_no()].insert(while_node->get_stmt_no());
    cfg_bip_al_[while_node->get_stmt_no()].insert({ stmt_lst[0]->get_stmt_no(), kNoBranch });
    reverse_cfg_bip_al_[stmt_lst[0]->get_stmt_no()].insert({ while_node->get_stmt_no(), kNoBranch });
  }

  // Connect LastStmts of stmt_lst to next in stmt_lst
  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(stmt_lst[i - 1])) {
      cfg_al_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      reverse_cfg_al_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);
      if (stmt_lst[i - 1]->get_kind() != NodeType::Call) {
        cfg_bip_al_[last_stmt].insert({ stmt_lst[i]->get_stmt_no(), kNoBranch });
        reverse_cfg_bip_al_[stmt_lst[i]->get_stmt_no()].insert({ last_stmt, kNoBranch });
      }
    }
  }

  // Connect LastStmts of stmt_lst to while node
  if (stmt_lst.size()) {
    for (auto& last_stmt : LastStmts(stmt_lst[stmt_lst.size() - 1])) {
      cfg_al_[last_stmt].insert(while_node->get_stmt_no());
      reverse_cfg_al_[while_node->get_stmt_no()].insert(last_stmt);
      if (stmt_table_.get_statement(last_stmt)->get_kind() != NodeType::Call) {
        cfg_bip_al_[last_stmt].insert({ while_node->get_stmt_no(), kNoBranch }); // todo: dangerous, might fk sth up
        reverse_cfg_bip_al_[while_node->get_stmt_no()].insert({ last_stmt, kNoBranch });
      }
    }
  }

  for (int i = 0; i < stmt_lst.size(); i++) {
    if (stmt_lst[i]->get_kind() == NodeType::Call) {
      auto* call_node = dynamic_cast<CallNode*>(stmt_lst[i]);
      std::string proc_name = call_node->get_proc()->get_name();
      int first_stmt = proc_table_.get_procedure(proc_name)->get_stmt_no();
      // Add edge from call node to start of procedure
      cfg_bip_al_[call_node->get_stmt_no()].insert({ first_stmt, call_node->get_stmt_no() });
      reverse_cfg_bip_al_[first_stmt].insert({call_node->get_stmt_no(), -first_stmt});
    }
  }
}
void PKB::LinkProcedures() {
  for (auto& call_stmt : stmt_table_.get_statements(NodeType::Call)) {
    int first_stmt_of_proc = proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    std::set<int> visited, ans;
    LastStmtsOfProcedure(first_stmt_of_proc, visited, ans);
    for (auto& last_stmt : ans) {
      for (auto& next_stmt : cfg_al_[call_stmt->get_stmt_no()]) {
        cfg_bip_al_[last_stmt].insert({ next_stmt, -call_stmt->get_stmt_no() });
        reverse_cfg_bip_al_[next_stmt].insert({ last_stmt, call_stmt->get_stmt_no() });
      }
    }
  }
}


void PKB::AddCallStacks() {
  int n = stmt_table_.get_num_statements() + 1;
  //int for stmt_no, string for call stack.
  std::set<std::pair<int,std::string>> visited;
  std::vector<int> call_stack;
  for (auto& stmt : stmt_table_.get_all_statements()) {
    if (visited.find({ stmt->get_stmt_no(), "" }) != visited.end()) continue;
    ProcessCallStacks(visited, call_stack, stmt->get_stmt_no());
  }
}

void PKB::ProcessCallStacks(std::set<std::pair<int, std::string>>& visited, std::vector<int>& call_stack, int u) { 
  std::string hash = CallStackToString(&call_stack);
  if (visited.find({ u, hash }) != visited.end()) return;
  visited.insert({ u,hash });
  auto stmt = stmt_table_.get_statement(u);
  stmt->AddCallStack(&call_stack);
  int branch;
  if (call_stack.size() == 0) {
    branch = kNoBranch;
  }
  else {
    branch = call_stack.back();
  }
  for (auto& [v, v_branch] : cfg_bip_al_[u]) {
    // Was called from another procedure, and this edge is a returning edge that does not return to that procedure
    if (branch != kNoBranch && v_branch < 0 && v_branch != -branch) {
      continue;
    }
    // Edge goes to a new procedure
    if (v_branch > 0) {
      call_stack.push_back(v_branch);
      ProcessCallStacks(visited, call_stack, v);
      call_stack.pop_back();
    }
    else if (v_branch < 0) {
      // Call stack not empty and edge goes back
      if (branch != kNoBranch) {
        // Edge goes back
        if (v_branch == -branch) {
          call_stack.pop_back();
          ProcessCallStacks(visited, call_stack, v);
          call_stack.push_back(branch);
        }
        // Else checked above
      }
      // Call stack empty, go wherever you want
      else {
      }
    }
    else if (v_branch == 0) {
      ProcessCallStacks(visited, call_stack, v);
    }
  }
}

std::string PKB::CallStackToString(std::vector<int>* call_stack) {
  std::string ans = "";
  for (auto& x : *call_stack) {
    ans += std::to_string(x);
    ans += ' ';
  }
  return ans;
}