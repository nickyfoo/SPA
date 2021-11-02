#include "procedure.h"
#include <iostream>
#include <utility>

Procedure::Procedure(std::string name, int stmt_no) {
  this->name_ = std::move(name);
  this->first_statement_ = stmt_no;
}

Procedure::Procedure(std::string name) {
  this->name_ = std::move(name);
}

Procedure::~Procedure() = default;

std::string Procedure::get_name() {
  return name_;
}

int Procedure::get_stmt_no() {
  return first_statement_;
}

std::set<std::string> *Procedure::get_uses() {
  return &uses_;
}

std::set<std::string> *Procedure::get_modifies() {
  return &modifies_;
}

std::set<std::string> *Procedure::get_calls() {
  return &calls_;
}

std::set<std::string> *Procedure::get_calls_star() {
  return &calls_star_;
}

std::set<std::string> *Procedure::get_callers() {
  return &callers_;
}

std::set<std::string> *Procedure::get_callers_star() {
  return &callers_star_;
}

void Procedure::AddUses(const std::string &var_name) {
  uses_.insert(var_name);
}

void Procedure::AddModifies(const std::string &var_name) {
  modifies_.insert(var_name);
}

void Procedure::AddCalls(const std::string &proc_name) {
  calls_.insert(proc_name);
}

void Procedure::AddCallsStar(const std::string &proc_name) {
  calls_star_.insert(proc_name);
}

void Procedure::AddCallers(const std::string &proc_name) {
  callers_.insert(proc_name);
}
void Procedure::AddCallersStar(const std::string &proc_name) {
  callers_star_.insert(proc_name);
}

void Procedure::CallsInfo() {
  std::cout << "Procedure " << name_ << " calls:\n";
  std::cout << "Calls: ";
  for (auto &x : calls_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "CallsStar: ";
  for (auto &x : calls_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "Procedure " << name_ << " has callers:\n";
  std::cout << "Callers: ";
  for (auto &x : callers_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
  std::cout << "CallersStar: ";
  for (auto &x : callers_star_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}

void Procedure::UsesInfo() {
  std::cout << "Procedure " << name_ << " uses:\n";
  std::cout << "Uses: ";
  for (auto &x : uses_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}

void Procedure::ModifiesInfo() {
  std::cout << "Procedure " << name_ << " modifies:\n";
  std::cout << "Modifies: ";
  for (auto &x : modifies_) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}
