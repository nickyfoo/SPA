#include "memory"
#include "query_preprocessor.h"
#include "query_optimizer.h"

QueryPreprocessor::QueryPreprocessor(std::string input) {
  std::vector<std::string> *entities;
  std::string select_clause;
  tie(entities, select_clause) = SplitEntitiesAndSelectClause(input);
  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> entities_map;
  entities_map = MakeEntitiesMap(entities);

  printf("syn: %d\n", std::get<1>(entities_map));
  printf("semo: %d\n", std::get<2>(entities_map));
  // If entity declaration is syntactically invalid, return clauses straight
  if (!std::get<1>(entities_map)) {
    auto *select_ret = new std::vector<ResultClause *>();
    auto *such_that_ret = new std::vector<SuchThatClause *>();
    auto *pattern_ret = new std::vector<PatternClause *>();
    auto *with_ret = new std::vector<WithClause *>();
    auto *false_res = new std::tuple<std::vector<ResultClause *> *,
    std::vector<SuchThatClause *> *,
    std::vector<PatternClause *> *,
    std::vector<WithClause *> *,
    std::unordered_map<std::string, EntityDeclaration *> *,
    bool, bool>(
        select_ret, such_that_ret,
        pattern_ret, with_ret, std::get<0>(entities_map), false, false);
    this->clauses_ = false_res;
    return;
  }
  std::tuple<std::vector<ResultClause *> *,
  std::vector<SuchThatClause *> *,
  std::vector<PatternClause *> *,
  std::vector<WithClause *> *,
  std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> *clauses = MakeClauses(std::get<2>(entities_map),
      std::get<0>(entities_map),
          select_clause);
  this->clauses_ = clauses;
}

QueryPreprocessor::~QueryPreprocessor() = default;

std::tuple<std::vector<ResultClause *> *,
std::vector<SuchThatClause *> *,
std::vector<PatternClause *> *,
std::vector<WithClause *> *,
std::unordered_map<std::string, EntityDeclaration *> *,
bool, bool> *QueryPreprocessor::get_clauses() {
  return this->clauses_;
}

std::tuple<std::vector<std::string> *, std::string>
QueryPreprocessor::SplitEntitiesAndSelectClause(std::string input) {
  std::string delimiter = ";";
  auto *entities = new std::vector<std::string>();
  size_t pos;
  std::string token;
  while ((pos = input.find(delimiter)) != std::string::npos) {
    token = input.substr(0, pos);
    entities->push_back(token);
    input.erase(0, pos + delimiter.length());
  }
  std::string select_clause = input;
  return std::tuple(entities, select_clause);
}

std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
bool, bool> QueryPreprocessor::MakeEntitiesMap(
    std::vector<std::string> *entities) {
  EntityDeclarationParser *ep = EntityDeclarationParser::get_instance();
  ep->set_entities(entities);
  return ep->get_entities_map();
}

std::tuple<std::vector<ResultClause *> *,
std::vector<SuchThatClause *> *,
std::vector<PatternClause *> *,
std::vector<WithClause *> *,
std::unordered_map<std::string, EntityDeclaration *> *,
bool, bool> *QueryPreprocessor::MakeClauses(bool semantically_valid,
                                            std::unordered_map<std::string,
                                                             EntityDeclaration *> *entities_map,
                                          const std::string &select_clause) {
  SelectClauseParser *scp = SelectClauseParser::get_instance();
  scp->set_select_clause(semantically_valid, entities_map, select_clause);
  std::tuple<std::vector<ResultClause *> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> *clauses_tuple = scp->get_clauses();
  return clauses_tuple;
}
