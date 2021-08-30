#include "QueryObject.h"
#include "SelectClauseParser.h"

QueryObject::QueryObject(string input)
{
    vector<string> entities;
    string select_clause;
    tie(entities, select_clause) = splitEntitiesAndSelectClause(input);
    unordered_map<string, Entity> entities_map = getEntitiesMap(entities);
    tuple<string, string, string> tuples = getSelectClause(entities_map, select_clause);
    if (entities_map == nullptr || tuples == nullptr) {
        throw invalid_argument("Invalid query");
    }

}

tuple<string, string, string> QueryObject::getClauses()
{
    return this->clauses;
}

tuple<vector<string>>, string> QueryObject::splitEntitiesAndSelectClause(string input) {
    string delimiter = ";";
    vector<string> entities;

    size_t pos = 0;
    string token;
    while ((pos = input.find(delimiter)) != string::npos) {
        token = input.substr(0, pos);
        entities.push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    string select_clause = token;
    return tuple(entities, select_clause);
}

unordered_map<string, Entity> QueryObject::getEntitiesMap(vector<string> entities)
{
    EntityParser *ep = ep->getInstance();
    ep.setEntities(entities);
    return ep.getEntitiesMap();
}

tuple<string, string, string> *QueryObject::getSelectClause(unordered_map<string, Entity> entities_map, string select_clause)
{
    SelectClauseParser *scp = scp->getInstance();
    scp->setSelectClause(entities_map, select_clause);
    return scp->getClauses();
}