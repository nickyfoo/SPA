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
    std::cout << "here1" << std::flush;
    tuple<string, vector<string>, vector<string>> clauses = splitTokensByClauses(select_clause);
    std::cout << "here2" << std::flush;
    string select_clause = std::get<0>(clauses);
    vector<string> such_that_clauses = std::get<1>(clauses);
    vector<string> pattern_clauses = std::get<2>(clauses);

    std::cout << "select is " << select_clause << "\n" << std::flush;
    for (string such_that : such_that_clauses) {
        std::cout << "such that is " << such_that << "\n" << std::flush;
    }
    for (string pattern : pattern_clauses) {
        std::cout << "pattern is " << pattern << "\n" << std::flush;
    }

    vector<Entity*> *select_ret = new vector<Entity*>();
    vector<Relationship*> *such_that_ret = new vector<Relationship*>();
    vector<Pattern*> *pattern_ret = new vector<Pattern*>();

    vector<string> select_clauses = splitSelect(select_clause);
    if (select_clauses.empty()) { // invalid select syntax
        return nullptr;
    }
    for (string select_clause : select_clauses) {
        if (synonym_to_entity->find(select_clause) == synonym_to_entity->end()) { // not found in hashmap
            return nullptr;
        } else {
            select_ret->push_back(synonym_to_entity->at(select_clause));
        }
    }
    for (string such_that_clause : such_that_clauses) {
        Relationship* relationship = getRelationshipStatementClause(such_that_clause);
        if (relationship == nullptr) {
            return nullptr;
        }
        such_that_ret->push_back(relationship);
    }


    PQLQuery* ret = new PQLQuery(select_ret, such_that_ret, pattern_ret, synonym_to_entity);
    return ret;

}

Relationship* SelectClauseParser::getRelationshipStatementClause(string relationship_statement) {
    relationship_statement.erase(remove(relationship_statement.begin(), relationship_statement.end(),
                                        ' '), relationship_statement.end());
    vector<string> relationship_clauses = splitTokensByMultipleDelimiters(relationship_statement, "(,)");
    Relationship* relationship = new Relationship(relationship_clauses.at(0));

    string left_ref = relationship_clauses.at(1);
    string right_ref = relationship_clauses.at(2);
    if ((synonym_to_entity->find(left_ref) != synonym_to_entity->end() || isValidIdentifier(left_ref) || isInteger(left_ref))
            && (synonym_to_entity->find(right_ref) != synonym_to_entity->end() || isValidIdentifier(right_ref) || isInteger(right_ref))
            && relationship->getType() != RelationshipType::None) {
        Entity* left;
        Entity* right;

        if (synonym_to_entity->find(left_ref) != synonym_to_entity->end()) {
            left = synonym_to_entity->at(left_ref);
        } else {
            left = new Entity(left_ref);
        }

        if (synonym_to_entity->find(right_ref) != synonym_to_entity->end()) {
            right = synonym_to_entity->at(right_ref);
        } else {
            right = new Entity(right_ref);
        }

        if (relationship->setRef(left, right)) {
            return relationship;
        }
    }
    return nullptr;
}

// Function that returns true if str
// is a valid identifier
bool SelectClauseParser::isValidIdentifier(string str)
{
    if (str == "_")
        return true;

    if (str[0] != '\'' && str[str.length() - 1] != '\'')
        return false;

    // If first character is invalid
    if (!((str[1] >= 'a' && str[1] <= 'z')
    || (str[1] >= 'A' && str[1] <= 'Z')
    || str[1] == '_'))
        return false;

    // Traverse the string for the rest of the characters
    for (int i = 2; i < str.length() - 1; i++) {
        if (!((str[i] >= 'a' && str[i] <= 'z')
        || (str[i] >= 'A' && str[i] <= 'Z')
        || (str[i] >= '0' && str[i] <= '9')
        || str[i] == '_'))
            return false;
    }

    // String is a valid identifier
    return true;
}

bool SelectClauseParser::isInteger(string& s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

vector<string> SelectClauseParser::splitSelect(string& s) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t pos = s.find_first_not_of(WHITESPACE);
    if (pos == std::string::npos) {
        return vector<string>();
    } else {
        s.erase(0, pos);
    }

    const std::string SELECT_DELIM = "Select";
    bool found_select = false;
    pos = 0;
    string token;

    while ((pos = s.find(SELECT_DELIM)) != string::npos) {
        if (found_select) { // two select clauses found
            return vector<string>();
        }
        token = s.substr(0, pos);
        s.erase(0, pos + SELECT_DELIM.length());
        found_select = true;

    }
    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    if (s.at(0) == '<' && s.at(s.length() - 1) == '>') {
        return splitTokensByDelimiter(s.substr(1, s.length() - 2), ",");
    } else {
        return vector<string>{s};
    }
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

// splits by such that, pattern and with
// returns an empty vector if it is invalid
tuple<string, vector<string>, vector<string>> SelectClauseParser::splitTokensByClauses(string input)
{
    string such_that_delim = "such that ";
    string pattern_delim = "pattern ";
    vector<string> delimiters{such_that_delim, pattern_delim};
    select_clause = "";
    vector<string> such_that_clauses;
    vector<string> pattern_clauses;

    size_t pos = 0;
    string token;
    bool last_found_such_that = false;
    bool last_found_pattern = false;

    while ((pos = input.find(such_that_delim)) != string::npos || (pos = input.find(pattern_delim)) != string::npos) {
        pos = min(input.find(such_that_delim), input.find(pattern_delim));
        token = input.substr(0, pos);
        if (!last_found_such_that && !last_found_pattern) {
            select_clause = token;
        } else if (last_found_such_that) {
            such_that_clauses.push_back(token);
        } else if (last_found_pattern) {
            pattern_clauses.push_back(token);
        }

        if (input.substr(pos, such_that_delim.length()) == such_that_delim) {
            input.erase(0, pos + such_that_delim.length());
            last_found_such_that = true;
            last_found_pattern = false;
        } else if (input.substr(pos, pattern_delim.length()) == pattern_delim) {
            input.erase(0, pos + pattern_delim.length());
            last_found_pattern = true;
            last_found_such_that = false;
        }
    }
    if (!last_found_such_that && !last_found_pattern) {
        select_clause = input;
    } else if (last_found_such_that) {
        such_that_clauses.push_back(input);
    } else if (last_found_pattern) {
        pattern_clauses.push_back(input);
    }

    return make_tuple(select_clause, such_that_clauses, pattern_clauses);
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

