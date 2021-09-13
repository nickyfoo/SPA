#include "query_preprocessor.h"

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
    PQLQuery *pql_query = MakePQLQuery(entities_map, select_clause);
    this->clauses_ = pql_query;
}

PQLQuery* QueryPreprocessor::get_pql_query() {
    return this->clauses_;
}

std::tuple<std::vector<std::string>*, std::string>
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

std::unordered_map<std::string, EntityDeclaration*>
        *QueryPreprocessor::MakeEntitiesMap(
                std::vector<std::string> *entities) {
    EntityDeclarationParser *ep = EntityDeclarationParser::get_instance();
    ep->set_entities(entities);
    return ep->get_entities_map();
}

PQLQuery *QueryPreprocessor::MakePQLQuery(std::unordered_map<std::string,
                                             EntityDeclaration*> *entities_map,
                                          const std::string &select_clause) {
    SelectClauseParser *scp = SelectClauseParser::get_instance();
    scp->set_select_clause(entities_map, select_clause);
    return scp->get_clauses();
}
