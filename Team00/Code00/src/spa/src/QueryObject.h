#pragma once

#include <stdexcept>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

class QueryObject {
public:
    QueryObject(string input);
    tuple<string, string, string> getClauses();
private:
    unordered_map<string, Entity> synToEntity;
    string selectClause;
    tuple<string, string, string> clauses;
    tuple<vector<string>, string> splitEntitiesAndSelectClause(string input);
    void sendToEntitiesParser();
    void sendToSelectClauseParser();
};