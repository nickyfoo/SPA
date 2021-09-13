#include "variable.h"
#include <iostream>

Variable::Variable(int index, std::string name) {
  this->index_ = index;
  this->name_ = name;
}

Variable::~Variable() = default;

std::string Variable::get_name() {
  return name_;
}

int Variable::get_index() {
  return index_;
}

void Variable::AddStmtUsing(int stmt_no) { 
  stmts_using_.insert(stmt_no); 
}

void Variable::AddStmtModifying(int stmt_no) {
  stmts_modifying_.insert(stmt_no);
}

void Variable::AddProcUsing(std::string proc_name) {
  procs_using_.insert(proc_name);
}

void Variable::AddProcModifying(std::string proc_name) {
  procs_modifying_.insert(proc_name);
}

std::set<int>* Variable::get_stmts_using() { 
  return &stmts_using_; 
}

std::set<int>* Variable::get_stmts_modifying() { 
  return &stmts_modifying_; 
}

std::set<std::string>* Variable::get_procs_using() { 
  return &procs_using_; 
}

std::set<std::string>* Variable::get_procs_modifying() {
  return &procs_modifying_;
}

void Variable::UsesInfo() {
  std::cout << "Variable " << name_ << " used by:\n";
  std::cout << "Statements Using: ";
  for (auto &x : stmts_using_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Procedures Using: ";
  for (auto& x : procs_using_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}

void Variable::ModifiesInfo() {
  std::cout << "Variable " << name_ << " modified by:\n";
  std::cout << "Statements Modifying: ";
  for (auto& x : stmts_modifying_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Procedures Modifying: ";
  for (auto& x : procs_modifying_) {
    std::cout << x << ' ';
  }
  std::cout << '\n' << '\n';
}