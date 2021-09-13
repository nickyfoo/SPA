#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <map>

#include "ast.h"

bool IsIdentifierEqual(IdentifierNode *i1, IdentifierNode *i2);
bool IsConstantEqual(ConstantNode *c1, ConstantNode *c2);
bool IsExpressionEqual(ExpressionNode *e1, ExpressionNode *e2);
bool IsRelExpressionEqual(RelExpressionNode *re1, RelExpressionNode *re2);
bool IsCondExpressionEqual(CondExpressionNode *ce1, CondExpressionNode *ce2);
bool IsAssignEqual(AssignNode *a1, AssignNode *a2);
bool IsIfEqual(IfNode *i1, IfNode *i2);
bool IsWhileEqual(WhileNode *w1, WhileNode *w2);
bool IsReadEqual(ReadNode *r1, ReadNode *r2);
bool IsPrintEqual(PrintNode *p1, PrintNode *p2);
bool IsCallEqual(CallNode *c1, CallNode *c2);
bool IsProcedureEqual(ProcedureNode *p1, ProcedureNode *p2);
bool IsProgramEqual(ProgramNode *p1, ProgramNode *p2);
bool IsNodeEqual(Node *t1, Node *t2);
std::vector<Node *> NextNodes(Node *node);
void Visit(Node *node, std::map<NodeType, std::vector<std::function<void(Node *currentNode)>>> functions);

inline bool IsNodeEqual(Node *t1, Node *t2) {
  if (t1->get_kind() != t2->get_kind()) {
    return false;
  }

  switch (t1->get_kind()) {
    case NodeType::Identifier: {
      auto *i1 = dynamic_cast<IdentifierNode *>(t1);
      auto *i2 = dynamic_cast<IdentifierNode *>(t2);
      return IsIdentifierEqual(i1, i2);
    }
    case NodeType::Constant: {
      auto *c1 = dynamic_cast<ConstantNode *>(t1);
      auto *c2 = dynamic_cast<ConstantNode *>(t2);
      return IsConstantEqual(c1, c2);
    }
    case NodeType::Expression: {
      auto *e1 = dynamic_cast<ExpressionNode *>(t1);
      auto *e2 = dynamic_cast<ExpressionNode *>(t2);
      return IsExpressionEqual(e1, e2);
    }
    case NodeType::RelExpression: {
      auto *re1 = dynamic_cast<RelExpressionNode *>(t1);
      auto *re2 = dynamic_cast<RelExpressionNode *>(t2);
      return IsRelExpressionEqual(re1, re2);
    }
    case NodeType::CondExpression: {
      auto *ce1 = dynamic_cast<CondExpressionNode *>(t1);
      auto *ce2 = dynamic_cast<CondExpressionNode *>(t2);
      return IsCondExpressionEqual(ce1, ce2);
    }
    case NodeType::Assign: {
      auto *a1 = dynamic_cast<AssignNode *>(t1);
      auto *a2 = dynamic_cast<AssignNode *>(t2);
      return IsAssignEqual(a1, a2);
    }
    case NodeType::If: {
      auto *i1 = dynamic_cast<IfNode *>(t1);
      auto *i2 = dynamic_cast<IfNode *>(t2);
      return IsIfEqual(i1, i2);
    }
    case NodeType::While: {
      auto *w1 = dynamic_cast<WhileNode *>(t1);
      auto *w2 = dynamic_cast<WhileNode *>(t2);
      return IsWhileEqual(w1, w2);
    }
    case NodeType::Read: {
      auto *r1 = dynamic_cast<ReadNode *>(t1);
      auto *r2 = dynamic_cast<ReadNode *>(t2);
      return IsReadEqual(r1, r2);
    }
    case NodeType::Print: {
      auto *p1 = dynamic_cast<PrintNode *>(t1);
      auto *p2 = dynamic_cast<PrintNode *>(t2);
      return IsPrintEqual(p1, p2);
    }
    case NodeType::Call: {
      auto *c1 = dynamic_cast<CallNode *>(t1);
      auto *c2 = dynamic_cast<CallNode *>(t2);
      return IsCallEqual(c1, c2);
    }
    case NodeType::Procedure: {
      auto *p1 = dynamic_cast<ProcedureNode *>(t1);
      auto *p2 = dynamic_cast<ProcedureNode *>(t2);
      return IsProcedureEqual(p1, p2);
    }
    case NodeType::Program: {
      auto *p1 = dynamic_cast<ProgramNode *>(t1);
      auto *p2 = dynamic_cast<ProgramNode *>(t2);
      return IsProgramEqual(p1, p2);
    }
  }

  return false;
}

inline bool IsIdentifierEqual(IdentifierNode *i1, IdentifierNode *i2) {
  bool metadata_equal = i1->get_line_no() == i2->get_line_no() && i1->get_col_no() == i2->get_col_no();
  bool ident_equal = i1->get_name() == i2->get_name();
  return metadata_equal && ident_equal;
}

inline bool IsConstantEqual(ConstantNode *c1, ConstantNode *c2) {
  bool metadata_equal = c1->get_line_no() == c2->get_line_no() && c1->get_col_no() == c2->get_col_no();
  bool const_equal = c1->get_value() == c2->get_value();
  return metadata_equal && const_equal;
}

inline bool IsExpressionEqual(ExpressionNode *e1, ExpressionNode *e2) {
  bool metadata_equal = e1->get_line_no() == e2->get_line_no() && e1->get_col_no() == e2->get_col_no();
  bool expr_equal = e1->get_expr_string() == e2->get_expr_string() && IsNodeEqual(e1->get_left(), e2->get_left()) &&
      IsNodeEqual(e1->get_right(), e2->get_right());
  return metadata_equal && expr_equal;
}

inline bool IsRelExpressionEqual(RelExpressionNode *re1, RelExpressionNode *re2) {
  bool metadata_equal = re1->get_line_no() == re2->get_line_no() && re1->get_col_no() == re2->get_col_no();
  bool rel_equal = re1->get_op() == re2->get_op() && IsNodeEqual(re1->get_left(), re2->get_left()) &&
      IsNodeEqual(re1->get_right(), re2->get_right());
  return metadata_equal && rel_equal;
}

inline bool IsCondExpressionEqual(CondExpressionNode *ce1, CondExpressionNode *ce2) {
  bool metadata_equal = ce1->get_line_no() == ce2->get_line_no() && ce1->get_col_no() == ce2->get_col_no();
  bool cond_equal = ce1->get_op() == ce2->get_op() && IsNodeEqual(ce1->get_left(), ce2->get_left()) &&
      IsNodeEqual(ce1->get_right(), ce2->get_right());
  return metadata_equal && cond_equal;
}

inline bool IsAssignEqual(AssignNode *a1, AssignNode *a2) {
  bool metadata_equal = a1->get_line_no() == a2->get_line_no() && a1->get_col_no() == a2->get_col_no() &&
      a1->get_stmt_no() == a2->get_stmt_no();
  bool assign_equal =
      IsIdentifierEqual(a1->get_var(), a2->get_var()) && IsNodeEqual(a1->expr(), a2->expr());
  return metadata_equal && assign_equal;
}

inline bool IsIfEqual(IfNode *i1, IfNode *i2) {
  bool metadata_equal = i1->get_line_no() == i2->get_line_no() && i1->get_col_no() == i2->get_col_no() &&
      i1->get_stmt_no() == i2->get_stmt_no();

  bool then_equal = true;
  auto then_list_1 = i1->get_then_stmt_lst();
  auto then_list_2 = i2->get_then_stmt_lst();

  if (then_list_1.size() != then_list_2.size()) {
    return false;
  }
  for (int idx = 0; idx < then_list_1.size(); idx++) {
    Node *n1 = then_list_1[idx];
    Node *n2 = then_list_2[idx];
    if (!IsNodeEqual(n1, n2)) {
      then_equal = false;
      break;
    }
  }

  bool else_equal = true;
  auto else_list_1 = i1->get_else_stmt_lst();
  auto else_list_2 = i2->get_else_stmt_lst();

  if (else_list_1.size() != else_list_2.size()) {
    return false;
  }
  for (int idx = 0; idx < then_list_1.size(); idx++) {
    Node *n1 = then_list_1[idx];
    Node *n2 = then_list_2[idx];
    if (!IsNodeEqual(n1, n2)) {
      else_equal = false;
      break;
    }
  }

  bool if_equal = IsNodeEqual(i1->get_cond(), i2->get_cond()) && then_equal && else_equal;
  return metadata_equal && if_equal;
}

inline bool IsWhileEqual(WhileNode *w1, WhileNode *w2) {
  bool metadata_equal = w1->get_line_no() == w2->get_line_no() && w1->get_col_no() == w2->get_col_no() &&
      w1->get_stmt_no() == w2->get_stmt_no();

  bool stmt_lst_equal = true;
  auto stmt_lst_1 = w1->get_stmt_list();
  auto stmt_lst_2 = w2->get_stmt_list();

  if (stmt_lst_1.size() != stmt_lst_2.size()) {
    return false;
  }
  for (int idx = 0; idx < stmt_lst_1.size(); idx++) {
    Node *n1 = stmt_lst_1[idx];
    Node *n2 = stmt_lst_2[idx];
    if (!IsNodeEqual(n1, n2)) {
      stmt_lst_equal = false;
      break;
    }
  }

  return IsNodeEqual(w1->get_cond(), w2->get_cond()) && metadata_equal && stmt_lst_equal;
}

inline bool IsReadEqual(ReadNode *r1, ReadNode *r2) {
  bool metadata_equal = r1->get_line_no() == r2->get_line_no() && r1->get_col_no() == r2->get_col_no() &&
      r1->get_stmt_no() == r2->get_stmt_no();
  bool read_equal = IsIdentifierEqual(r1->get_var(), r2->get_var());
  return metadata_equal && read_equal;
}

inline bool IsPrintEqual(PrintNode *p1, PrintNode *p2) {
  bool metadata_equal = p1->get_line_no() == p2->get_line_no() && p1->get_col_no() == p2->get_col_no() &&
      p1->get_stmt_no() == p2->get_stmt_no();
  bool print_equal = IsIdentifierEqual(p1->get_var(), p2->get_var());
  return metadata_equal && print_equal;
}

inline bool IsCallEqual(CallNode *c1, CallNode *c2) {
  bool metadata_equal = c1->get_line_no() == c2->get_line_no() && c1->get_col_no() == c2->get_col_no() &&
      c1->get_stmt_no() == c2->get_stmt_no();
  bool call_equal = IsIdentifierEqual(c1->get_proc(), c2->get_proc());
  return metadata_equal && call_equal;
}

inline bool IsProcedureEqual(ProcedureNode *p1, ProcedureNode *p2) {
  bool metadata_equal = p1->get_line_no() == p2->get_line_no() && p1->get_col_no() == p2->get_col_no();

  bool stmt_lst_equal = true;
  auto stmt_lst_1 = p1->get_stmt_lst();
  auto stmt_lst_2 = p2->get_stmt_lst();

  if (stmt_lst_1.size() != stmt_lst_2.size()) {
    return false;
  }
  for (int idx = 0; idx < stmt_lst_1.size(); idx++) {
    Node *n1 = stmt_lst_1[idx];
    Node *n2 = stmt_lst_2[idx];
    if (!IsNodeEqual(n1, n2)) {
      stmt_lst_equal = false;
      break;
    }
  }

  bool procedure_equal = p1->get_name() == p2->get_name() && stmt_lst_equal;
  return metadata_equal && procedure_equal;
}

inline bool IsProgramEqual(ProgramNode *p1, ProgramNode *p2) {
  bool metadata_equal = p1->get_line_no() == p2->get_line_no() && p1->get_col_no() == p2->get_col_no();

  bool proc_lst_equal = true;
  auto proc_lst_1 = p1->get_procedures();
  auto proc_lst_2 = p2->get_procedures();

  if (proc_lst_1.size() != proc_lst_2.size()) {
    return false;
  }
  for (int idx = 0; idx < proc_lst_1.size(); idx++) {
    ProcedureNode *n1 = proc_lst_1[idx];
    ProcedureNode *n2 = proc_lst_2[idx];
    if (!IsProcedureEqual(n1, n2)) {
      proc_lst_equal = false;
      break;
    }
  }

  return metadata_equal && proc_lst_equal;
}

std::vector<Node *> NextNodes(Node *node) {
  std::vector<Node *> next_nodes;
  switch (node->get_kind()) {
    case Identifier:
    case Constant:break;
    case Expression: {
      auto *expression_node = (ExpressionNode *) node;
      next_nodes.push_back(expression_node->get_left());
      next_nodes.push_back(expression_node->get_right());
      break;
    }
    case RelExpression: {
      auto *expression_node = (RelExpressionNode *) node;
      next_nodes.push_back(expression_node->get_left());
      next_nodes.push_back(expression_node->get_right());
      break;
    }
    case CondExpression: {
      auto *expression_node = (CondExpressionNode *) node;
      next_nodes.push_back(expression_node->get_left());
      next_nodes.push_back(expression_node->get_right());
      break;
    }
    case Assign: {
      auto *assign_node = (AssignNode *) node;
      next_nodes.push_back(assign_node->get_var());
      next_nodes.push_back(assign_node->expr());
      break;
    }
    case If: {
      auto *if_node = (IfNode *) node;
      next_nodes.push_back(if_node->get_cond());
      for (Node *n : if_node->get_then_stmt_lst()) {
        next_nodes.push_back(n);
      }
      for (Node *n : if_node->get_else_stmt_lst()) {
        next_nodes.push_back(n);
      }
      break;
    }
    case While: {
      auto *while_node = (WhileNode *) node;
      next_nodes.push_back(while_node->get_cond());
      for (Node *n : while_node->get_stmt_list()) {
        next_nodes.push_back(n);
      }
      break;
    }
    case Read: {
      auto *read_node = (ReadNode *) node;
      next_nodes.push_back(read_node->get_var());
      break;
    }
    case Print: {
      auto *print_node = (PrintNode *) node;
      next_nodes.push_back(print_node->get_var());
      break;
    }
    case Call: {
      auto *call_node = (CallNode *) node;
      next_nodes.push_back(call_node->get_proc());
      break;
    }
    case Procedure: {
      auto *procedure_node = (ProcedureNode *) node;
      for (Node *n : procedure_node->get_stmt_lst()) {
        next_nodes.push_back(n);
      }
      break;
    }
    case Program: {
      auto *program_node = (ProgramNode *) node;
      for (Node *n : program_node->get_procedures()) {
        next_nodes.push_back(n);
      }
      break;
    }
  }
  return next_nodes;
}

void Visit(Node *node, std::map<NodeType, std::vector<std::function<void(Node *currentNode)>>> functions) {
  // TODO: throw an error
  if (node == nullptr) return;

  // Execute the corresponding function taking the node as a parameter
  for (const auto &func : functions[node->get_kind()]) func(node);

  // Continue traversing the AST
  for (Node *n : NextNodes(node)) {
    if (n) Visit(n, functions);
  }
}