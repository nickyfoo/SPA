#include <iostream>
#include <tuple>
#include <utility>
#include "select_clause_parser.h"
#include "such_that_clause.h"
#include "line_ref.h"

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

std::tuple<std::vector<std::string> *,
           std::vector<SuchThatClause *> *,
           std::vector<PatternClause *> *,
           std::vector<WithClause *> *,
           std::unordered_map<std::string, EntityDeclaration *> *> *SelectClauseParser::get_clauses() {
  std::tuple<std::string,
             std::vector<std::string>,
             std::vector<std::string>,
             std::vector<std::string>> clauses;
  clauses = SplitTokensByClauses(select_statement_);
  std::string select_clause = std::get<0>(clauses);
  std::vector<std::string> such_that_clauses = std::get<1>(clauses);
  std::vector<std::string> pattern_clauses = std::get<2>(clauses);
  std::vector<std::string> with_clauses = std::get<3>(clauses);
  auto *select_ret = new std::vector<std::string>();
  auto *such_that_ret = new std::vector<SuchThatClause *>();
  auto *pattern_ret = new std::vector<PatternClause *>();
  auto *with_ret = new std::vector<WithClause *>();
  std::vector<std::string> select_clauses = SplitSelect(select_clause);
  if (select_clauses.empty()) {  // invalid select syntax
    return nullptr;
  }
  for (const std::string &select : select_clauses) {
    if (select == "BOOLEAN" && select_clauses.size() == 1) {
      // do nothing, flag to evaluator that it requires a boolean output
    } else if (synonym_to_entity_->find(select) != synonym_to_entity_->end()) {
      select_ret->push_back(select);
    } else {
      if (IsValidAttr(select)) {
        select_ret->push_back(select);
      } else {
        return nullptr;
      }
    }
  }
  for (const std::string &such_that_clause : such_that_clauses) {
    std::vector<SuchThatClause *> *relationship = MakeSuchThatClause(such_that_clause);
    if (relationship == nullptr) {
      return nullptr;
    }
    for (SuchThatClause *clause : *relationship) {
      such_that_ret->push_back(clause);
    }
  }
  for (const std::string &pattern_clause : pattern_clauses) {
    std::vector<PatternClause *> *pattern = MakePatternClause(pattern_clause);
    if (pattern == nullptr) {
      return nullptr;
    }
    for (PatternClause *clause : *pattern) {
      pattern_ret->push_back(clause);
    }
  }

  for (const std::string &with_clause : with_clauses) {
    std::vector<WithClause *> *with = MakeWithClause(with_clause);
    if (with == nullptr) {
      return nullptr;
    }
    for (WithClause *clause : *with) {
      with_ret->push_back(clause);
    }
  }

//  // check for whether there are repeated synonyms
//  bool has_one_repeated_synonym = false;
//  bool has_two_repeated_synonyms = false;
//  for (SuchThatClause *relationship : *such_that_ret) {
//    std::string left_relationship_str, right_relationship_str;
//    if (relationship->get_left_ref()->get_type() == SuchThatRefType::Statement) {
//      left_relationship_str = relationship->get_left_ref()->get_stmt_ref().get_value();
//    } else {
//      left_relationship_str = relationship->get_left_ref()->get_ent_ref().get_value();
//    }
//
//    if (relationship->get_right_ref()->get_type() == SuchThatRefType::Statement) {
//      right_relationship_str = relationship->get_right_ref()->get_stmt_ref().get_value();
//    } else {
//      right_relationship_str = relationship->get_right_ref()->get_ent_ref().get_value();
//    }
//
//    for (PatternClause *pattern : *pattern_ret) {
//      if (left_relationship_str != "_" &&
//          (pattern->get_synonym()->get_synonym() == left_relationship_str
//              || pattern->get_variable()->get_value() == left_relationship_str)) {
//        has_one_repeated_synonym = true;
//      }
//
//      if (right_relationship_str != "_" &&
//          (pattern->get_synonym()->get_synonym() == right_relationship_str
//              || pattern->get_variable()->get_value() == right_relationship_str)) {
//        has_one_repeated_synonym ? has_two_repeated_synonyms = true
//                                 : has_one_repeated_synonym = true;
//      }
//    }
//  }

  auto ret = new std::tuple<std::vector<std::string> *,
                            std::vector<SuchThatClause *> *,
                            std::vector<PatternClause *> *,
                            std::vector<WithClause *> *,
                            std::unordered_map<std::string, EntityDeclaration *> *>(
                                select_ret, such_that_ret,
                                pattern_ret, with_ret, synonym_to_entity_);

  return ret;
}

std::vector<SuchThatClause *> *SelectClauseParser::MakeSuchThatClause(
    const std::string& relationship_statement) {
  auto *ret = new std::vector<SuchThatClause *>();
  if (relationship_statement.empty()) {
    return nullptr;
  }

  std::vector<std::vector<std::string>> relationship_clauses;
  relationship_clauses = SplitTokensByBrackets(relationship_statement);

  for (std::vector<std::string> relationship_clause : relationship_clauses) {
    if (relationship_clause.size() != 3) {
      return nullptr;
    }

    auto *relationship = new SuchThatClause(relationship_clause.at(0));
    if (relationship->get_type() == RelRef::None) {  // invalid relation
      return nullptr;
    }

    std::string left_ref = relationship_clause.at(1);
    std::string right_ref = relationship_clause.at(2);
    SuchThatRef *left_such_that_ref = MakeSuchThatRefLeft(relationship, left_ref);
    SuchThatRef *right_such_that_ref = MakeSuchThatRefRight(relationship, right_ref);
    if (left_such_that_ref == nullptr || right_such_that_ref == nullptr) {
      return nullptr;
    }

    if (relationship->set_ref(left_such_that_ref, right_such_that_ref)) {
      ret->push_back(relationship);
    } else {
      return nullptr;
    }
  }

  return ret;
}

std::vector<PatternClause *> *SelectClauseParser::MakePatternClause(
    const std::string& pattern_statement) {
  auto *ret = new std::vector<PatternClause *>();
  if (pattern_statement.empty()) {
    return nullptr;
  }

  std::vector<std::vector<std::string>> pattern_clauses = SplitTokensByBrackets(
      pattern_statement);
  for (std::vector<std::string> pattern_clause : pattern_clauses) {
    if (pattern_clause.size() < 3 || pattern_clause.size() > 4) {
      return nullptr;
    }
    std::string synonym = pattern_clause.at(0);
    std::string left_ref = pattern_clause.at(1);
    std::string right_ref = pattern_clause.at(2);
    if (pattern_clause.size() == 3) {  // assign and while
      auto *pattern = MakePatternRef(synonym, left_ref, right_ref);
      if (pattern == nullptr ||
          !(pattern->get_type() == EntityType::Assign ||
              pattern->get_type() == EntityType::While)) {
        return nullptr;
      } else {
        ret->push_back(pattern);
      }
    } else {  // if
      std::string last_ref = pattern_clause.at(3);
      auto *pattern = MakePatternRef(synonym, left_ref, right_ref);
      if (last_ref != "_" || pattern == nullptr || pattern->get_type() != EntityType::If) {
        return nullptr;
      } else {
        ret->push_back(pattern);
      }
    }
  }

  return ret;
}

std::vector<WithClause *> *SelectClauseParser::MakeWithClause(
    const std::string& with_statement) {
  auto *ret = new std::vector<WithClause *>();
  if (with_statement.empty()) {
    return nullptr;
  }

  std::vector<std::pair<std::string, std::string>> with_clauses =
      SplitTokensByEqual(with_statement);
  if (with_clauses.empty()) {
    return nullptr;
  }
  for (const std::pair<std::string, std::string>& with_clause : with_clauses) {
    const std::string WHITESPACE = " \n\r\t\f\v";

    std::string left_ref = with_clause.first;
    std::string right_ref = with_clause.second;

    size_t start = left_ref.find_first_not_of(WHITESPACE);
    if (start != std::string::npos) left_ref = left_ref.substr(start);
    size_t end = left_ref.find_last_not_of(WHITESPACE);
    if (end != std::string::npos) left_ref = left_ref.substr(0, end + 1);

    start = right_ref.find_first_not_of(WHITESPACE);
    if (start != std::string::npos) right_ref = right_ref.substr(start);
    end = right_ref.find_last_not_of(WHITESPACE);
    if (end != std::string::npos) right_ref = right_ref.substr(0, end + 1);

    auto *with = MakeWithRef(left_ref, right_ref);
    if (with == nullptr) {
      return nullptr;
    } else {
      ret->push_back(with);
    }
  }
  return ret;
}

PatternClause *SelectClauseParser::MakePatternRef(const std::string &synonym,
                                                  const std::string& left_ref,
                                                  const std::string& right_ref) {
  PatternClause *ret;
  auto *ent_ref = new EntRef();
  if ((synonym_to_entity_->find(synonym) != synonym_to_entity_->end())
      && (synonym_to_entity_->at(synonym)->get_type() == EntityType::Assign
          || synonym_to_entity_->at(synonym)->get_type() == EntityType::If
          || synonym_to_entity_->at(synonym)->get_type() == EntityType::While)) {
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

WithClause *SelectClauseParser::MakeWithRef(const std::string& left_ref,
                                            const std::string& right_ref) {
  std::string left_str;
  std::string right_str;
  EntityType left_type;
  EntityType right_type;
  AttrValueType left_attr_value_type;
  AttrValueType right_attr_value_type;

  if (IsInteger(left_ref)) {  // set as EntityType::None if it is an integer
    left_str = left_ref;
    left_type = EntityType::None;
    left_attr_value_type = AttrValueType::Integer;
  } else if (IsValidIdentifier(left_ref)) {  // set as EntityType::None if it is an ident
    left_str = left_ref.substr(1, left_ref.length() - 2);
    left_type = EntityType::None;
    left_attr_value_type = AttrValueType::Name;
  } else {
    std::tie(left_str, left_type, left_attr_value_type) = GetWithRefTypeAndAttrValueType(left_ref);
    if (left_type == EntityType::None) {  // not valid ref
      return nullptr;
    }
  }
  if (IsInteger(right_ref)) {  // set as EntityType::None if it is an integer
    right_str = right_ref;
    right_type = EntityType::None;
    right_attr_value_type = AttrValueType::Integer;
  } else if (IsValidIdentifier(right_ref)) {  // set as EntityType::None if it is an ident
    right_str = right_ref.substr(1, right_ref.length() - 2);
    right_type = EntityType::None;
    right_attr_value_type = AttrValueType::Name;
  } else {
    std::tie(right_str, right_type, right_attr_value_type) =
        GetWithRefTypeAndAttrValueType(right_ref);
    if (right_type == EntityType::None) {  // not valid ref
      return nullptr;
    }
  }

  if (left_attr_value_type != right_attr_value_type) {  // must be same attribute value types
    return nullptr;
  }

  auto *ret = new WithClause(left_str, left_type, left_attr_value_type,
                                   right_str, right_type, right_attr_value_type);
  return ret;
}

std::tuple<std::string, EntityType, AttrValueType>
SelectClauseParser::GetWithRefTypeAndAttrValueType(std::string ref) {
  std::vector<std::string> synonym_attribute = SplitTokensByDelimiter(ref, ".");
  if (synonym_attribute.size() == 1) {  // prog line
    std::string synonym = synonym_attribute.at(0);
    if (synonym_to_entity_->find(synonym) != synonym_to_entity_->end()) {
      if (synonym_to_entity_->at(synonym)->get_type() == EntityType::ProgLine) {
        return std::make_tuple(synonym, EntityType::ProgLine, AttrValueType::Integer);
      }
    }
  } else if (synonym_attribute.size() == 2) {
    std::string synonym = synonym_attribute.at(0);
    std::string attribute = synonym_attribute.at(1);

    if (synonym_to_entity_->find(synonym) != synonym_to_entity_->end()) {
      EntityType type = synonym_to_entity_->find(synonym)->second->get_type();
      AttrValueType attr_value_type = AttrValueType::None;
      switch (type) {
        case EntityType::Procedure:
          if (attribute == "procName")
            attr_value_type = AttrValueType::Name;
          break;
        case EntityType::Variable:
          if (attribute == "varName")
            attr_value_type = AttrValueType::Name;
          break;
        case EntityType::Constant:
          if (attribute == "value")
            attr_value_type = AttrValueType::Integer;
          break;
        case EntityType::Call:
          if (attribute == "procName") {
            attr_value_type = AttrValueType::Name;
            break;
          } else if (attribute == "stmt#") {
            attr_value_type = AttrValueType::Integer;
            break;
          }
        case EntityType::Print:
        case EntityType::Read:
          if (attribute == "varName") attr_value_type = AttrValueType::Name;
          else if (attribute == "stmt#") attr_value_type = AttrValueType::Integer;
          break;
        case EntityType::Stmt:
        case EntityType::While:
        case EntityType::If:
        case EntityType::Assign:
          if (attribute == "stmt#") {
            attr_value_type = AttrValueType::Integer;
          }
          break;
        default:break;
      }
      if (attr_value_type != AttrValueType::None) {
        return std::make_tuple(synonym, type, attr_value_type);
      }
    }
  }
  return std::make_tuple("", EntityType::None, AttrValueType::None);  // not a valid with clause
}

SuchThatRef *SelectClauseParser::MakeSuchThatRefLeft(
    SuchThatClause *relationship, std::string left_ref) {
  SuchThatRef *left_such_that_ref;
  StmtRef left_stmt_ref;
  EntRef left_ent_ref;
  LineRef left_line_ref;
  RelRef type = relationship->get_type();

  // existing synonym_
  if (synonym_to_entity_->find(left_ref) != synonym_to_entity_->end()) {
    EntityType entity_type = synonym_to_entity_->at(left_ref)->get_type();
    switch (entity_type) {
      case EntityType::Assign:
        if (type == RelRef::Affects || type == RelRef::AffectsT) {
          left_stmt_ref.set_synonym(left_ref);
          left_stmt_ref.set_entity_type(entity_type);
          left_such_that_ref = new SuchThatRef(left_stmt_ref);
          break;
        }
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
      case EntityType::ProgLine:
      case EntityType::While: {
        if (type == RelRef::Next || type == RelRef::NextT) {
          left_line_ref.set_synonym(left_ref);
          left_line_ref.set_entity_type(entity_type);
          left_such_that_ref = new SuchThatRef(left_line_ref);
          break;
        }
        left_stmt_ref.set_synonym(left_ref);
        left_stmt_ref.set_entity_type(entity_type);
        left_such_that_ref = new SuchThatRef(left_stmt_ref);
        break;
      }
      case EntityType::Procedure:
        if (type == RelRef::Uses || type == RelRef::Modifies
            || type == RelRef::Calls || type == RelRef::CallsT) {
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
        return nullptr;
    }
  } else if (IsInteger(left_ref)) {  // statement number
    if (type == RelRef::Next || type == RelRef::NextT) {
      left_line_ref.set_line_num(std::stoi(left_ref));
      left_such_that_ref = new SuchThatRef(left_line_ref);
    } else {
      left_stmt_ref.set_stmt_num(std::stoi(left_ref));
      left_such_that_ref = new SuchThatRef(left_stmt_ref);
    }
  } else if (left_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies
        || type == RelRef::Calls || type == RelRef::CallsT) {
      left_ent_ref.set_wild_card();
      left_such_that_ref = new SuchThatRef(left_ent_ref);
    } else if (type == RelRef::Next || type == RelRef::NextT) {
      left_line_ref.set_wild_card();
      left_such_that_ref = new SuchThatRef(left_line_ref);
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
  LineRef right_line_ref;
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
        if (type == RelRef::Affects || type == RelRef::AffectsT) {
          right_stmt_ref.set_synonym(right_ref);
          right_stmt_ref.set_entity_type(entity_type);
          right_such_that_ref = new SuchThatRef(right_stmt_ref);
          break;
        }
      case EntityType::Call:
      case EntityType::If:
      case EntityType::Print:
      case EntityType::Read:
      case EntityType::Stmt:
      case EntityType::ProgLine:
      case EntityType::While: {
        if (type == RelRef::Next || type == RelRef::NextT) {
          right_line_ref.set_synonym(right_ref);
          right_line_ref.set_entity_type(entity_type);
          right_such_that_ref = new SuchThatRef(right_line_ref);
          break;
        } else if (type != RelRef::Affects && type != RelRef::AffectsT) {
          right_stmt_ref.set_synonym(right_ref);
          right_stmt_ref.set_entity_type(entity_type);
          right_such_that_ref = new SuchThatRef(right_stmt_ref);
          break;
        }
      }
      case EntityType::Procedure:
        if (type == RelRef::Calls || type == RelRef::CallsT) {
          right_ent_ref.set_synonym(right_ref);
          right_such_that_ref = new SuchThatRef(right_ent_ref);
          break;
        }
      case EntityType::Constant:
      default:return nullptr;
    }
  } else if (IsInteger(right_ref)) {  // statement number
    if (type == RelRef::Next || type == RelRef::NextT) {
      right_line_ref.set_line_num(std::stoi(right_ref));
      right_such_that_ref = new SuchThatRef(right_line_ref);
    } else {
      right_stmt_ref.set_stmt_num(std::stoi(right_ref));
      right_such_that_ref = new SuchThatRef(right_stmt_ref);
    }
  } else if (right_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies
        || type == RelRef::Calls || type == RelRef::CallsT) {
      right_ent_ref.set_wild_card();
      right_such_that_ref = new SuchThatRef(right_ent_ref);
    } else if (type == RelRef::Next || type == RelRef::NextT) {
      right_line_ref.set_wild_card();
      right_such_that_ref = new SuchThatRef(right_line_ref);
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

std::vector<std::string> SelectClauseParser::SplitTokensByEqualDelim(
    std::string input) {
  std::string delimiter = "=";
  std::vector<std::string> tokens;
  size_t pos;
  std::string token;
  while ((pos = input.find(delimiter)) != std::string::npos) {
    token = input.substr(0, pos);
    tokens.push_back(token);
    input.erase(0, pos + delimiter.length());
  }
  tokens.push_back(input);
  return tokens;
}

// splits by such that, pattern and with
std::tuple<std::string, std::vector<std::string>,
           std::vector<std::string>, std::vector<std::string>>
SelectClauseParser::SplitTokensByClauses(const std::string &input) {
  std::string SUCH_THAT_DELIM = "such that ";
  std::string PATTERN_DELIM = "pattern ";
  std::string WITH_DELIM = "with ";
  std::vector<std::string> delimiters{SUCH_THAT_DELIM, PATTERN_DELIM, WITH_DELIM};
  std::string select_clause;
  std::vector<std::string> such_that_clauses;
  std::vector<std::string> pattern_clauses;
  std::vector<std::string> with_clauses;

  size_t pos;
  std::string token;
  bool last_found_such_that = false;
  bool last_found_pattern = false;
  bool last_found_with = false;
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
      if (c == '(' && curr_ss.at(curr_ss.length() - 1) == ' ') {
        ss.str("");
        ss << curr_ss.substr(0, curr_ss.length() - 1);
      }
      ss << c;
      if (open_bracket_found) {  // additional open brackets found
        return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
      }
      open_bracket_found = true;
      whitespace_found = true;
    } else if (c == ')' || c == '>') {
      ss << c;
      if (!open_bracket_found) {  // additional close brackets found
        return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
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
          return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
        }
      }
    }
  }

  std::string clean_input = ss.str();
  pos = clean_input.find(' ');
  if (pos == std::string::npos) {  // find first whitespace
    return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
  }

  pos = clean_input.find(' ', pos + 1);
  if (pos == std::string::npos) {  // find second white space
    select_clause = clean_input;
    return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
  }

  select_clause = clean_input.substr(0, pos);  // adds the select clause
  clean_input.erase(0, pos);

  while (clean_input.find(SUCH_THAT_DELIM) != std::string::npos
      || clean_input.find(PATTERN_DELIM) != std::string::npos
      || clean_input.find(WITH_DELIM) != std::string::npos) {
    // find the earliest clause
    pos = std::min(std::min(clean_input.find(SUCH_THAT_DELIM),
                            clean_input.find(PATTERN_DELIM)),
                   clean_input.find(WITH_DELIM));
    token = clean_input.substr(0, pos);
    if (last_found_such_that) {
      such_that_clauses.push_back(token);
    } else if (last_found_pattern) {
      pattern_clauses.push_back(token);
    } else if (last_found_with) {
      with_clauses.push_back(token);
    }

    if (clean_input.substr(pos, SUCH_THAT_DELIM.length())
        == SUCH_THAT_DELIM) {
      clean_input.erase(0, pos + SUCH_THAT_DELIM.length());
      last_found_such_that = true;
      last_found_pattern = false;
      last_found_with = false;
    } else if (clean_input.substr(pos, PATTERN_DELIM.length())
        == PATTERN_DELIM) {
      clean_input.erase(0, pos + PATTERN_DELIM.length());
      last_found_pattern = true;
      last_found_such_that = false;
      last_found_with = false;
    } else if (clean_input.substr(pos, WITH_DELIM.length())
        == WITH_DELIM) {
      clean_input.erase(0, pos + WITH_DELIM.length());
      last_found_with = true;
      last_found_such_that = false;
      last_found_pattern = false;
    }
  }
  if (!last_found_such_that && !last_found_pattern && !last_found_with) {
    select_clause.append(clean_input);  // error
  }
  if (last_found_such_that) {
    such_that_clauses.push_back(clean_input);
  } else if (last_found_pattern) {
    pattern_clauses.push_back(clean_input);
  } else if (last_found_with) {
    with_clauses.push_back(clean_input);
  }

  return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
}

std::vector<std::vector<std::string>> SelectClauseParser::SplitTokensByBrackets(
    const std::string &input) {
  const std::string brackets = "(,)";
  const std::string AND_DELIM = " and ";
  std::vector<std::vector<std::string>> ret;
  std::vector<std::string> clauses = SplitTokensByDelimiter(input, AND_DELIM);
  for (const std::string& clause : clauses) {
    std::stringstream input_stream;
    input_stream << clause;
    std::vector<std::string> tokens;

    std::string line;
    while (getline(input_stream, line)) {
      size_t prev = 0, pos;
      while ((pos = line.find_first_of(brackets, prev))
          != std::string::npos) {
        if (pos >= prev)
          tokens.push_back(line.substr(prev, pos - prev));
        prev = pos + 1;
      }
      if (prev < line.length())
        if (line.substr(prev, std::string::npos) != "" &&
            line.substr(prev, std::string::npos) != " ")
          tokens.push_back(line.substr(prev, std::string::npos));
    }
    ret.push_back(tokens);
  }

  return ret;
}

std::vector<std::pair<std::string, std::string>> SelectClauseParser::SplitTokensByEqual(
    const std::string &input) {
  const std::string equal = "=";
  const std::string AND_DELIM = " and ";
  std::vector<std::pair<std::string, std::string>> ret;
  std::vector<std::string> clauses = SplitTokensByDelimiter(input, AND_DELIM);
  for (const std::string& clause : clauses) {
    std::vector<std::string> with_clause = SplitTokensByEqualDelim(clause);
    if (with_clause.size() != 2) {
      return {};
    }
    std::string left_ref = with_clause.at(0);
    std::string right_ref = with_clause.at(1);
    ret.push_back(std::make_pair(left_ref, right_ref));
  }

  return ret;
}
bool SelectClauseParser::IsValidAttr(const std::string &select) {
  std::vector<std::string> synonym_attribute = SplitTokensByDelimiter(select, ".");
  if (synonym_attribute.size() != 2) {
    return false;
  }
  std::string synonym = synonym_attribute.at(0);
  std::string attribute = synonym_attribute.at(1);

  EntityType type = synonym_to_entity_->find(synonym)->second->get_type();
  AttrValueType attr_value_type = AttrValueType::None;
  switch (type) {
    case EntityType::Procedure:
      if (attribute == "procName")
        attr_value_type = AttrValueType::Name;
      break;
    case EntityType::Variable:
      if (attribute == "varName")
        attr_value_type = AttrValueType::Name;
      break;
    case EntityType::Constant:
      if (attribute == "value")
        attr_value_type = AttrValueType::Integer;
      break;
    case EntityType::Call:
      if (attribute == "procName") {
        attr_value_type = AttrValueType::Name;
        break;
      } else if (attribute == "stmt#") {
        attr_value_type = AttrValueType::Integer;
        break;
      }
    case EntityType::Print:
    case EntityType::Read:
      if (attribute == "varName") attr_value_type = AttrValueType::Name;
      else if (attribute == "stmt#") attr_value_type = AttrValueType::Integer;
      break;
    case EntityType::Stmt:
    case EntityType::While:
    case EntityType::If:
    case EntityType::Assign:
      if (attribute == "stmt#") {
        attr_value_type = AttrValueType::Integer;
      }
      break;
    default:break;
  }

  if (attr_value_type != AttrValueType::None) {
    return true;
  } else {
    return false;
  }
}
