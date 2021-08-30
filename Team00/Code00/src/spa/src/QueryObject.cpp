#include "QueryObject.h"

QueryObject::QueryObject(string input)
{
    vector<string> *entities;
    string select_clause;
    tie(entities, select_clause) = splitEntitiesAndSelectClause(input);
    unordered_map<string, Entity*> *entities_map = getEntitiesMap(entities);
    PQLQuery *pqlquery = getSelectClause(entities_map, select_clause);
    this->clauses = pqlquery;

}

PQLQuery *QueryObject::getPQLQuery()
{
    return this->clauses;
}

tuple<vector<string>*, string> QueryObject::splitEntitiesAndSelectClause(string input) {
    string delimiter = ";";
    vector<string> *entities = new vector<string>();
    size_t pos = 0;
    string token;
    while ((pos = input.find(delimiter)) != string::npos) {
        token = input.substr(0, pos);
        entities->push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    string select_clause = input;
    return tuple(entities, select_clause);
}

unordered_map<string, Entity*> *QueryObject::getEntitiesMap(vector<string> *entities)
{
    EntityDeclarationParser *ep = ep->getInstance();
    ep->setEntities(entities);
    return ep->getEntitiesMap();
}

PQLQuery *QueryObject::getSelectClause(unordered_map<string, Entity*> *entities_map, string select_clause)
{
    SelectClauseParser *scp = scp->getInstance();
    scp->setSelectClause(entities_map, select_clause);
    return scp->getClauses();
}