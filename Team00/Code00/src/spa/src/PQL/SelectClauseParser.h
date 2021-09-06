#pragma once

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "EntityDeclaration.h"
#include "SuchThatClause.h"
#include "PQLQuery.h"

class SelectClauseParser {
public:
    static SelectClauseParser *getInstance();
    void setSelectClause(std::unordered_map<std::string, EntityDeclaration*> *syn_to_ent, std::string select_clause);
    PQLQuery *getClauses();
private:
    static SelectClauseParser *instance;
    std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entity;
    std::string select_statement;
    SelectClauseParser();
    SuchThatRef* makeSuchThatRef(SuchThatClause *relationship, std::string ref);
    PatternClause* makePatternRef(std::string& synonym, std::string left_ref, std::string right_ref);
    SuchThatClause* getRelationshipClause(std::string relationship_statement);
    PatternClause* getPatternClause(std::string pattern_statement);
    static std::vector<std::string> splitSelect(std::string& select_clause);
    static std::vector<std::string> splitTokensByDelimiter(std::string input, const std::string& delimiter);
    static std::vector<std::string> splitTokensByMultipleDelimiters(const std::string& input, const std::string& delimiters);
    static std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> splitTokensByClauses(const std::string& input);
    static bool isValidIdentifier(std::string str);
    static bool isInteger(std::string& str);

};