#pragma once

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <utility>
#include "entity_declaration.h"
#include "such_that_clause.h"
#include "with_clause.h"
#include "pql_query.h"
#include "result_clause.h"

class SelectClauseParser {
 public:
  static SelectClauseParser *get_instance();
  void set_select_clause(bool semantically_valid,
                         std::unordered_map<std::string,
                                            EntityDeclaration *> *syn_to_ent,
                         std::string select_clause);
  std::tuple<std::vector<ResultClause *> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool,
             bool> *get_clauses();

 private:
  static SelectClauseParser *instance;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entity_;
  std::string select_statement_;
  bool semantically_valid_;
  SelectClauseParser();
  ResultClause *ValidateResultClauseWithAttr(const std::string &select);

  // SuchThatClause functions
  std::vector<SuchThatClause *> *MakeSuchThatClause(const std::string &relationship_statement);
  int SetSuchThatRefLeft(SuchThatRef *left_such_that_ref,
                         SuchThatClause *relationship, std::string left_ref);
  int SetSuchThatRefRight(SuchThatRef *right_such_that_ref,
                          SuchThatClause *relationship, std::string right_ref);

  // PatternClause functions
  std::vector<PatternClause *> *MakePatternClause(const std::string &pattern_statement);
  int SetPatternRef(PatternClause *pattern,
                    const std::string &left_ref,
                    const std::string &right_ref);

  // WithClause functions
  std::vector<WithClause *> *MakeWithClause(const std::string &with_statement);
  int SetWithRef(WithClause *with, const std::string &left_ref, const std::string &right_ref);
  std::tuple<std::string, EntityType, AttrValueType>
  GetWithRefTypeAndAttrValueType(std::string ref);

  // Split functions
  static std::vector<std::string> SplitSelect(std::string select_clause);
  static std::tuple<std::string, std::vector<std::string>,
                    std::vector<std::string>, std::vector<std::string>> SplitClauses(
      const std::string &input);
  static std::vector<std::string> SplitTokensByDelimiter(
      std::string input, const std::string &delimiter);
  static std::vector<std::vector<std::string>> SplitBrackets(
      const std::string &input);
  static std::vector<std::pair<std::string, std::string>> SplitWith(
      const std::string &input);
  static std::vector<std::string> SplitTokensByEqualDelim(std::string input);

  // Helper functions
  static bool IsValidIdentifier(const std::string &str);
  static bool IsInteger(const std::string &str);
  static std::string TrimTrailingWhitespaces(const std::string &str);

};
