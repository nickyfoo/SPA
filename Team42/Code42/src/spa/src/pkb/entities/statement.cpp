#include "statement.h"
#include <iostream>

Statement::Statement(int line_no, ast::Kind type) {
  this->stmt_no_ = line_no;
  this->kind_ = type;
}

Statement::Statement() = default;

Statement::~Statement() = default;

int Statement::get_stmt_no() {
  return stmt_no_;
}

int Statement::get_stmt_no(ast::Node *node) {
  switch (node->kind) {
    case ast::Assign: {
      ast::AssignNode *castedAssignNode{static_cast<ast::AssignNode *>(node)};
      return castedAssignNode->stmtNo;
    }
    case ast::If: {
      ast::IfNode *castedIfNode{static_cast<ast::IfNode *>(node)};
      return castedIfNode->stmtNo;
    }
    case ast::While: {
      ast::WhileNode *castedWhileNode{static_cast<ast::WhileNode *>(node)};
      return castedWhileNode->stmtNo;
    }
    case ast::Read: {
      ast::ReadNode *castedReadNode{static_cast<ast::ReadNode *>(node)};
      return castedReadNode->stmtNo;
    }
    case ast::Print: {
      ast::PrintNode *castedPrintNode{static_cast<ast::PrintNode *>(node)};
      return castedPrintNode->stmtNo;
    }
    case ast::Call: {
      ast::CallNode *castedCallNode{static_cast<ast::CallNode *>(node)};
      return castedCallNode->stmtNo;
    }
    default: {
      return 0;
    }
  }
};

ast::Kind Statement::get_kind() {
  return kind_;
}

std::string Statement::get_expr_string() {
  // TODO: Only applicable for AssignStatement for now, need to add if/while
  return expr_string_;
}

std::set<int> *Statement::get_followers() {
  return &followers_;
}

std::set<int> *Statement::get_followers_star() {
  return &followers_star_;
}

std::set<int> *Statement::get_followees() {
  return &followees_;
}

std::set<int> *Statement::get_followees_star() {
  return &followees_star_;
}

std::set<int> *Statement::get_parents() {
  return &parents_;
}

std::set<int> *Statement::get_parents_star() {
  return &parents_star_;
}

std::set<int> *Statement::get_children() {
  return &children_;
}

std::set<int> *Statement::get_children_star() {
  return &children_star_;
}

void Statement::set_expr_string(std::string expr_string) {
  this->expr_string_ = expr_string;
}

void Statement::AddFollower(int line_no) {
  followers_.insert(line_no);
}

void Statement::AddFollowerStar(int line_no) {
  followers_star_.insert(line_no);
}

void Statement::AddFollowee(int line_no) {
  followees_.insert(line_no);
}

void Statement::AddFolloweeStar(int line_no) {
  followees_star_.insert(line_no);
}

void Statement::AddParent(int line_no) {
  parents_.insert(line_no);
}

void Statement::AddParentStar(int line_no) {
  parents_star_.insert(line_no);
}

void Statement::AddChild(int line_no) {
  children_.insert(line_no);
}

void Statement::AddChildStar(int line_no) {
  children_star_.insert(line_no);
}

void Statement::FollowsInfo() {
  std::cout << "Statement " << stmt_no_ << " is followed by:\n";
  std::cout << "Followers: ";
  for (auto &x : followers_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "FollowersStar: ";
  for (auto &x : followers_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Statement " << stmt_no_ << " is a followee of/follows:\n";
  std::cout << "Followees: ";
  for (auto &x : followees_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "FolloweesStar: ";
  for (auto &x : followees_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}

void Statement::ParentInfo() {
  std::cout << "Statement " << stmt_no_ << " has parents:\n";
  std::cout << "Parents: ";
  for (auto &x : parents_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "ParentsStar: ";
  for (auto &x : parents_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Statement " << stmt_no_ << " has children:\n";
  std::cout << "Children: ";
  for (auto &x : children_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "ChildrenStar: ";
  for (auto &x : children_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}
