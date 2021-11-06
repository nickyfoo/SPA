#include "with_query_manager.h"

WithQueryManager::WithQueryManager() {
}

WithQueryManager::~WithQueryManager() = default;

ResultTable *WithQueryManager::EvaluateWith(std::shared_ptr<WithClause> with,
                                            std::unordered_map<std::string,
                                                               std::vector<Entity *>> synonym_to_entities_vec) {
  ResultTable *ret = new ResultTable();
  AttrValueType return_type = with->get_left_attr_value_type();
  std::vector<std::string> vec;
  if (with->get_left_type() == EntityType::None
      && with->get_right_type() == EntityType::None) {  // 12 = 12
    if (with->get_left_ref() != with->get_right_ref()) {
      return nullptr;
    } else {
      return ret;
    }
  } else if (with->get_left_type() == EntityType::None
      && with->get_right_type() != EntityType::None) {  // 12 = s.stmt#
    if (return_type == AttrValueType::Integer) {
      vec = GetIntegers(with->get_right_ref(), with->get_right_type(),
                        with->get_left_ref(), synonym_to_entities_vec);
      ret->AddSingleColumn(with->get_right_ref(), vec);
    } else if (return_type == AttrValueType::Name) {
      vec = GetNames(with->get_right_ref(), with->get_right_type(),
                     with->get_left_ref(), synonym_to_entities_vec);
      ret->AddSingleColumn(with->get_right_ref(), vec);
    } else {
      throw std::runtime_error("Unknown AttrValueType!");
    }
  } else if (with->get_left_type() != EntityType::None
      && with->get_right_type() == EntityType::None) {  // s.stmt# = 12
    if (return_type == AttrValueType::Integer) {
      vec = GetIntegers(with->get_left_ref(), with->get_left_type(),
                        with->get_right_ref(), synonym_to_entities_vec);
      ret->AddSingleColumn(with->get_left_ref(), vec);
    } else if (return_type == AttrValueType::Name) {
      vec = GetNames(with->get_left_ref(), with->get_left_type(),
                     with->get_right_ref(), synonym_to_entities_vec);
      ret->AddSingleColumn(with->get_left_ref(), vec);
    } else {
      throw std::runtime_error("Unknown AttrValueType!");
    }
  } else if (with->get_left_type() != EntityType::None &&
      with->get_right_type() != EntityType::None) {  // s.stmt# = a.stmt#
    std::vector<std::string> left_vec, right_vec;
    std::tie(left_vec, right_vec) = GetSynonymPairs(with, synonym_to_entities_vec);
    if (with->get_left_ref() == with->get_right_ref()) {
      ret->AddSingleColumn(with->get_left_ref(), left_vec);
    } else {
      ret->AddDoubleColumns(with->get_left_ref(), left_vec, with->get_right_ref(), right_vec);
    }
  }

  if (ret->get_table()->empty()) {
    return nullptr;
  }
  return ret;
}

// checks for matching name, but adds the default type to table
std::vector<std::string> WithQueryManager::GetNames(std::string synonym,
                                                    EntityType type,
                                                    std::string argument,
                                                    std::unordered_map<std::string,
                                                                       std::vector<Entity *>>
                                                    synonym_to_entities_vec) {
  std::vector<std::string> output;
  for (Entity *entity : synonym_to_entities_vec.at(synonym)) {
    switch (type) {
      case EntityType::Procedure: {
        auto *procedure = dynamic_cast<Procedure *>(entity);
        if (procedure->get_name() == argument) {
          output.push_back(procedure->get_name());
        }
        break;
      }
      case EntityType::Variable: {
        auto *variable = dynamic_cast<Variable *>(entity);
        if (variable->get_name() == argument) {
          output.push_back(variable->get_name());
        }
        break;
      }
      case EntityType::Call: {
        auto *stmt = dynamic_cast<Statement *>(entity);
        if (stmt->get_called_proc_name() == argument) {
          output.push_back(std::to_string(stmt->get_stmt_no()));
        }
        break;
      }
      case EntityType::Read: {
        auto *stmt = dynamic_cast<Statement *>(entity);
        for (auto &i : *stmt->get_modifies()) {
          if (i == argument) {
            output.push_back(std::to_string(stmt->get_stmt_no()));
          }
        }
        break;
      }
      case EntityType::Print: {
        auto *stmt = dynamic_cast<Statement *>(entity);
        for (auto &i : *stmt->get_uses()) {
          if (i == argument) {
            output.push_back(std::to_string(stmt->get_stmt_no()));
          }
        }
        break;
      }
      default:
        throw std::runtime_error("Wrong EntityType!");
    }
  }
  return output;
}

std::vector<std::string> WithQueryManager::GetIntegers(std::string synonym,
                                                       EntityType type,
                                                       std::string argument,
                                                       std::unordered_map<
                                                           std::string, std::vector<Entity *>>
                                                       synonym_to_entities_vec) {
  std::vector<std::string> output;
  for (Entity *entity : synonym_to_entities_vec.at(synonym)) {
    switch (type) {
      case EntityType::Constant: {
        auto *constant = dynamic_cast<Constant *>(entity);
        if (constant->get_value() == argument) {
          output.push_back(constant->get_value());
        }
        break;
      }
      case EntityType::Stmt:
      case EntityType::ProgLine:
      case EntityType::Call:
      case EntityType::Read:
      case EntityType::While:
      case EntityType::If:
      case EntityType::Assign:
      case EntityType::Print: {
        auto *stmt = dynamic_cast<Statement *>(entity);
        if (std::to_string(stmt->get_stmt_no()) == argument) {
          output.push_back(std::to_string(stmt->get_stmt_no()));
        }
        break;
      }
      default:
        throw std::runtime_error("Wrong EntityType!");
    }
  }
  return output;
}

std::tuple<std::vector<std::string>, std::vector<std::string>> WithQueryManager::GetSynonymPairs(
    std::shared_ptr<WithClause> with,
    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  std::vector<std::string> left_synonym;
  std::vector<std::string> right_synonym;
  if (with->get_left_attr_value_type() == AttrValueType::Integer) {
    for (Entity *right_entity : synonym_to_entities_vec.at(with->get_right_ref())) {
      EntityType type = with->get_right_type();
      std::vector<std::string> curr_vec;
      std::string right_arg;
      switch (type) {
        case EntityType::Constant: {
          auto *constant = dynamic_cast<Constant *>(right_entity);
          right_arg = constant->get_value();
          curr_vec = GetIntegers(with->get_left_ref(), with->get_left_type(), right_arg,
                                 synonym_to_entities_vec);
          break;
        }
        case EntityType::Stmt:
        case EntityType::ProgLine:
        case EntityType::Call:
        case EntityType::Read:
        case EntityType::While:
        case EntityType::If:
        case EntityType::Assign:
        case EntityType::Print: {
          auto *stmt = dynamic_cast<Statement *>(right_entity);
          right_arg = std::to_string(stmt->get_stmt_no());
          curr_vec = GetIntegers(with->get_left_ref(), with->get_left_type(), right_arg,
                                 synonym_to_entities_vec);
          break;
        }
        default:
          throw std::runtime_error("Wrong EntityType!");
      }
      for (std::string s : curr_vec) {
        left_synonym.push_back(s);
        right_synonym.push_back(right_arg);
      }
    }
  } else if (with->get_left_attr_value_type() == AttrValueType::Name) {
    for (Entity *right_entity : synonym_to_entities_vec.at(with->get_right_ref())) {
      EntityType type = with->get_right_type();
      std::vector<std::string> curr_vec;
      std::string right_arg;
      switch (type) {
        case EntityType::Procedure: {
          auto *procedure = dynamic_cast<Procedure *>(right_entity);
          right_arg = procedure->get_name();
          curr_vec = GetNames(with->get_left_ref(), with->get_left_type(), right_arg,
                              synonym_to_entities_vec);
          break;
        }
        case EntityType::Variable: {
          auto *variable = dynamic_cast<Variable *>(right_entity);
          right_arg = variable->get_name();
          curr_vec = GetNames(with->get_left_ref(), with->get_left_type(), right_arg,
                              synonym_to_entities_vec);
          break;
        }
        case EntityType::Call: {
          auto *stmt = dynamic_cast<Statement *>(right_entity);
          right_arg = stmt->get_called_proc_name();
          curr_vec = GetNames(with->get_left_ref(), with->get_left_type(), right_arg,
                              synonym_to_entities_vec);
          right_arg = std::to_string(stmt->get_stmt_no());
          break;
        }
        case EntityType::Read: {
          auto *stmt = dynamic_cast<Statement *>(right_entity);
          for (auto &i : *stmt->get_modifies()) {
            curr_vec = GetNames(with->get_left_ref(), with->get_left_type(), i,
                                synonym_to_entities_vec);
            right_arg = std::to_string(stmt->get_stmt_no());
          }
          break;
        }
        case EntityType::Print: {
          auto *stmt = dynamic_cast<Statement *>(right_entity);
          for (auto &i : *stmt->get_uses()) {
            curr_vec = GetNames(with->get_left_ref(), with->get_left_type(), i,
                                synonym_to_entities_vec);
            right_arg = std::to_string(stmt->get_stmt_no());
          }
          break;
        }
        default:
          throw std::runtime_error("Wrong EntityType!");
      }
      for (std::string s : curr_vec) {
        left_synonym.push_back(s);
        right_synonym.push_back(right_arg);
      }
    }
  }
  return std::make_tuple(left_synonym, right_synonym);
}
