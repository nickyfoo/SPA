#include "SelectClauseParser.h"
#include "SuchThatClause.h"
#include <iostream>
#include <tuple>

SelectClauseParser::SelectClauseParser() {
    this->synonym_to_entity = nullptr;
}

SelectClauseParser* SelectClauseParser::instance = nullptr;

SelectClauseParser *SelectClauseParser::getInstance() {
    if (!instance)
        instance = new SelectClauseParser;
    return instance;
}

void SelectClauseParser::setSelectClause(std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entity, std::string select_clause)
{
    this->synonym_to_entity = synonym_to_entity;
    select_clause.erase(remove(select_clause.begin(), select_clause.end(),
                                        '\n'), select_clause.end());
    this->select_clause = select_clause;
}

PQLQuery *SelectClauseParser::getClauses()
{
    std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> clauses = splitTokensByClauses(select_clause);
    std::string select_clause = std::get<0>(clauses);
    std::vector<std::string> such_that_clauses = std::get<1>(clauses);
    std::vector<std::string> pattern_clauses = std::get<2>(clauses);

    auto *select_ret = new std::vector<std::string>();
    auto *such_that_ret = new std::vector<SuchThatClause*>();
    auto *pattern_ret = new std::vector<Pattern*>();

    std::vector<std::string> select_clauses = splitSelect(select_clause);
    if (select_clauses.empty()) { // invalid select syntax
        return nullptr;
    }
    for (const std::string& select : select_clauses) {
        if (synonym_to_entity->find(select) == synonym_to_entity->end()) { // not found in hashmap
            return nullptr;
        } else {
            select_ret->push_back(select);
        }
    }

    for (const std::string& such_that_clause : such_that_clauses) {
        SuchThatClause* relationship = getRelationshipClause(such_that_clause);
        if (relationship == nullptr) {
            return nullptr;
        }
        such_that_ret->push_back(relationship);
    }

    for (const std::string& pattern_clause : pattern_clauses) {
        Pattern* pattern = getPatternClause(pattern_clause);
        if (pattern == nullptr) {
            return nullptr;
        }
        pattern_ret->push_back(pattern);
    }

    auto* ret = new PQLQuery(select_ret, such_that_ret, pattern_ret, synonym_to_entity);
    return ret;

}

SuchThatClause* SelectClauseParser::getRelationshipClause(std::string relationship_statement) {
    if (relationship_statement.empty()) {
        return nullptr;
    }
    relationship_statement.erase(remove(relationship_statement.begin(), relationship_statement.end(),
                                        ' '), relationship_statement.end());

    std::vector<std::string> relationship_clauses = splitTokensByMultipleDelimiters(relationship_statement, "(,)");
    if (relationship_clauses.size() > 3) { // should only contain RelRef, left_ref and right_ref
        return nullptr;
    }

    auto* relationship = new SuchThatClause(relationship_clauses.at(0));
    if (relationship->getType() == RelRef::None) { // invalid relation
        return nullptr;
    }

    std::string left_ref = relationship_clauses.at(1);
    std::string right_ref = relationship_clauses.at(2);
    SuchThatRef* left_such_that_ref = makeSuchThatRef(relationship, left_ref);
    SuchThatRef* right_such_that_ref = makeSuchThatRef(relationship, right_ref);

    if (left_such_that_ref == nullptr || right_such_that_ref == nullptr) {
        return nullptr;
    }

    if (relationship->setRef(left_such_that_ref, right_such_that_ref)) {
        return relationship;
    }
    return nullptr;

}

Pattern* SelectClauseParser::getPatternClause(std::string pattern_statement) {
    if (pattern_statement.empty()) {
        return nullptr;
    }
    pattern_statement.erase(remove(pattern_statement.begin(), pattern_statement.end(),
                                   ' '), pattern_statement.end());
    std::vector<std::string> pattern_clauses = splitTokensByMultipleDelimiters(pattern_statement, "(,)");
    if (pattern_clauses.size() > 3) {
        return nullptr;
    }

    std::string synonym = pattern_clauses.at(0);
    std::string left_ref = pattern_clauses.at(1);
    std::string right_ref = pattern_clauses.at(2);
    auto* pattern = new Pattern(synonym);
    if (synonym_to_entity->find(synonym) == synonym_to_entity->end()
            || synonym_to_entity->find(synonym)->second->getType() != EntityType::Assign) {
        return nullptr;
    }
    if ((synonym_to_entity->find(left_ref) != synonym_to_entity->end() && synonym_to_entity->find(left_ref)->second->getType() == EntityType::Variable)
        || isValidIdentifier(left_ref)) {
        EntityDeclaration* left;

        if (synonym_to_entity->find(left_ref) != synonym_to_entity->end()) {
            left = synonym_to_entity->at(left_ref);
        } else {
            left = new EntityDeclaration(left_ref);
        }

        if (pattern->setRef(left, right_ref)) {
            return pattern;
        }
    }
    return nullptr;
}

SuchThatRef* SelectClauseParser::makeSuchThatRef(SuchThatClause *such_that_clause, std::string ref) {
    SuchThatRef* ret;
    StmtRef stmt_ref;
    EntRef ent_ref;

    // existing synonym
    if (synonym_to_entity->find(ref) != synonym_to_entity->end()) {
        EntityType entity_type = synonym_to_entity->find(ref)->second->getType();
        switch(entity_type) {
            case EntityType::Assign: case EntityType::Call: case EntityType::If: case EntityType::Print: case EntityType::Read: case EntityType::Stmt: case EntityType::While: {
                stmt_ref.setSynonym(ref);
                ret = new SuchThatRef(stmt_ref);
                break;
            }
            case EntityType::Constant: case EntityType::Procedure: case EntityType::Variable: {
                ent_ref.setSynonym(ref);
                ret = new SuchThatRef(ent_ref);
                break;
            }
            default:
                return nullptr;
        }
    } else if (isInteger(ref)) { // statement number
        stmt_ref.setStmtNum(std::stoi(ref));
        ret = new SuchThatRef(stmt_ref);
    } else if (ref == "_") { // wild card
        stmt_ref.setWildCard();
        ret = new SuchThatRef(stmt_ref);
    } else if (isValidIdentifier(ref)) {
        ent_ref.setArgument(ref);
        ret = new SuchThatRef(ent_ref);
    } else {
        return nullptr;
    }

    if (ret->getType() != SuchThatRefType::None) { // invalid syntax
        return ret;
    }

    return nullptr;
}

// Function that returns true if str
// is a valid identifier
bool SelectClauseParser::isValidIdentifier(std::string str) {
    if (str == "_") {
        return true;
    }

    if (!((str[0] == '\'' && str[str.length() - 1] == '\'') || (str[0] == '\"' && str[str.length() - 1] == '\"'))) {
        return false;
    }

    // If first character is invalid
    if (!((str[1] >= 'a' && str[1] <= 'z') || (str[1] >= 'A' && str[1] <= 'Z') || str[1] == '_')) {
        return false;
    }


    // Traverse the string for the rest of the characters
    for (int i = 2; i < str.length() - 1; i++) {
        if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') ||
        (str[i] >= '0' && str[i] <= '9') || str[i] == '_')) {
            return false;
        }
    }

    // String is a valid identifier
    return true;
}

bool SelectClauseParser::isInteger(std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

std::vector<std::string> SelectClauseParser::splitSelect(std::string& s) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t pos = s.find_first_not_of(WHITESPACE);
    if (pos == std::string::npos) {
        return {};
    } else {
        s.erase(0, pos);
    }

    const std::string SELECT_DELIM = "Select";
    bool found_select = false;
    std::string token;

    while ((pos = s.find(SELECT_DELIM)) != std::string::npos) {
        if (found_select) { // two select clauses found
            return {};
        }
        s.erase(0, pos + SELECT_DELIM.length());
        found_select = true;

    }
    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    if (s.empty()) {
        return {};
    } else if (s.at(0) == '<' && s.at(s.length() - 1) == '>') {
        return splitTokensByDelimiter(s.substr(1, s.length() - 2), ",");
    } else {
        return std::vector<std::string>{s};
    }
}

std::vector<std::string> SelectClauseParser::splitTokensByDelimiter(std::string input, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t pos;
    std::string token;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        input.erase(0, pos + delimiter.length());
    }
    tokens.push_back(input);
    return tokens;
}

// splits by such that, pattern and with
std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> SelectClauseParser::splitTokensByClauses(const std::string& input)
{
    std::string such_that_delim = "such that ";
    std::string pattern_delim = "pattern ";
    std::vector<std::string> delimiters{such_that_delim, pattern_delim};
    std::string select_clause;
    std::vector<std::string> such_that_clauses;
    std::vector<std::string> pattern_clauses;

    size_t pos;
    std::string token;
    bool last_found_such_that = false;
    bool last_found_pattern = false;

    std::stringstream ss;
    // first pass to remove all whitespaces within brackets and before brackets
    bool whitespace_found = true;
    bool open_bracket_found = false;
    for (char c : input) {
        if (c == ' ') {
            if (!whitespace_found) {
                ss << c;
                whitespace_found = true;
            }
        } else if (c == '(' || c == '<') {
            std::string curr_ss = ss.str();
            if (curr_ss.at(curr_ss.length() - 1) == ' ') {
                ss.str("");
                ss << curr_ss.substr(0, curr_ss.length() - 1);
            }
            ss << c;
            open_bracket_found = true;
            whitespace_found = true;
        } else if (c == ')' || c == '>') {
            ss << c;
            open_bracket_found = false;
            whitespace_found = false;
        } else {
            ss << c;
            if (!open_bracket_found) {
                whitespace_found = false;
            }
        }
    }

    std::string clean_input = ss.str();
    pos = clean_input.find(' ');
    if (pos == std::string::npos) { // find first whitespace
        return make_tuple(select_clause, such_that_clauses, pattern_clauses);
    }

    pos = clean_input.find(' ', pos + 1);
    if (pos == std::string::npos) { // find second white space
        select_clause = clean_input;
        return make_tuple(select_clause, such_that_clauses, pattern_clauses);
    }

    select_clause = clean_input.substr(0, pos); // adds the select clause
    clean_input.erase(0, pos);

    while (clean_input.find(such_that_delim) != std::string::npos || clean_input.find(pattern_delim) != std::string::npos) {
        pos = std::min(clean_input.find(such_that_delim), clean_input.find(pattern_delim)); // find the earliest clause
        token = clean_input.substr(0, pos);
        if (last_found_such_that) {
            such_that_clauses.push_back(token);
        } else if (last_found_pattern) {
            pattern_clauses.push_back(token);
        }

        if (clean_input.substr(pos, such_that_delim.length()) == such_that_delim) {
            clean_input.erase(0, pos + such_that_delim.length());
            last_found_such_that = true;
            last_found_pattern = false;
        } else if (clean_input.substr(pos, pattern_delim.length()) == pattern_delim) {
            clean_input.erase(0, pos + pattern_delim.length());
            last_found_pattern = true;
            last_found_such_that = false;
        }
    }
    if (!last_found_such_that && !last_found_pattern) {
        select_clause.append(clean_input); // error
    }
    if (last_found_such_that) {
        such_that_clauses.push_back(clean_input);
    } else if (last_found_pattern) {
        pattern_clauses.push_back(clean_input);
    }

    return make_tuple(select_clause, such_that_clauses, pattern_clauses);
}

std::vector<std::string> SelectClauseParser::splitTokensByMultipleDelimiters(const std::string& input, const std::string& delimiters)
{
    std::stringstream input_stream;
    input_stream << input;
    std::vector<std::string> tokens;

    std::string line;
    while(getline(input_stream, line))
    {
        size_t prev = 0, pos;
        while ((pos = line.find_first_of(delimiters, prev)) != std::string::npos)
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

