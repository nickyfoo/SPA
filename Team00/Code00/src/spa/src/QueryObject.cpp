#include "QueryObject.h"

QueryObject::QueryObject(std::string input)
{
    std::vector<std::string> *entities;
    std::string select_clause;
    tie(entities, select_clause) = splitEntitiesAndSelectClause(input);
    std::unordered_map<std::string, Entity*> *entities_map = getEntitiesMap(entities);
    if (entities_map == nullptr) {
        this->clauses = nullptr;
        return;
    }
    PQLQuery *pqlquery = getSelectClause(entities_map, select_clause);
    this->clauses = pqlquery;

}

PQLQuery *QueryObject::getPQLQuery()
{
    return this->clauses;
}

std::tuple<std::vector<std::string>*, std::string> QueryObject::splitEntitiesAndSelectClause(std::string input) {
    std::string delimiter = ";";
    std::vector<std::string> *entities = new std::vector<std::string>();
    size_t pos = 0;
    std::string token;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        entities->push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    std::string select_clause = input;
    return std::tuple(entities, select_clause);
}

std::unordered_map<std::string, Entity*> *QueryObject::getEntitiesMap(std::vector<std::string> *entities)
{
    EntityDeclarationParser *ep = ep->getInstance();
    ep->setEntities(entities);
    return ep->getEntitiesMap();
}

PQLQuery *QueryObject::getSelectClause(std::unordered_map<std::string, Entity*> *entities_map, std::string select_clause)
{
    SelectClauseParser *scp = scp->getInstance();
    scp->setSelectClause(entities_map, select_clause);
    return scp->getClauses();
}