#pragma once

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "select_clause_parser.h"
#include "entity_declaration_parser.h"

class QueryPreprocessor {
 public:
  explicit QueryPreprocessor(std::string input);
  ~QueryPreprocessor();
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> *get_clauses();

 private:
  std::unordered_map<std::string, EntityDeclaration> syn_to_entity_;
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> *clauses_;
  static std::tuple<std::vector<std::string> *, std::string>
  SplitEntitiesAndSelectClause(std::string input);
  static std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> MakeEntitiesMap(std::vector<std::string> *entities);
  static std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> *MakeClauses(
      bool semantically_valid,
      std::unordered_map<std::string, EntityDeclaration *> *entities_map,
      const std::string &select_clause);
};
