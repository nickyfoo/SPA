#include <iostream>
#include <tuple>
#include "select_clause_parser.h"
#include "such_that_clause.h"

SelectClauseParser::SelectClauseParser() {
  this->synonym_to_entity_ = nullptr;
}

SelectClauseParser *SelectClauseParser::instance = nullptr;

SelectClauseParser *SelectClauseParser::get_instance() {
  if (!instance)
    instance = new SelectClauseParser;
  return instance;
}

void SelectClauseParser::set_select_clause(std::unordered_map<std::string,
                                                              EntityDeclaration *> *syn_to_ent,
                                           std::string select_clause) {
  this->synonym_to_entity_ = syn_to_ent;
  select_clause.erase(remove(select_clause.begin(), select_clause.end(),
                             '\n'), select_clause.end());
  this->select_statement_ = select_clause;
}

PQLQuery *SelectClauseParser::get_clauses() {
  std::tuple<std::string,
             std::vector<std::string>,
             std::vector<std::string>> clauses;
  clauses = SplitTokensByClauses(select_statement_);
  std::string select_clause = std::get<0>(clauses);
  std::vector<std::string> such_that_clauses = std::get<1>(clauses);
  std::vector<std::string> pattern_clauses = std::get<2>(clauses);

  auto *select_ret = new std::vector<std::string>();
  auto *such_that_ret = new std::vector<SuchThatClause *>();
  auto *pattern_ret = new std::vector<PatternClause *>();

  std::vector<std::string> select_clauses = SplitSelect(select_clause);
  if (select_clauses.empty()) {  // invalid select syntax
    return nullptr;
  }

  for (const std::string &select : select_clauses) {
    // not found in hashmap
    if (synonym_to_entity_->find(select) == synonym_to_entity_->end()) {
      return nullptr;
    } else {
      select_ret->push_back(select);
    }
  }

  for (const std::string &such_that_clause : such_that_clauses) {
    SuchThatClause *relationship = MakeSuchThatClause(such_that_clause);
    if (relationship == nullptr) {
      return nullptr;
    }
    such_that_ret->push_back(relationship);
  }

  for (const std::string &pattern_clause : pattern_clauses) {
    PatternClause *pattern = MakePatternClause(pattern_clause);
    if (pattern == nullptr) {
      return nullptr;
    }
    pattern_ret->push_back(pattern);
  }

  // check for whether there are repeated synonyms
  bool has_one_repeated_synonym = false;
  bool has_two_repeated_synonyms = false;
  for (SuchThatClause *relationship : *such_that_ret) {
    std::string left_relationship_str, right_relationship_str;
    if (relationship->get_left_ref()->get_type() == SuchThatRefType::Statement) {
      left_relationship_str = relationship->get_left_ref()->get_stmt_ref().get_value();
    } else {
      left_relationship_str = relationship->get_left_ref()->get_ent_ref().get_value();
    }

    if (relationship->get_right_ref()->get_type() == SuchThatRefType::Statement) {
      right_relationship_str = relationship->get_right_ref()->get_stmt_ref().get_value();
    } else {
      right_relationship_str = relationship->get_right_ref()->get_ent_ref().get_value();
    }

    for (PatternClause *pattern : *pattern_ret) {
      if (left_relationship_str != "_" &&
      (pattern->get_synonym()->get_synonym() == left_relationship_str
      || pattern->get_left_ref()->get_value() == left_relationship_str)) {
        has_one_repeated_synonym = true;
      }

      if (right_relationship_str != "_" &&
      (pattern->get_synonym()->get_synonym() == right_relationship_str
      || pattern->get_left_ref()->get_value() == right_relationship_str)) {
        has_one_repeated_synonym ? has_two_repeated_synonyms = true
            : has_one_repeated_synonym = true;
      }
    }
  }

  auto *ret = new PQLQuery(select_ret, such_that_ret,
                           pattern_ret, synonym_to_entity_,
                           has_one_repeated_synonym, has_two_repeated_synonyms);

  return ret;
}

SuchThatClause *SelectClauseParser::MakeSuchThatClause(
    std::string relationship_statement) {
  if (relationship_statement.empty()) {
    return nullptr;
  }
  relationship_statement.erase(remove(relationship_statement.begin(),
                                      relationship_statement.end(), ' '),
                               relationship_statement.end());

  std::vector<std::string> relationship_clauses;
  relationship_clauses = SplitTokensByMultipleDelimiters(
      relationship_statement, "(,)");
  // should only contain RelRef, left_ref_ and right_ref_
  if (relationship_clauses.size() > 3) {
    return nullptr;
  }
  auto *relationship = new SuchThatClause(relationship_clauses.at(0));
  if (relationship->get_type() == RelRef::None) {  // invalid relation
    return nullptr;
  }

  std::string left_ref = relationship_clauses.at(1);
  std::string right_ref = relationship_clauses.at(2);
  SuchThatRef *left_such_that_ref = MakeSuchThatRefLeft(relationship, left_ref);
  SuchThatRef *right_such_that_ref = MakeSuchThatRefRight(relationship, right_ref);
  if (left_such_that_ref == nullptr || right_such_that_ref == nullptr) {
    return nullptr;
  }

  if (relationship->set_ref(left_such_that_ref, right_such_that_ref)) {
    return relationship;
  }
  return nullptr;
}

PatternClause *SelectClauseParser::MakePatternClause(
    std::string pattern_statement) {
  if (pattern_statement.empty()) {
    return nullptr;
  }
  pattern_statement.erase(remove(pattern_statement.begin(),
                                 pattern_statement.end(),
                                 ' '), pattern_statement.end());
  std::vector<std::string> pattern_clauses = SplitTokensByMultipleDelimiters(
      pattern_statement, "(,)");
  if (pattern_clauses.size() > 3) {
    return nullptr;
  }
  std::string synonym = pattern_clauses.at(0);
  std::string left_ref = pattern_clauses.at(1);
  std::string right_ref = pattern_clauses.at(2);
  auto *pattern = MakePatternRef(synonym, left_ref, right_ref);
  if (pattern == nullptr) {
    return nullptr;
  }

  return pattern;
}

PatternClause *SelectClauseParser::MakePatternRef(const std::string &synonym,
                                                  std::string left_ref,
                                                  std::string right_ref) {
  PatternClause *ret;
  auto *ent_ref = new EntRef();
  if ((synonym_to_entity_->find(synonym) != synonym_to_entity_->end())
      && (synonym_to_entity_->at(synonym)->get_type() == EntityType::Assign)) {
    ret = new PatternClause(synonym_to_entity_->find(synonym)->second);
  } else {
    return nullptr;
  }
  if ((synonym_to_entity_->find(left_ref) != synonym_to_entity_->end()) &&
      (synonym_to_entity_->at(left_ref)->get_type() == EntityType::Variable)) {
    ent_ref->set_synonym(left_ref);
  } else if (left_ref == "_") {
    ent_ref->set_wild_card();
  } else if (IsValidIdentifier(left_ref)) {
    ent_ref->set_argument(left_ref.substr(1, left_ref.length() - 2));
  } else {
    return nullptr;
  }
  if (ret->set_ref(ent_ref, right_ref)) {
    return ret;
  } else {
    return nullptr;
  }
}


SuchThatRef *SelectClauseParser::MakeSuchThatRefLeft(
    SuchThatClause *relationship, std::string left_ref) {
  SuchThatRef *left_such_that_ref;
  StmtRef left_stmt_ref;
  EntRef left_ent_ref;
  RelRef type = relationship->get_type();

  // existing synonym_
  if (synonym_to_entity_->find(left_ref) != synonym_to_entity_->end()) {
    EntityType entity_type = synonym_to_entity_->at(left_ref)->get_type();
    switch (entity_type) {
      case EntityType::Assign:
      case EntityType::Call:
      case EntityType::If:
      case EntityType::Print:
        if (type == RelRef::Modifies && entity_type == EntityType::Print) {
          return nullptr;
        }
      case EntityType::Read:
        if (type == RelRef::Uses && entity_type == EntityType::Read) {
          return nullptr;
        }
      case EntityType::Stmt:
        case EntityType::While: {
          left_stmt_ref.set_synonym(left_ref);
          left_stmt_ref.set_entity_type(entity_type);
          left_such_that_ref = new SuchThatRef(left_stmt_ref);
          break;
        }
      case EntityType::Procedure:
        if (type == RelRef::Uses || type == RelRef::Modifies) {
          left_ent_ref.set_synonym(left_ref);
          left_such_that_ref = new SuchThatRef(left_ent_ref);
          break;
        }
      case EntityType::Variable: {
        if (type != RelRef::Uses && type != RelRef::Modifies) {
          left_ent_ref.set_synonym(left_ref);
          left_such_that_ref = new SuchThatRef(left_ent_ref);
          break;
        }
      }
      case EntityType::Constant:
        default:
          return nullptr;;
    }
  } else if (IsInteger(left_ref)) {  // statement number
    left_stmt_ref.set_stmt_num(std::stoi(left_ref));
    left_such_that_ref = new SuchThatRef(left_stmt_ref);
  } else if (left_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies) {
      left_ent_ref.set_wild_card();
      left_such_that_ref = new SuchThatRef(left_ent_ref);
    } else {
      left_stmt_ref.set_wild_card();
      left_such_that_ref = new SuchThatRef(left_stmt_ref);
    }
  } else if (IsValidIdentifier(left_ref)) {
    left_ent_ref.set_argument(left_ref.substr(1, left_ref.length() - 2));
    left_such_that_ref = new SuchThatRef(left_ent_ref);
  } else {
    return nullptr;
  }

  if (left_such_that_ref->get_type() != SuchThatRefType::None) {  // invalid syntax
    return left_such_that_ref;
  }
  return nullptr;
}

SuchThatRef *SelectClauseParser::MakeSuchThatRefRight(
    SuchThatClause *relationship, std::string right_ref) {
  SuchThatRef *right_such_that_ref;
  StmtRef right_stmt_ref;
  EntRef right_ent_ref;
  RelRef type = relationship->get_type();

  // existing synonym_
  if (synonym_to_entity_->find(right_ref) != synonym_to_entity_->end()) {
    EntityType entity_type = synonym_to_entity_->at(right_ref)->get_type();
    switch (entity_type) {
      case EntityType::Variable: {
        if (type == RelRef::Uses || type == RelRef::Modifies) {
          right_ent_ref.set_synonym(right_ref);
          right_such_that_ref = new SuchThatRef(right_ent_ref);
          break;
        } else {
          return nullptr;
        }
      }
      case EntityType::Assign:
      case EntityType::Call:
      case EntityType::If:
      case EntityType::Print:
      case EntityType::Read:
      case EntityType::Stmt:
      case EntityType::While: {
        right_stmt_ref.set_synonym(right_ref);
        right_stmt_ref.set_entity_type(entity_type);
        right_such_that_ref = new SuchThatRef(right_stmt_ref);
        break;
      }
      case EntityType::Procedure:
      case EntityType::Constant:
      default:return nullptr;;
    }
  } else if (IsInteger(right_ref)) {  // statement number
    right_stmt_ref.set_stmt_num(std::stoi(right_ref));
    right_such_that_ref = new SuchThatRef(right_stmt_ref);
  } else if (right_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies) {
      right_ent_ref.set_wild_card();
      right_such_that_ref = new SuchThatRef(right_ent_ref);
    } else {
      right_stmt_ref.set_wild_card();
      right_such_that_ref = new SuchThatRef(right_stmt_ref);
    }
  } else if (IsValidIdentifier(right_ref)) {
    right_ent_ref.set_argument(right_ref.substr(1, right_ref.length() - 2));
    right_such_that_ref = new SuchThatRef(right_ent_ref);
  } else {
    return nullptr;
  }

  if (right_such_that_ref->get_type() != SuchThatRefType::None) {  // invalid syntax
    return right_such_that_ref;
  }
  return nullptr;
}

// Function that returns true if str is a valid identifier
bool SelectClauseParser::IsValidIdentifier(const std::string &str) {

  if (!((str[0] == '\'' && str[str.length() - 1] == '\'')
      || (str[0] == '\"' && str[str.length() - 1] == '\"'))) {
    return false;
  }

  // If first character is invalid
  if (!((str[1] >= 'a' && str[1] <= 'z') || (str[1] >= 'A' && str[1] <= 'Z')
      || str[1] == '_')) {
    return false;
  }


  // Traverse the string for the rest of the characters
  for (int i = 2; i < str.length() - 1; i++) {
    if (!((str[i] >= 'a' && str[i] <= 'z')
        || (str[i] >= 'A' && str[i] <= 'Z')
        || (str[i] >= '0' && str[i] <= '9')
        || str[i] == '_')) {
      return false;
    }
  }

  // String is a valid identifier
  return true;
}

bool SelectClauseParser::IsInteger(const std::string &str) {
  std::string::const_iterator it = str.begin();
  while (it != str.end() && isdigit(*it)) ++it;
  return !str.empty() && it == str.end();
}

std::vector<std::string> SelectClauseParser::SplitSelect(
    std::string select_clause) {
  const std::string WHITESPACE = " \n\r\t\f\v";
  size_t pos = select_clause.find_first_not_of(WHITESPACE);
  if (pos == std::string::npos) {
    return {};
  } else {
    select_clause.erase(0, pos);
  }

  const std::string SELECT_DELIM = "Select";
  bool found_select = false;
  std::string token;

  while ((pos = select_clause.find(SELECT_DELIM)) != std::string::npos) {
    select_clause.erase(0, pos + SELECT_DELIM.length());
    break;
  }

  select_clause.erase(remove(select_clause.begin(),
                             select_clause.end(), '\n'), select_clause.end());
  select_clause.erase(remove(select_clause.begin(), select_clause.end(), ' '),
                      select_clause.end());

  if (select_clause.empty()) {
    return {};
  } else if (select_clause.at(0) == '<'
      && select_clause.at(select_clause.length() - 1) == '>') {
    return SplitTokensByDelimiter(
        select_clause.substr(1, select_clause.length() - 2), ",");
  } else {
    return std::vector<std::string>{select_clause};
  }
}

std::vector<std::string> SelectClauseParser::SplitTokensByDelimiter(
    std::string input, const std::string &delimiter) {
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
std::tuple<std::string, std::vector<std::string>, std::vector<std::string>>
SelectClauseParser::SplitTokensByClauses(const std::string &input) {
  std::string SUCH_THAT_DELIM = "such that ";
  std::string PATTERN_DELIM = "pattern ";
  std::vector<std::string> delimiters{SUCH_THAT_DELIM, PATTERN_DELIM};
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
  std::stringstream prev_word_stream;
  for (char c : input) {
    if (c == ' ') {
      // extra check to account for such that clause without extra spaces
      std::string check_for_such = prev_word_stream.str();
      bool such_clause_found = false;
      if (!whitespace_found) {
        ss << c;
        prev_word_stream << c;
        whitespace_found = true;
        such_clause_found = true;
      }
      if (check_for_such == "such " && !such_clause_found) {
        prev_word_stream << c;
      } else if (check_for_such.substr(0, 4) != "such") {
        prev_word_stream.str("");
      }
    } else if (c == '(' || c == '<') {
      std::string curr_ss = ss.str();
      if (curr_ss.at(curr_ss.length() - 1) == ' ') {
        ss.str("");
        ss << curr_ss.substr(0, curr_ss.length() - 1);
      }
      ss << c;
      if (open_bracket_found) {  // additional open brackets found
        return make_tuple(select_clause, such_that_clauses, pattern_clauses);
      }
      open_bracket_found = true;
      whitespace_found = true;
    } else if (c == ')' || c == '>') {
      ss << c;
      if (!open_bracket_found) {  // additional close brackets found
        return make_tuple(select_clause, such_that_clauses, pattern_clauses);
      }
      open_bracket_found = false;
      whitespace_found = false;
    } else {
      ss << c;
      if (!open_bracket_found) {
        whitespace_found = false;
        prev_word_stream << c;
        std::string check_for_such_that = prev_word_stream.str();
        if (check_for_such_that == "such  that") {  // extra spaces between such that clause
          return make_tuple(select_clause, such_that_clauses, pattern_clauses);
        }
      }
    }
  }

  std::string clean_input = ss.str();
  pos = clean_input.find(' ');
  if (pos == std::string::npos) {  // find first whitespace
    return make_tuple(select_clause, such_that_clauses, pattern_clauses);
  }

  pos = clean_input.find(' ', pos + 1);
  if (pos == std::string::npos) {  // find second white space
    select_clause = clean_input;
    return make_tuple(select_clause, such_that_clauses, pattern_clauses);
  }

  select_clause = clean_input.substr(0, pos);  // adds the select clause
  clean_input.erase(0, pos);

  while (clean_input.find(SUCH_THAT_DELIM) != std::string::npos
      || clean_input.find(PATTERN_DELIM) != std::string::npos) {
    // find the earliest clause
    pos = std::min(clean_input.find(SUCH_THAT_DELIM),
                   clean_input.find(PATTERN_DELIM));
    token = clean_input.substr(0, pos);
    if (last_found_such_that) {
      such_that_clauses.push_back(token);
    } else if (last_found_pattern) {
      pattern_clauses.push_back(token);
    }

    if (clean_input.substr(pos, SUCH_THAT_DELIM.length())
        == SUCH_THAT_DELIM) {
      clean_input.erase(0, pos + SUCH_THAT_DELIM.length());
      last_found_such_that = true;
      last_found_pattern = false;
    } else if (clean_input.substr(pos, PATTERN_DELIM.length())
        == PATTERN_DELIM) {
      clean_input.erase(0, pos + PATTERN_DELIM.length());
      last_found_pattern = true;
      last_found_such_that = false;
    }
  }
  if (!last_found_such_that && !last_found_pattern) {
    select_clause.append(clean_input);  // error
  }
  if (last_found_such_that) {
    such_that_clauses.push_back(clean_input);
  } else if (last_found_pattern) {
    pattern_clauses.push_back(clean_input);
  }

  return make_tuple(select_clause, such_that_clauses, pattern_clauses);
}

std::vector<std::string> SelectClauseParser::SplitTokensByMultipleDelimiters(
    const std::string &input, const std::string &delimiters) {
  std::stringstream input_stream;
  input_stream << input;
  std::vector<std::string> tokens;

  std::string line;
  while (getline(input_stream, line)) {
    size_t prev = 0, pos;
    while ((pos = line.find_first_of(delimiters, prev))
        != std::string::npos) {
      if (pos >= prev)
        tokens.push_back(line.substr(prev, pos - prev));
      prev = pos + 1;
    }
    if (prev < line.length())
      tokens.push_back(line.substr(prev, std::string::npos));
  }
  return tokens;
}
