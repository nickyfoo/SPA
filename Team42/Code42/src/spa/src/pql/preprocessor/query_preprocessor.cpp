#include "memory"
#include "query_preprocessor.h"
#include "query_optimizer.h"

QueryPreprocessor::QueryPreprocessor(std::string input) {
  std::vector<std::string> *entities;
  std::string select_clause;
  tie(entities, select_clause) = SplitEntitiesAndSelectClause(input);
  std::unordered_map<std::string, EntityDeclaration *> *entities_map;
  entities_map = MakeEntitiesMap(entities);
  if (entities_map == nullptr) {
    this->clauses_ = nullptr;
    return;
  }
  printf("here1\n");
  PQLQuery *pql_query = MakePQLQuery(entities_map, select_clause);
  this->clauses_ = pql_query;
}

QueryPreprocessor::~QueryPreprocessor() = default;

PQLQuery *QueryPreprocessor::get_pql_query() {
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

std::unordered_map<std::string, EntityDeclaration *>
*QueryPreprocessor::MakeEntitiesMap(
    std::vector<std::string> *entities) {
  EntityDeclarationParser *ep = EntityDeclarationParser::get_instance();
  ep->set_entities(entities);
  return ep->get_entities_map();
}

PQLQuery *QueryPreprocessor::MakePQLQuery(std::unordered_map<std::string,
                                                             EntityDeclaration *> *entities_map,
                                          const std::string &select_clause) {
  SelectClauseParser *scp = SelectClauseParser::get_instance();
  scp->set_select_clause(entities_map, select_clause);
  std::tuple<std::vector<ResultClause *> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool> *clauses_tuple = scp->get_clauses();
  printf("muthafucker\n");
  if (clauses_tuple == nullptr) return nullptr;
  printf("DID IT STILL ALIVE HERE\n");
  bool is_valid_clause = std::get<5>(*clauses_tuple);
  if (!is_valid_clause) {
    printf("HEREBOI\n");
    return new PQLQuery(std::get<0>(*clauses_tuple),
                                       std::vector<std::shared_ptr<ClauseGroup>>(),
                                       std::get<4>(*clauses_tuple),
                                       false);
  }
  QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clauses_tuple),
                                                  std::get<2>(*clauses_tuple),
                                                  std::get<3>(*clauses_tuple),
                                                  std::get<0>(*clauses_tuple));
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateGroupings();
  PQLQuery *pql_query = new PQLQuery(std::get<0>(*clauses_tuple),
                                     clause_groups,
                                     std::get<4>(*clauses_tuple),
                                     true);
  return pql_query;
}
