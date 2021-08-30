#pragma once

#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;
typedef short PROC;

class SelectClauseParser {
public:
    static SelectClauseParser *getInstance();
    void setSelectClause(unordered_map<string, string> *synonym_to_entity, string select_clause);
    tuple<string, string, string> *getClauses();
private:
    static SelectClauseParser *instance;
    unordered_map<string, string>* synonym_to_entity;
    string select_clause;
    SelectClauseParser();
    bool checkValidSyntax();
    vector<string> getTokens();
    bool isValidIdentifier(string str);
};