#include "SelectClauseParser.h"
#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

SelectClauseParser::SelectClauseParser() {}

SelectClauseParser *SelectClauseParser::getInstance() {
    if (!instance)
        instance = new SelectClauseParser;
    return instance;
}

void SelectClauseParser::setSelectClause(unordered_map<string, string> *synonym_to_entity, string select_cause)
{
    this->synonym_to_entity = synonym_to_entity;
    this->select_clause = select_clause;
}

tuple<string, string, string> SelectClauseParser::getClauses()
{
    if (!checkValidSyntax()) {
        return NULL;
    }
}

bool SelectClauseParser::checkValidSyntax()
{
    vector<string> tokens = getTokens();
    bool valid_syntax = true;

    valid_syntax = tokens.at(0) == "Select" && isValidIdentifier(tokens.at(1))
            && (synonym_to_entity->find(tokens.at(1)) != synonym_to_entity->end());
    unordered_map<string, string> queried_entities;
    queried_entities.insert({[tokens.at(1)], synonym_to_entity->find(tokens.at(1)});


//    for (auto it = tokens.begin()+1; it != tokens.end(); ++it)
//    {
//        if (it == 'such')
//        {
//            auto next_it = ++it;
//            if (next_it != tokens.end() && next_it == 'that')
//            {
//                such_that_found = true;
//            }
//        }
//    }

    bool such_that_found = false;
    bool pattern_found = false;
    for (int i=2; i < tokens.size(); ++i)
    {
        if (such_that_found) {

        }
        if (tokens[i] == "such" && i != tokens.size() - 1 && tokens[i+1] == "that") {
            such_that_found = true;
            ++i;
        } else if (tokens[i] == "pattern") {
            pattern_found = true;
        }
    }

    return valid_syntax && (synonym_to_entity->find(tokens.at(1)) != synonym_to_entity->end());

}

// Function that returns true if str
// is a valid identifier
bool isValidIdentifier(string str)
{
    // If first character is invalid
    if (!((str[0] >= 'a' && str[0] <= 'z')
    || (str[0] >= 'A' && str[0] <= 'Z')
    || str[0] == '_'))
        return false;

    // Traverse the string for the rest of the characters
    for (int i = 1; i < str.length(); i++) {
        if (!((str[i] >= 'a' && str[i] <= 'z')
        || (str[i] >= 'A' && str[i] <= 'Z')
        || (str[i] >= '0' && str[i] <= '9')
        || str[i] == '_'))
            return false;
    }

    // String is a valid identifier
    return true;
}

vector<string> SelectClauseParser::getTokens()
{
    string delimiter = " ";
    vector<string> tokens;

    size_t pos = 0;
    string token;
    while ((pos = select_clause.find(delimiter)) != string::npos) {
        token = select_clause.substr(0, pos);
        tokens.push_back(token);
        select_clause.erase(0, pos + delimiter.length());
    }
    tokens.push_back(token)
}

