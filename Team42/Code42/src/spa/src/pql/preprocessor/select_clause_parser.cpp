#include <iostream>
#include <tuple>
#include <utility>

#include "line_ref.h"
#include "pattern_clause.h"
#include "result_clause.h"
#include "select_clause_parser.h"
#include "such_that_clause.h"
#include "with_clause.h"

SelectClauseParser::SelectClauseParser() { this->synonym_to_entity_ = nullptr; }

SelectClauseParser *SelectClauseParser::instance = nullptr;

SelectClauseParser *SelectClauseParser::get_instance() {
  if (!instance) instance = new SelectClauseParser;
  return instance;
}

void SelectClauseParser::set_select_clause(
    std::unordered_map<std::string, EntityDeclaration *> *syn_to_ent, std::string select_clause) {
  this->synonym_to_entity_ = syn_to_ent;
  select_clause.erase(remove(select_clause.begin(), select_clause.end(), '\n'),
                      select_clause.end());
  this->select_statement_ = select_clause;
}

// returns nullptr if any clauses are invalid
// returns a tuple with is_valid_clause = false if BOOLEAN operation
std::tuple<std::vector<ResultClause *> *, std::vector<SuchThatClause *> *,
           std::vector<PatternClause *> *, std::vector<WithClause *> *,
           std::unordered_map<std::string, EntityDeclaration *> *,
           bool, bool> *
    SelectClauseParser::get_clauses() {
  std::tuple<std::string, std::vector<std::string>, std::vector<std::string>,
             std::vector<std::string>>
      clauses;
  clauses = SplitClauses(select_statement_);
  std::string select_clause = std::get<0>(clauses);
  std::vector<std::string> such_that_clauses = std::get<1>(clauses);
  std::vector<std::string> pattern_clauses = std::get<2>(clauses);
  std::vector<std::string> with_clauses = std::get<3>(clauses);
  auto *select_ret = new std::vector<ResultClause *>();
  auto *such_that_ret = new std::vector<SuchThatClause *>();
  auto *pattern_ret = new std::vector<PatternClause *>();
  auto *with_ret = new std::vector<WithClause *>();
  bool syntactically_valid = true;
  bool semantically_valid = true;
  std::vector<std::string> select_clauses = SplitSelect(select_clause);
  auto *syntactically_false_res = new std::tuple<std::vector<ResultClause *> *, std::vector<SuchThatClause *> *,
                             std::vector<PatternClause *> *, std::vector<WithClause *> *,
                             std::unordered_map<std::string, EntityDeclaration *> *, bool, bool>(
      select_ret, such_that_ret, pattern_ret, with_ret, synonym_to_entity_,
      false, false);

  if (select_clauses.empty()) {  // invalid select syntax
    return syntactically_false_res;
  }

  for (const std::string &select : select_clauses) {
    ResultClause *result_clause;
    if (select == "BOOLEAN" && select_clauses.size() == 1
        && synonym_to_entity_->find("BOOLEAN") == synonym_to_entity_->end()) {
      result_clause = new ResultClause("", EntityType::None, ReturnType::Boolean);
    } else if (synonym_to_entity_->find(select) != synonym_to_entity_->end()) {
      result_clause =
          new ResultClause(select, synonym_to_entity_->at(select)->get_type(), ReturnType::Default);
    } else if (IsValidIdentifier(select)) {
      semantically_valid = false;
    } else {
      result_clause = ValidateResultClauseWithAttr(select);
    }
    if (semantically_valid) {
      if (result_clause == nullptr || !result_clause->set_elem()) {
        return syntactically_false_res;
      } else {
        select_ret->push_back(result_clause);
      }
    }
  }
  for (const std::string &such_that_clause : such_that_clauses) {
    std::vector<SuchThatClause *> *relationship = MakeSuchThatClause(such_that_clause);
    if (relationship == nullptr) {
      return syntactically_false_res;
    } else if (relationship->empty()) {
      semantically_valid = false;
    }
    for (SuchThatClause *clause : *relationship) {
      such_that_ret->push_back(clause);
    }
  }

  for (const std::string &pattern_clause : pattern_clauses) {
    std::vector<PatternClause *> *pattern = MakePatternClause(pattern_clause);
    if (pattern == nullptr) {
      return syntactically_false_res;
    } else if (pattern->empty()) {
      semantically_valid = false;
    }
    for (PatternClause *clause : *pattern) {
      pattern_ret->push_back(clause);
    }
  }

  for (const std::string &with_clause : with_clauses) {
    std::vector<WithClause *> *with = MakeWithClause(with_clause);
    if (with == nullptr) {
      return syntactically_false_res;
    } else if (with->empty()) {
      semantically_valid = false;
    }
    for (WithClause *clause : *with) {
      with_ret->push_back(clause);
    }
  }
  auto *res = new std::tuple<std::vector<ResultClause *> *, std::vector<SuchThatClause *> *,
                             std::vector<PatternClause *> *, std::vector<WithClause *> *,
                             std::unordered_map<std::string, EntityDeclaration *> *, bool, bool>(
      select_ret, such_that_ret, pattern_ret, with_ret, synonym_to_entity_,
      syntactically_valid, semantically_valid);
  return res;
}

// returns nullptr if syntactically invalid,
// empty vector if semantically invalid
std::vector<SuchThatClause *> *SelectClauseParser::MakeSuchThatClause(
    const std::string &relationship_statement) {
  auto *ret = new std::vector<SuchThatClause *>();
  if (relationship_statement.empty()) {
    return nullptr;
  }
  std::vector<std::vector<std::string>> relationship_clauses;
  relationship_clauses = SplitBrackets(relationship_statement);
  if (relationship_clauses.empty()) return nullptr;
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
    SuchThatRef *left_such_that_ref = new SuchThatRef();
    SuchThatRef *right_such_that_ref = new SuchThatRef();
    int valid_left = SetSuchThatRefLeft(left_such_that_ref, relationship, left_ref);
    int valid_right = SetSuchThatRefRight(right_such_that_ref, relationship, right_ref);
    if (valid_left == -1 || valid_right == -1) {  // syntactically invalid
      return nullptr;
    }

    if (relationship->set_ref(left_such_that_ref, right_such_that_ref) &&
    valid_left == 1 && valid_right == 1) {
      ret->push_back(relationship);
    } else {  // semantically invalid
      return new std::vector<SuchThatClause *>();;
    }
  }

  return ret;
}

// returns nullptr if syntactically invalid,
// empty vector if semantically invalid
std::vector<PatternClause *> *SelectClauseParser::MakePatternClause(
    const std::string &pattern_statement) {
  auto *ret = new std::vector<PatternClause *>();
  if (pattern_statement.empty()) return nullptr;
  std::vector<std::vector<std::string>> pattern_clauses = SplitBrackets(pattern_statement);
  if (pattern_clauses.empty()) return nullptr;
  for (std::vector<std::string> pattern_clause : pattern_clauses) {
    if (pattern_clause.size() < 3 || pattern_clause.size() > 4) return nullptr;
    std::string synonym = pattern_clause.at(0);
    std::string left_ref = pattern_clause.at(1);
    std::string right_ref = pattern_clause.at(2);
    if (synonym.empty() || left_ref.empty() || right_ref.empty()) return nullptr;
    PatternClause *pattern;
    if (synonym_to_entity_->find(synonym) == synonym_to_entity_->end()) {
      return new std::vector<PatternClause *>();;
    } else if ((synonym_to_entity_->at(synonym)->get_type() == EntityType::Assign ||
            synonym_to_entity_->at(synonym)->get_type() == EntityType::If ||
            synonym_to_entity_->at(synonym)->get_type() == EntityType::While)) {
      pattern = new PatternClause(synonym_to_entity_->find(synonym)->second);
    } else {
      return nullptr;
    }
    if (pattern_clause.size() == 3) {  // assign and while
      int valid = SetPatternRef(pattern, left_ref, right_ref);
      if (!(pattern->get_type() == EntityType::Assign ||
          pattern->get_type() == EntityType::While) || valid == -1) {
        return nullptr;
      } else if (valid == 0) {
        return new std::vector<PatternClause *>();;
      } else {
        ret->push_back(pattern);
      }
    } else {  // if
      std::string last_ref = pattern_clause.at(3);
      int valid = SetPatternRef(pattern, left_ref, right_ref);
      if (last_ref != "_" || pattern->get_type() != EntityType::If || valid == -1) {
        return nullptr;
      } else if (valid == 0) {
        return new std::vector<PatternClause *>();;
      } else {
        ret->push_back(pattern);
      }
    }
  }
  return ret;
}

std::vector<WithClause *> *SelectClauseParser::MakeWithClause(const std::string &with_statement) {
  auto *ret = new std::vector<WithClause *>();
  if (with_statement.empty()) return nullptr;
  std::vector<std::pair<std::string, std::string>> with_clauses =
      SplitWith(with_statement);
  if (with_clauses.empty()) return nullptr;
  for (const std::pair<std::string, std::string> &with_clause : with_clauses) {
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

    WithClause* with = new WithClause();
    int valid = SetWithRef(with, left_ref, right_ref);

    if (valid == -1) {
      return nullptr;
    } else if (valid == 0) {
      return new std::vector<WithClause *>();
    } else {
      ret->push_back(with);
    }
  }
  return ret;
}

// 1 to represent valid
// 0 to represent semantically invalid
// -1 to represent syntactically invalid
int SelectClauseParser::SetPatternRef(PatternClause *pattern,
                                                 const std::string &left_ref,
                                                 const std::string &right_ref) {
//  PatternClause *ret;
  auto *ent_ref = new EntRef();
  if (synonym_to_entity_->find(left_ref) != synonym_to_entity_->end()) {
    if (synonym_to_entity_->at(left_ref)->get_type() == EntityType::Variable) {
      ent_ref->set_synonym(left_ref);
    } else {
      return 0;
    }
  } else if (left_ref == "_") {
    ent_ref->set_wild_card();
  } else if (left_ref.length() > 2 && left_ref[0] == '\"' && left_ref[left_ref.length() - 1] == '\"'
  && IsValidIdentifier(left_ref.substr(1, left_ref.length() - 2))) {
    ent_ref->set_argument(left_ref.substr(1, left_ref.length() - 2));
  } else {
    return -1;
  }
  if (pattern->set_ref(ent_ref, right_ref)) {
    return 1;
  } else {
    return -1;
  }
}

// 1 to represent valid
// 0 to represent semantically invalid
// -1 to represent syntactically invalid
int SelectClauseParser::SetWithRef(WithClause *with, const std::string &left_ref,
                                   const std::string &right_ref) {
  std::string left_str;
  std::string right_str;
  EntityType left_type;
  EntityType right_type;
  AttrValueType left_attr_value_type;
  AttrValueType right_attr_value_type;
  printf("one\n");
  // set as EntityType::None if integer or ident
  if (IsInteger(left_ref)) {
    left_str = left_ref;
    left_type = EntityType::None;
    left_attr_value_type = AttrValueType::Integer;
  } else if (left_ref.length() > 2 && left_ref[0] == '\"'
      && left_ref[left_ref.length() - 1] == '\"'
      && IsValidIdentifier(left_ref.substr(1, left_ref.length() - 2))) {
    left_str = left_ref.substr(1, left_ref.length() - 2);
    left_type = EntityType::None;
    left_attr_value_type = AttrValueType::Name;
  } else {
    std::tie(left_str, left_type, left_attr_value_type) =
        GetWithRefTypeAndAttrValueType(left_ref);
    if (left_type == EntityType::None) {
      if (left_str == "") return -1;
      else return 0;
    }
  }
  printf("two\n");
  // set as EntityType::None if integer or ident
  if (IsInteger(right_ref)) {
    right_str = right_ref;
    right_type = EntityType::None;
    right_attr_value_type = AttrValueType::Integer;
  } else if (right_ref.length() > 2 && right_ref[0] == '\"'
      && right_ref[right_ref.length() - 1] == '\"'
      && IsValidIdentifier(right_ref.substr(1, right_ref.length() - 2))) {
    right_str = right_ref.substr(1, right_ref.length() - 2);
    right_type = EntityType::None;
    right_attr_value_type = AttrValueType::Name;
  } else {
    std::tie(right_str, right_type, right_attr_value_type) =
        GetWithRefTypeAndAttrValueType(right_ref);
    if (right_type == EntityType::None) {
      if (right_str == "") return -1;
      else return 0;
    }
  }
  printf("three\n");
  // not previously declared
  if ((left_type != EntityType::None &&
      synonym_to_entity_->find(left_str) == synonym_to_entity_->end()) ||
      (right_type != EntityType::None &&
      synonym_to_entity_->find(right_str) == synonym_to_entity_->end())) {
    return 0;
  }
  printf("four\n");
  // check same type
  if (left_attr_value_type != right_attr_value_type) {  // must be same attribute value types
    return 0;
  }
  printf("five\n");
  with->set_values(left_str, left_type, left_attr_value_type, right_str, right_type,
                             right_attr_value_type);
  return 1;
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
    } else {
      return std::make_tuple(synonym, EntityType::None, AttrValueType::None);
    }
  } else if (synonym_attribute.size() == 2) {
    std::string synonym = synonym_attribute.at(0);
    std::string attribute = synonym_attribute.at(1);
    if (synonym_to_entity_->find(synonym) != synonym_to_entity_->end()) {
      EntityType type = synonym_to_entity_->find(synonym)->second->get_type();
      AttrValueType attr_value_type = AttrValueType::None;
      switch (type) {
        case EntityType::Procedure:
          if (attribute == "procName") attr_value_type = AttrValueType::Name;
          break;
        case EntityType::Variable:
          if (attribute == "varName") attr_value_type = AttrValueType::Name;
          break;
        case EntityType::Constant:
          if (attribute == "value") attr_value_type = AttrValueType::Integer;
          break;
        case EntityType::Call:
          if (attribute == "procName")
            attr_value_type = AttrValueType::Name;
          else if (attribute == "stmt#")
            attr_value_type = AttrValueType::Integer;
          break;
        case EntityType::Print:
        case EntityType::Read:
          if (attribute == "varName")
            attr_value_type = AttrValueType::Name;
          else if (attribute == "stmt#")
            attr_value_type = AttrValueType::Integer;
          break;
        case EntityType::Stmt:
        case EntityType::While:
        case EntityType::If:
        case EntityType::Assign:
          if (attribute == "stmt#")
            attr_value_type = AttrValueType::Integer;
          break;
        default:break;
      }
      printf("here\n");
      return std::make_tuple(synonym, type, attr_value_type);
    }
    if (attribute == "procName" || attribute == "stmt#"
        || attribute == "value" || attribute == "varName") {  // correct semantics
      return std::make_tuple(synonym, EntityType::None, AttrValueType::None);
    }
  }
  return std::make_tuple("", EntityType::None, AttrValueType::None);  // not a valid with clause
}

// 1 to represent valid
// 0 to represent semantically invalid
// -1 to represent syntactically invalid
int SelectClauseParser::SetSuchThatRefLeft(SuchThatRef *left_such_that_ref,
                                           SuchThatClause *relationship,
                                           std::string left_ref) {
  StmtRef left_stmt_ref;
  EntRef left_ent_ref;
  LineRef left_line_ref;
  RelRef type = relationship->get_type();

  // existing synonym_
  if (synonym_to_entity_->find(left_ref) != synonym_to_entity_->end()) {
    EntityType entity_type = synonym_to_entity_->at(left_ref)->get_type();
    switch (entity_type) {
      case EntityType::Call:
      case EntityType::If:
      case EntityType::Print:
        if (type == RelRef::Modifies && entity_type == EntityType::Print) {
          return 0;
        }
      case EntityType::Read:
        if (type == RelRef::Uses && entity_type == EntityType::Read) {
          return 0;
        }
      case EntityType::Assign:
      case EntityType::Stmt:
      case EntityType::ProgLine:
        if ((entity_type == EntityType::Stmt || entity_type == EntityType::ProgLine
            || entity_type == EntityType::Assign) &&
            (type == RelRef::Affects || type == RelRef::AffectsT ||
                type == RelRef::AffectsBip || type == RelRef::AffectsTBip)) {
          left_stmt_ref.set_synonym(left_ref);
          left_stmt_ref.set_entity_type(entity_type);
          left_such_that_ref->set_stmt_ref(left_stmt_ref);
          return 1;
        }
      case EntityType::While: {
        if (type == RelRef::Next || type == RelRef::NextT ||
            type == RelRef::NextBip || type == RelRef::NextTBip) {
          left_line_ref.set_synonym(left_ref);
          left_line_ref.set_entity_type(entity_type);
          left_such_that_ref->set_line_ref(left_line_ref);
          return 1;
        } else if (type != RelRef::Calls && type != RelRef::CallsT) {
          left_stmt_ref.set_synonym(left_ref);
          left_stmt_ref.set_entity_type(entity_type);
          left_such_that_ref->set_stmt_ref(left_stmt_ref);
          return 1;
        } else {
          return 0;
        }
      }
      case EntityType::Procedure:
        if (type == RelRef::Uses || type == RelRef::Modifies ||
            type == RelRef::Calls || type == RelRef::CallsT) {
          left_ent_ref.set_synonym(left_ref);
          left_such_that_ref->set_ent_ref(left_ent_ref);
          return 1;
        }
      case EntityType::Variable:
      case EntityType::Constant:
      default:
        return 0;
    }
  } else if (IsInteger(left_ref)) {  // statement number
    int left_ref_int = std::stoi(left_ref);
    if (left_ref_int < 1) {  // invalid statement number
      return 0;
    }
    if (type == RelRef::Next || type == RelRef::NextT ||
        type == RelRef::NextBip || type == RelRef::NextTBip) {
      left_line_ref.set_line_num(std::stoi(left_ref));
      left_such_that_ref->set_line_ref(left_line_ref);
    } else {
      left_stmt_ref.set_stmt_num(std::stoi(left_ref));
      left_such_that_ref->set_stmt_ref(left_stmt_ref);
    }
  } else if (left_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies || type == RelRef::Calls ||
        type == RelRef::CallsT) {
      left_ent_ref.set_wild_card();
      left_such_that_ref->set_ent_ref(left_ent_ref);
    } else if (type == RelRef::Next || type == RelRef::NextT ||
              type == RelRef::NextBip || type == RelRef::NextTBip) {
      left_line_ref.set_wild_card();
      left_such_that_ref->set_line_ref(left_line_ref);
    } else {
      left_stmt_ref.set_wild_card();
      left_such_that_ref->set_stmt_ref(left_stmt_ref);
    }
  } else if (left_ref.length() > 2 && left_ref[0] == '\"'
      && left_ref[left_ref.length() - 1] == '\"'
      && IsValidIdentifier(left_ref.substr(1, left_ref.length() - 1))) {
    left_ent_ref.set_argument(left_ref.substr(1, left_ref.length() - 2));
    left_such_that_ref->set_ent_ref(left_ent_ref);
  } else if (IsValidIdentifier(left_ref)) {
    return 0;
  } else {
    return -1;
  }

  if (left_such_that_ref->get_type() != SuchThatRefType::None) {  // invalid syntax
    return 1;
  }
  return -1;
}

// 1 to represent valid
// 0 to represent semantically invalid
// -1 to represent syntactically invalid
int SelectClauseParser::SetSuchThatRefRight(SuchThatRef *right_such_that_ref,
                                             SuchThatClause *relationship,
                                             std::string right_ref) {
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
          right_such_that_ref->set_ent_ref(right_ent_ref);
          return 1;
        } else {
          return 0;
        }
      }
      case EntityType::Call:
      case EntityType::If:
      case EntityType::Print:
      case EntityType::Read:
      case EntityType::Assign:
      case EntityType::Stmt:
      case EntityType::ProgLine:
        if ((entity_type == EntityType::Stmt || entity_type == EntityType::ProgLine
            || entity_type == EntityType::Assign) &&
            (type == RelRef::Affects || type == RelRef::AffectsT ||
                type == RelRef::AffectsBip || type == RelRef::AffectsTBip)) {
          right_stmt_ref.set_synonym(right_ref);
          right_stmt_ref.set_entity_type(entity_type);
          right_such_that_ref->set_stmt_ref(right_stmt_ref);
          return 1;
        }
      case EntityType::While: {
        if (type == RelRef::Next || type == RelRef::NextT ||
            type == RelRef::NextBip || type == RelRef::NextTBip) {
          right_line_ref.set_synonym(right_ref);
          right_line_ref.set_entity_type(entity_type);
          right_such_that_ref->set_line_ref(right_line_ref);
          return 1;
        } else if (type != RelRef::Affects && type != RelRef::AffectsT &&
            type != RelRef::AffectsBip && type != RelRef::AffectsTBip &&
            type != RelRef::Calls && type != RelRef::CallsT) {
          right_stmt_ref.set_synonym(right_ref);
          right_stmt_ref.set_entity_type(entity_type);
          right_such_that_ref->set_stmt_ref(right_stmt_ref);
          return 1;
        } else {
          return 0;
        }
      }
      case EntityType::Procedure:
        if (type == RelRef::Calls || type == RelRef::CallsT) {
          right_ent_ref.set_synonym(right_ref);
          right_such_that_ref->set_ent_ref(right_ent_ref);
          break;
        }
      case EntityType::Constant:
      default:
        return 0;
    }
  } else if (IsInteger(right_ref)) {  // statement number
    int right_ref_int = std::stoi(right_ref);
    if (right_ref_int < 1) {  // invalid statement number
      return 0;
    }
    if (type == RelRef::Next || type == RelRef::NextT ||
        type == RelRef::NextBip || type == RelRef::NextTBip) {
      right_line_ref.set_line_num(right_ref_int);
      right_such_that_ref->set_line_ref(right_line_ref);
    } else {
      right_stmt_ref.set_stmt_num(right_ref_int);
      right_such_that_ref->set_stmt_ref(right_stmt_ref);
    }
  } else if (right_ref == "_") {  // wild card
    if (type == RelRef::Uses || type == RelRef::Modifies || type == RelRef::Calls ||
        type == RelRef::CallsT) {
      right_ent_ref.set_wild_card();
      right_such_that_ref->set_ent_ref(right_ent_ref);
    } else if (type == RelRef::Next || type == RelRef::NextT |
        type == RelRef::NextBip || type == RelRef::NextTBip) {
      right_line_ref.set_wild_card();
      right_such_that_ref->set_line_ref(right_line_ref);
    } else {
      right_stmt_ref.set_wild_card();
      right_such_that_ref->set_stmt_ref(right_stmt_ref);
    }
  } else if (right_ref.length() > 2 && right_ref[0] == '\"'
      && right_ref[right_ref.length() - 1] == '\"'
      && IsValidIdentifier(right_ref.substr(1, right_ref.length() - 2))) {
    right_ent_ref.set_argument(right_ref.substr(1,  right_ref.length() - 2));
    right_such_that_ref->set_ent_ref(right_ent_ref);
  } else if (IsValidIdentifier(right_ref)) {
    return 0;
  } else {
    return -1;
  }

  if (right_such_that_ref->get_type() != SuchThatRefType::None) {  // invalid syntax
    return 1;
  }
  return -1;
}

// Function that returns true if str is a valid identifier
bool SelectClauseParser::IsValidIdentifier(const std::string &str) {

  // If first character is invalid
  if (!((str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z'))) {
    return false;
  }

  // Traverse the string for the rest of the characters
  for (int i = 1; i < str.length() - 1; i++) {
    if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') ||
          (str[i] >= '0' && str[i] <= '9'))) {
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

std::vector<std::string> SelectClauseParser::SplitSelect(std::string select_clause) {
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
    found_select = true;
    select_clause.erase(0, pos + SELECT_DELIM.length());
    break;
  }

  if (!found_select) return {};
  select_clause.erase(std::remove_if(select_clause.begin(), select_clause.end(), ::isspace),
                      select_clause.end());

  if (select_clause.empty()) {
    return {};
  } else if (select_clause.at(0) == '<' && select_clause.at(select_clause.length() - 1) == '>') {
    return SplitTokensByDelimiter(select_clause.substr(1, select_clause.length() - 2), ",");
  } else {
    return std::vector<std::string>{select_clause};
  }
}

std::vector<std::string> SelectClauseParser::SplitTokensByDelimiter(std::string input,
                                                                    const std::string &delimiter) {
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

std::vector<std::string> SelectClauseParser::SplitTokensByEqualDelim(std::string input) {
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
std::tuple<std::string, std::vector<std::string>, std::vector<std::string>,
           std::vector<std::string>>
SelectClauseParser::SplitClauses(const std::string &input) {
  std::string select_clause;
  std::vector<std::string> such_that_clauses;
  std::vector<std::string> pattern_clauses;
  std::vector<std::string> with_clauses;
  auto false_res = make_tuple("", such_that_clauses, pattern_clauses, with_clauses);

  size_t pos;
  std::string token;
  std::stringstream ss;
  const std::string WHITESPACE = " \n\r\t\f\v";

  // first pass to remove all whitespaces within brackets and before brackets,
  // and trim whitespaces after open commas and before close commas
  bool whitespace_found = true;
  bool open_bracket_found = false;
  bool inverted_commas_found = false;
  std::stringstream prev_word_stream;
  std::stringstream argument_word_stream;
  for (char c : input) {
    if (c == '\"' || c == '\'') {
      if (inverted_commas_found) {  // closing comma encountered
        std::string arg = argument_word_stream.str();
        auto arg_begin = arg.find_first_not_of(WHITESPACE);
        if (arg_begin == std::string::npos) return false_res;

        auto arg_end = arg.find_last_not_of(WHITESPACE);
        std::string trimmed_arg = arg.substr(arg_begin, arg_end - arg_begin + 1);
        ss << "\"" << trimmed_arg << "\"";
        argument_word_stream.str("");
      }
      inverted_commas_found = !inverted_commas_found;
      whitespace_found = false;
    } else if (inverted_commas_found) {
      argument_word_stream << c;
//      ss << c;
    } else if (isspace(c)) {
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
        return false_res;
      }
      open_bracket_found = true;
      whitespace_found = true;
    } else if (c == ')' || c == '>') {
      ss << c;
      if (!open_bracket_found) {  // additional close brackets found
        return false_res;
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
          return false_res;
        }
      }
    }
  }
  if (inverted_commas_found) return false_res;

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

  // second pass to split into clauses
  inverted_commas_found = false;
  bool last_found_such_that = false;
  bool last_found_pattern = false;
  bool last_found_with = false;
  prev_word_stream.str("");
  ss.str("");
  for (char c : clean_input) {
    if (c == '\"' || c == '\'') {
      ss << prev_word_stream.str();
      ss << c;
      prev_word_stream.str("");
      inverted_commas_found = !inverted_commas_found;
    } else if (inverted_commas_found) {
      ss << c;
    } else if (isspace(c)) {
      if (prev_word_stream.str() == "such") {
        prev_word_stream << " ";
      } else if (prev_word_stream.str() == "such that" || prev_word_stream.str() == "pattern"
          || prev_word_stream.str() == "with" || prev_word_stream.str() == "and") {
        if (last_found_such_that) {
          such_that_clauses.push_back(ss.str());
        } else if (last_found_pattern) {
          pattern_clauses.push_back(ss.str());
        } else if (last_found_with) {
          with_clauses.push_back(ss.str());
        } else if (prev_word_stream.str() == "and") {  // and seen before any clause keywords
          return false_res;
        } else if (ss.str() != " ") { // if there is a term before the first clause keyword, error
          return false_res;
        }

        if (prev_word_stream.str() != "and") {
          last_found_such_that = false;
          last_found_pattern = false;
          last_found_with = false;

          if (prev_word_stream.str() == "such that") last_found_such_that = true;
          else if (prev_word_stream.str() == "pattern") last_found_pattern = true;
          else if (prev_word_stream.str() == "with") last_found_with = true;
        }

        prev_word_stream.str("");
        ss.str("");
      } else {
        ss << prev_word_stream.str();
        ss << " ";
        prev_word_stream.str("");
      }
    } else {
      prev_word_stream << c;
    }
  }

  ss << prev_word_stream.str();
  if (last_found_such_that) such_that_clauses.push_back(ss.str());
  else if (last_found_pattern) pattern_clauses.push_back(ss.str());
  else if (last_found_with) with_clauses.push_back(ss.str());
  else select_clause.append(clean_input);

  return make_tuple(select_clause, such_that_clauses, pattern_clauses, with_clauses);
}

std::vector<std::vector<std::string>> SelectClauseParser::SplitBrackets(
    const std::string &input) {
  const std::string brackets = "(,)";
  const std::string AND_DELIM = " and ";
  std::vector<std::vector<std::string>> ret;
  std::vector<std::string> clauses = SplitTokensByDelimiter(input, AND_DELIM);
  for (const std::string &clause : clauses) {
    if (clause == "") return {};
    std::vector<std::string> tokens;
    std::stringstream ss;

    bool inverted_commas_found = false;
    bool open_bracket_found = false;
    bool close_bracket_found = false;
    for (char c : clause) {
      if (c == '\"' || c == '\'') {
        ss << c;
        inverted_commas_found = !inverted_commas_found;
      } else if (inverted_commas_found) {
        ss << c;
      } else if (c == '(' || c == '<') {
        if (open_bracket_found) return {};  // error, found extra
        open_bracket_found = true;
        tokens.push_back(ss.str());
        ss.str("");
      } else if (c == ')' || c == '>') {
        if (close_bracket_found) return {};  // error, found extra
        close_bracket_found = true;
        tokens.push_back(ss.str());
        ss.str("");
      } else if (c == ',') {
        tokens.push_back(ss.str());
        ss.str("");
      } else if (!isspace(c)) {
        ss << c;
      }
    }
    if (ss.str() != "") {
      tokens.push_back(ss.str());
    }
    ret.push_back(tokens);
  }

  return ret;
}

std::vector<std::pair<std::string, std::string>> SelectClauseParser::SplitWith(
    const std::string &input) {
  const std::string equal = "=";
  const std::string AND_DELIM = " and ";
  std::vector<std::pair<std::string, std::string>> ret;
  std::vector<std::string> clauses = SplitTokensByDelimiter(input, AND_DELIM);
  for (const std::string &clause : clauses) {
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

ResultClause *SelectClauseParser::ValidateResultClauseWithAttr(const std::string &select) {
  std::vector<std::string> synonym_attribute = SplitTokensByDelimiter(select, ".");
  if (synonym_attribute.size() != 2) {
    return nullptr;
  }
  std::string synonym = synonym_attribute.at(0);
  std::string attribute = synonym_attribute.at(1);
  if (synonym_to_entity_->find(synonym) == synonym_to_entity_->end()) {
    return nullptr;
  }
  EntityType synonym_type = synonym_to_entity_->find(synonym)->second->get_type();
  ReturnType return_type;
  switch (synonym_type) {
    case EntityType::Procedure:if (attribute == "procName") return_type = ReturnType::Name;
      break;
    case EntityType::Variable:if (attribute == "varName") return_type = ReturnType::Name;
      break;
    case EntityType::Constant:if (attribute == "value") return_type = ReturnType::Integer;
      break;
    case EntityType::Call:
      if (attribute == "procName") {
        return_type = ReturnType::Name;
        break;
      } else if (attribute == "stmt#") {
        return_type = ReturnType::Integer;
        break;
      }
    case EntityType::Print:
    case EntityType::Read:
      if (attribute == "varName") {
        return_type = ReturnType::Name;
        break;
      } else if (attribute == "stmt#") {
        return_type = ReturnType::Integer;
        break;
      }
    case EntityType::Stmt:
    case EntityType::While:
    case EntityType::If:
    case EntityType::Assign:
      if (attribute == "stmt#") {
        return_type = ReturnType::Integer;
        break;
      }
    default:return nullptr;
  }

  return new ResultClause(synonym, synonym_type, return_type);
}