#pragma once

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "Entity.h"
#include "Relationship.h"
#include "PQLQuery.h"

class SelectClauseParser {
public:
    static SelectClauseParser *getInstance();
    void setSelectClause(std::unordered_map<std::string, Entity*> *synonym_to_entity, std::string select_clause);
    PQLQuery *getClauses();
private:
    static SelectClauseParser *instance;
    std::unordered_map<std::string, Entity*> *synonym_to_entity;
    std::string select_clause;
    SelectClauseParser();
    Relationship* getRelationshipClause(std::string relationship_statement);
    Pattern* getPatternClause(std::string pattern_statement);
    static std::vector<std::string> splitSelect(std::string& select_clause);
    static std::vector<std::string> splitTokensByDelimiter(std::string input, const std::string& delimiter);
    static std::vector<std::string> splitTokensByMultipleDelimiters(const std::string& input, const std::string& delimiters);
    std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> splitTokensByClauses(std::string input);
    static bool isValidIdentifier(std::string str);
    static bool isInteger(std::string& str);

};