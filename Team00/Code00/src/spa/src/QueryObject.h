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
    explicit QueryObject(std::string input);
    PQLQuery *getPQLQuery();
private:
    std::unordered_map<std::string, EntityDeclaration> synToEntity;
    PQLQuery *clauses;
    static std::tuple<std::vector<std::string>*, std::string> splitEntitiesAndSelectClause(std::string input);
    static std::unordered_map<std::string, EntityDeclaration*> *getEntitiesMap(std::vector<std::string> *entities);
    static PQLQuery *getSelectClause(std::unordered_map<std::string, EntityDeclaration*> *entities_map, std::string& select_clause);
};