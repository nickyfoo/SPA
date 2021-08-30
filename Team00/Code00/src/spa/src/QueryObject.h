#pragma once

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "SelectClauseParser.h"
#include "EntityDeclarationParser.h"

using namespace std;

class QueryObject {
public:
    QueryObject(string input);
    PQLQuery *getPQLQuery();
private:
    unordered_map<string, Entity> synToEntity;
    string selectClause;
    PQLQuery *clauses;
    tuple<vector<string>*, string> splitEntitiesAndSelectClause(string input);
    unordered_map<string, Entity*> *getEntitiesMap(vector<string> *entities);
    PQLQuery *getSelectClause(unordered_map<string, Entity*> *entities_map, string select_clause);
};