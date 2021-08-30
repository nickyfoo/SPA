#include "SelectClauseParser.h"
#include "Relationship.h"
#include <iostream>
#include <tuple>

using namespace std;

SelectClauseParser::SelectClauseParser() {}
SelectClauseParser* SelectClauseParser::instance = 0;

SelectClauseParser *SelectClauseParser::getInstance() {
    if (!instance)
        instance = new SelectClauseParser;
    return instance;
}

void SelectClauseParser::setSelectClause(unordered_map<string, Entity*> *synonym_to_entity, string select_clause)
{
    this->synonym_to_entity = synonym_to_entity;
    select_clause.erase(remove(select_clause.begin(), select_clause.end(),
                                        '\n'), select_clause.end());
    this->select_clause = select_clause;
}

PQLQuery *SelectClauseParser::getClauses()
{
    vector<string> tokens = splitTokensByDelimiter(select_clause, " such that ");

    bool valid_syntax;
    vector<Entity*> *select = new vector<Entity*>();
    vector<Relationship*> *such_that = new vector<Relationship*>();

    string select_statement = tokens.at(0);
    vector<string> select_clauses = splitTokensByDelimiter(select_statement, " ");
    valid_syntax = select_clauses.at(0) == "Select" && isValidIdentifier(select_clauses.at(1))
            && (synonym_to_entity->find(select_clauses.at(1)) != synonym_to_entity->end());
    if (valid_syntax) {
        select->push_back(synonym_to_entity->at(select_clauses.at(1) ));
    };
    if (valid_syntax && tokens.size() > 1) {
        string relationship_statement = tokens.at(1);
        relationship_statement.erase(remove(relationship_statement.begin(), relationship_statement.end(),
                                               ' '), relationship_statement.end());
        vector<string> relationship_clauses = splitTokensByMultipleDelimiters(relationship_statement, "(,)");
        Relationship* relationship = new Relationship(relationship_clauses.at(0));

        string left_ref = relationship_clauses.at(1);
        string right_ref = relationship_clauses.at(2);
        if (synonym_to_entity->find(left_ref) != synonym_to_entity->end()
                && synonym_to_entity->find(right_ref) != synonym_to_entity->end()
                && relationship->getType() != RelationshipType::None) {
            Entity* left = synonym_to_entity->at(left_ref);
            Entity* right = synonym_to_entity->at(right_ref);
            if (relationship->setRef(left, right)) {
                such_that->push_back(relationship);
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    PQLQuery* ret = new PQLQuery(select, such_that);
    return ret;

}

// Function that returns true if str
// is a valid identifier
bool SelectClauseParser::isValidIdentifier(string str)
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

vector<string> SelectClauseParser::splitTokensByDelimiter(string input, string delimiter)
{
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = input.find(delimiter)) != string::npos) {
        token = input.substr(0, pos);
        if (token != "") {
            tokens.push_back(token);
        }
        input.erase(0, pos + delimiter.length());
    }
    tokens.push_back(input);
    return tokens;
}

vector<string> SelectClauseParser::splitTokensByMultipleDelimiters(string input, string delimiters)
{
    stringstream input_stream;
    input_stream << input;
    vector<string> tokens;

    string line;
    while(getline(input_stream, line))
    {
        size_t prev = 0, pos;
        while ((pos = line.find_first_of(delimiters, prev)) != string::npos)
        {
            if (pos > prev)
                tokens.push_back(line.substr(prev, pos-prev));
            prev = pos+1;
        }
        if (prev < line.length())
            tokens.push_back(line.substr(prev, std::string::npos));
    }

    return tokens;
}

