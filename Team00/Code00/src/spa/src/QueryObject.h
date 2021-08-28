#pragma once

#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

class QueryObject {
public:
    QueryObject(string input);
private:
    unordered_map<string, Entity> synToEntity;
    tuple<vector<string>, string> splitEntitiesAndSelectClause(string input);
    string selectClause;
    void sendToEntitiesParser();
    void sendToSelectClauseParser();
};