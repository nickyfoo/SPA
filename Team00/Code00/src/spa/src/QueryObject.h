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

class QueryObject {
public:
    QueryObject(std::string input);
    PQLQuery *getPQLQuery();
private:
    std::unordered_map<std::string, Entity> synToEntity;
    std::string selectClause;
    PQLQuery *clauses;
    std::tuple<std::vector<std::string>*, std::string> splitEntitiesAndSelectClause(std::string input);
    std::unordered_map<std::string, Entity*> *getEntitiesMap(std::vector<std::string> *entities);
    PQLQuery *getSelectClause(std::unordered_map<std::string, Entity*> *entities_map, std::string select_clause);
};