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
    bool checkValidSyntax();
    Relationship* getRelationshipClause(std::string relationship_statement);
    Pattern* getPatternClause(std::string pattern_statement);
    std::vector<std::string> splitSelect(std::string& select_clause);
    std::vector<std::string> splitTokensByDelimiter(std::string input, std::string delimiter);
    std::vector<std::string> splitTokensByMultipleDelimiters(std::string input, std::string delimiters);
    std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> splitTokensByClauses(std::string input);
    bool isValidIdentifier(std::string str);
    bool isInteger(std::string& str);

};