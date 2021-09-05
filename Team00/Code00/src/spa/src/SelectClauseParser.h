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

using namespace std;
typedef short PROC;

class SelectClauseParser {
public:
    static SelectClauseParser *getInstance();
    void setSelectClause(unordered_map<string, Entity*> *synonym_to_entity, string select_clause);
    PQLQuery *getClauses();
private:
    static SelectClauseParser *instance;
    unordered_map<string, Entity*> *synonym_to_entity;
    string select_clause;
    SelectClauseParser();
    bool checkValidSyntax();
    Relationship* getRelationshipStatementClause(string relationship_statement);
    vector<string> splitSelect(string& select_clause);
    vector<string> splitTokensByDelimiter(string input, string delimiter);
    vector<string> splitTokensByMultipleDelimiters(string input, string delimiters);
    tuple<string, vector<string>, vector<string>> splitTokensByClauses(string input);
    bool isValidIdentifier(string str);
    bool isInteger(string& str);

};