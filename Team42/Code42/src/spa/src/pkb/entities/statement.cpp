#include "statement.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

Statement::Statement(int line_no, NodeType type) {
  this->stmt_no_ = line_no;
  this->kind_ = type;
}

Statement::Statement() = default;

Statement::~Statement() = default;

int Statement::get_stmt_no() {
  return stmt_no_;
}

NodeType Statement::get_kind() {
  return kind_;
}

std::string Statement::get_expr_string() {
  return expr_string_;
}

// TODO(nic): would be nice if AST had a vector/set of variables used while generating postfix
std::vector<std::string> Statement::get_vars_from_expr_string() {
  std::string var;
  std::vector<std::string> ans;
  std::stringstream ss(expr_string_);
  while (getline(ss, var, ' ')) {
    // Check if first letter is LETTER according to grammar
    bool valid = isalpha(var[0]);
    for (auto &c : var) {
      if (!isalnum(c)) {
        valid = false;
        break;
      }
    }
    if (valid) ans.push_back(var);
  }
  return ans;
}

std::string Statement::get_called_proc_name() {
  if (this->kind_ == NodeType::Call) {
    return this->called_proc_name;
  }
  return "";
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

std::set<std::string> *Statement::get_uses() {
  return &uses_;
}

std::set<std::string> *Statement::get_modifies() {
  return &modifies_;
}

std::set<int>* Statement::get_next() {
  return &next_;
}

std::set<int>* Statement::get_next_star() {
  return &next_star_;
}

std::set<int>* Statement::get_prev() {
  return &prev_;
}
// Gets the prev star statements.
std::set<int>* Statement::get_prev_star() {
  return &prev_star_;
}

std::set<int>* Statement::get_affects() {
  return &affects_;
}

std::set<int>* Statement::get_affects_star() {
  return &affects_star_;
}

std::set<int>* Statement::get_affected_by() {
  return &affected_by_;
}

std::set<int>* Statement::get_affected_by_star() {
  return &affected_by_star_;
}


void Statement::set_expr_string(std::string expr_string) {
  this->expr_string_ = expr_string;
}

void Statement::set_called_proc_name(std::string name) {
  if (this->kind_ == NodeType::Call) {
    this->called_proc_name = name;
  }
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

void Statement::AddUses(std::string var_name) {
  uses_.insert(var_name);
}

void Statement::AddModifies(std::string var_name) {
  modifies_.insert(var_name);
}

void Statement::AddNext(int line_no) {
  next_.insert(line_no);
}

void Statement::AddNextStar(int line_no) {
  next_star_.insert(line_no);
}

void Statement::AddPrev(int line_no) {
  prev_.insert(line_no);
}

void Statement::AddPrevStar(int line_no) {
  prev_star_.insert(line_no);
}

void Statement::AddAffects(int line_no) {
  affects_.insert(line_no);
}

void Statement::AddAffectsStar(int line_no) {
  affects_star_.insert(line_no);
}

void Statement::AddAffectedBy(int line_no) {
  affected_by_.insert(line_no);
}

void Statement::AddAffectedByStar(int line_no) {
  affected_by_star_.insert(line_no);
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

void Statement::UsesInfo() {
  std::cout << "Statement " << stmt_no_ << " uses:\n";
  std::cout << "Uses: ";
  for (auto &x : uses_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}

void Statement::ModifiesInfo() {
  std::cout << "Statement " << stmt_no_ << " modifies:\n";
  std::cout << "Modifies: ";
  for (auto &x : modifies_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}


void Statement::NextInfo() {
  std::cout << "Statement " << stmt_no_ << " has next:\n";
  std::cout << "Next: ";
  for (auto& x : next_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "NextStar: ";
  for (auto& x : next_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Statement " << stmt_no_ << " has prev:\n";
  std::cout << "Prev: ";
  for (auto& x : prev_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "PrevStar: ";
  for (auto& x : prev_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}

void Statement::AffectsInfo() {
  std::cout << "Statement " << stmt_no_ << " has affects:\n";
  std::cout << "Affects: ";
  for (auto& x : affects_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "AffectsStar: ";
  for (auto& x : affects_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Statement " << stmt_no_ << " is affected by:\n";
  std::cout << "AffectedBy: ";
  for (auto& x : affected_by_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "AffectedByStar: ";
  for (auto& x : affected_by_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}
