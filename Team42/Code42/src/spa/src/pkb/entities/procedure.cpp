#include "procedure.h"
#include <iostream>

Procedure::Procedure(std::string name) {
  this->name_ = name;
}

Procedure::~Procedure() = default;

std::string Procedure::get_name() {
  return name_;
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

void Procedure::AddUses(std::string var_name) {
  uses_.insert(var_name);
}

void Procedure::AddModifies(std::string var_name) {
  modifies_.insert(var_name);
}

void Procedure::AddCalls(std::string proc_name) {
  calls_.insert(proc_name);
}

void Procedure::AddCallsStar(std::string proc_name) {
  calls_star_.insert(proc_name);
}

void Procedure::AddCallers(std::string proc_name) {
  callers_.insert(proc_name);
}
void Procedure::AddCallersStar(std::string proc_name) {
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
