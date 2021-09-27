#pragma once

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "entity_declaration.h"
#include "such_that_clause.h"
#include "pql_query.h"

class SelectClauseParser {
 public:
  static SelectClauseParser *get_instance();
  void set_select_clause(std::unordered_map<std::string,
                                            EntityDeclaration *> *syn_to_ent,
                         std::string select_clause);
  PQLQuery *get_clauses();

 private:
  static SelectClauseParser *instance;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entity_;
  std::string select_statement_;
  SelectClauseParser();
  SuchThatRef* MakeSuchThatRefLeft(SuchThatClause *relationship, std::string ref);
  SuchThatRef* MakeSuchThatRefRight(SuchThatClause *relationship, std::string ref);
  PatternClause *MakePatternRef(const std::string &synonym,
                                std::string left_ref,
                                std::string right_ref);
  std::vector<SuchThatClause *> *MakeSuchThatClause(std::string relationship_statement);
  std::vector<PatternClause *> *MakePatternClause(std::string pattern_statement);
  static std::vector<std::string> SplitSelect(std::string select_clause);
  static std::vector<std::string> SplitTokensByDelimiter(
      std::string input, const std::string &delimiter);
  static std::vector<std::vector<std::string>> SplitTokensByBrackets(
      const std::string &input);
  static std::tuple<std::string, std::vector<std::string>,
                    std::vector<std::string>, std::vector<std::string>> SplitTokensByClauses(
      const std::string &input);
  static bool IsValidIdentifier(const std::string &str);
  static bool IsInteger(const std::string &str);
};
