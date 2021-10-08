#include <sstream>
#include "query_evaluator.h"
#include "ast.h"  // Get kind enum
#include "statement.h"
#include "procedure.h"
#include "variable.h"
#include "pattern_query_manager.h"
#include "relationship_query_manager.h"
#include "with_query_manager.h"
#include "pql_query.h"
#include "pkb.h"

QueryEvaluator::QueryEvaluator(PQLQuery *pql_query, PKB *pkb) {
  if (pql_query != nullptr) {
    this->entities_to_return_ = pql_query->get_query_entities();
    this->clause_groups_ = pql_query->get_clause_groups();
    this->synonym_to_entity_dec_ = pql_query->get_synonym_to_entities();
    this->pkb_ = pkb;
  } else {
    this->entities_to_return_ = nullptr;
    this->synonym_to_entity_dec_ = nullptr;
    this->pkb_ = nullptr;
  }
}

std::vector<std::string> *QueryEvaluator::Evaluate() {
  if (synonym_to_entity_dec_ == nullptr) {
    return new std::vector<std::string>{};
  }
  printf("EVALUATE\n");
  RelationshipQueryManager *relationship_query_manager;
  PatternQueryManager *pattern_query_manager;
  WithQueryManager *with_query_manager;
  ResultTable *result_table = new ResultTable();
  relationship_query_manager = new RelationshipQueryManager(pkb_);
  pattern_query_manager = new PatternQueryManager(pkb_);
  with_query_manager = new WithQueryManager();

  // guaranteed to have at least 3 clause groups,
  // where first group is without synonyms,
  // second group is without any return synonyms,
  // and third groups on are those with synonyms in results
  printf("clause groups size: %d\n", clause_groups_.size());
  for (int i = 0; i < clause_groups_.size(); i++) {
    printf("I IS : %d\n", i);
    std::vector<ClauseVertex> clause_vertexes = clause_groups_.at(i)->get_clauses();
    printf("1\n");
    ResultTable *intermediate_table = new ResultTable();
    bool first_table_entry = true;
    for (ClauseVertex clause_vertex: clause_vertexes) {
      printf("2\n");
      ClauseType type = clause_vertex.get_clause()->get_type();
      auto synonym_to_entities_vec = GetPossibleEntitiesVec(clause_vertex);
      bool has_synonyms = !clause_vertex.get_synonyms_used().empty();
      ResultTable *table;
      switch (type) {
        case ClauseType::SuchThatClause:
          table = relationship_query_manager->EvaluateRelationship(std::dynamic_pointer_cast<SuchThatClause>(clause_vertex.get_clause()),
                                                                   synonym_to_entities_vec);
          break;
        case ClauseType::PatternClause:
          table = pattern_query_manager->EvaluatePattern(std::dynamic_pointer_cast<PatternClause>(clause_vertex.get_clause()),
                                                         synonym_to_entities_vec);
          break;
        case ClauseType::WithClause:
          table = with_query_manager->EvaluateWith(std::dynamic_pointer_cast<WithClause>(clause_vertex.get_clause()),
                                                   synonym_to_entities_vec);
          break;
        default:throw std::runtime_error("Unknown ClauseType found");
      }
      if (table == nullptr) {  // if a nullptr is received, it means that clause evaluates to false or empty results
        return ConvertToOutput(result_table, false);
      }
      if (first_table_entry && has_synonyms) {
        printf("GOTTTT1\n");
        intermediate_table->set_table(*table);
        first_table_entry = false;
      } else if (has_synonyms) {
        printf("GOTTTT2\n");
        intermediate_table->NaturalJoin(*table);
      }
    }
    printf("finished for loop\n");
    if (i == 1) {  // first two groups which only has to evaluate true / false
      if (intermediate_table->get_table()->size() == 0 && !first_table_entry) {
        return ConvertToOutput(result_table, false);
      }
    } else if (i == 2) {  // first group when the result table is initially empty
      printf("rehana\n");
      for (std::vector<std::string> v : *intermediate_table->get_table()) {
        for (std::string s : v) {
          printf("%s ", s.c_str());
        }
        printf("\n");
      }
      if (!first_table_entry && intermediate_table->get_table()->empty()) {  // found a clause
        printf("wtfukkkk\n");
        return ConvertToOutput(result_table, false);
      }
      result_table->set_table(*intermediate_table);
    } else if (i > 2){
      printf("in here\n");
      result_table->CrossJoin(*intermediate_table);
      if (!first_table_entry && result_table->get_table()->empty()) {
        printf("it empty");
        return ConvertToOutput(result_table, false);
      }
    }

  }
  printf("DAFKKK\n");
  for (std::vector<std::string> v : *result_table->get_table()) {
    for (std::string s : v) {
      printf("%s ", s.c_str());
    }
    printf("\n");
  }
  return ConvertToOutput(result_table, true);
}

std::unordered_map<std::string, std::vector<Entity *>> QueryEvaluator::GetPossibleEntitiesVec(ClauseVertex clause_vertex) {
  std::vector<std::string> synonyms_used = clause_vertex.get_synonyms_used();
  std::unordered_map<std::string, std::vector<Entity *>> ret;
  for (std::string synonym : synonyms_used) {
    EntityType type = synonym_to_entity_dec_->at(synonym)->get_type();
    std::vector<Entity *> entities;
    switch (type) {
      case EntityType::ProgLine:
      case EntityType::Stmt: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_all_statements();
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Read: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Read);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Print: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Print);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Call: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Call);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::While: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::While);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::If: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::If);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Assign: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Assign);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Variable: {
        std::vector<Variable *> entities_var;
        entities_var = pkb_->get_all_variables();
        for (Variable *var : entities_var) {
          auto *entity = static_cast<Entity *>(var);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Constant: {
        std::vector<Constant *> entities_const;
        entities_const = pkb_->get_all_constants();
        for (Constant *cons : entities_const) {
          auto *entity = static_cast<Entity *>(cons);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Procedure: {
        std::vector<Procedure *> entities_proc;
        entities_proc = pkb_->get_all_procedures();
        for (Procedure *proc : entities_proc) {
          auto *entity = static_cast<Entity *>(proc);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::None:
        throw std::runtime_error("Unknown EntityType!");
    }
    ret.insert({synonym, entities});
  }
//  RelationshipQueryManager *relationship_query_manager;
//  PatternQueryManager *pattern_query_manager;
//  if (!relationships_->empty() &&
//      !IsEmpty(synonym_to_entity_result)) {
//    relationship_query_manager = new RelationshipQueryManager(synonym_to_entity_result,
//                                                              relationships_,
//                                                              entities_to_return_,
//                                                              pkb_,
//                                                              has_two_repeated_synonyms_);
//
//    relationship_query_manager->EvaluateRelationships();
//  }
//
//  // Check if any entity vector is empty
//  // If it is, return empty result.
//  if (IsEmpty(synonym_to_entity_result)) {
//    return new std::vector<std::string>{};
//  }
//
//  if (!patterns_->empty() &&
//      !IsEmpty(synonym_to_entity_result)) {
//    pattern_query_manager = new PatternQueryManager(synonym_to_entity_result,
//                                                    patterns_,
//                                                    entities_to_return_,
//                                                    pkb_,
//                                                    has_two_repeated_synonyms_);
//
//    pattern_query_manager->EvaluatePatterns();
//  }
//  // Check if any entity vector is empty
//  // If it is, return empty result.
//  if (IsEmpty(synonym_to_entity_result)) {
//    return new std::vector<std::string>{};
//  }
//
//  // If there are repeated synonyms between relationship and pattern
//  // run relationship manager again
//  if (has_one_repeated_synonym_) {
//    relationship_query_manager->EvaluateRelationships();
//  }
//
//  if (has_two_repeated_synonyms_) {
//    std::vector<std::pair<int, std::string>> *relationship_vec =
//        relationship_query_manager->get_stmt_var_pair_vector();
//    std::vector<std::pair<int, std::string>> *pattern_vec =
//        pattern_query_manager->get_vec_results();
//    sort(relationship_vec->begin(), relationship_vec->end());
//    sort(pattern_vec->begin(), pattern_vec->end());
//    std::vector<std::pair<int, std::string>> result;
//    std::set_intersection(relationship_vec->begin(), relationship_vec->end(),
//                          pattern_vec->begin(), pattern_vec->end(), std::back_inserter(result));
//
//    auto *output = new std::vector<std::string>();
//    if (patterns_->at(0)->get_synonym()->get_synonym() == entities_to_return_->at(0)) {
//      for (std::pair<int, std::string> pair : result) {
//        // Add item to results vector if it doesn't already exist in vector.
//        if (std::find(output->begin(), output->end(), std::to_string(pair.first)) ==
//        output->end()) {
//          output->push_back(std::to_string(pair.first));
//        }
//      }
//      return output;
//    } else if (patterns_->at(0)->get_variable()->get_synonym() == entities_to_return_->at(0)) {
//      for (std::pair<int, std::string> pair : result) {
//        // Add item to results vector if it doesn't already exist in vector.
//        if (std::find(output->begin(), output->end(), pair.second) == output->end()) {
//          output->push_back(pair.second);
//        }
//      }
//      return output;
//    } else if (!result.empty()) {
//      ConvertToOutput(synonym_to_entity_result);
//    } else {
//      return {};
//    }
//    ret.insert({synonym, entities});
//  }
  return ret;
}

bool QueryEvaluator::IsEmpty(std::unordered_map<std::string,
                                                std::vector<Entity *>> *synonym_to_entity_result) {
  for (auto &pair : *synonym_to_entity_result) {
    if (pair.second.empty()) {
      return true;
    }
  }
  if (synonym_to_entity_result->empty()) return true;
  return false;
}

// Converting from vector Entity objects to vector of strings to output.
std::vector<std::string>
*QueryEvaluator::ConvertToOutput(ResultTable *table_result, bool is_valid_query) {
  printf("came here\n");
  auto *output = new std::vector<std::string>();
  printf("IS VALID QUERY: %d\n", is_valid_query);
  printf("ENTITIES TO RETURN SIZE: %d\n", entities_to_return_->size());
  printf("RETURN TYPE IS BOOLEAN: %d\n", entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean);
  printf("RETURN TYPE IS DEFAULT: %d\n", entities_to_return_->at(0)->get_return_type() == ReturnType::Default);
  printf("RETURN TYPE IS Integer: %d\n", entities_to_return_->at(0)->get_return_type() == ReturnType::Integer);
  printf("RETURN TYPE IS Name: %d\n", entities_to_return_->at(0)->get_return_type() == ReturnType::Name);


  if (!is_valid_query) {
    if (entities_to_return_->size() == 1 && entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean) {
      printf("WTFFFF1\n");
      output->push_back("FALSE");
    }
    return output;
  }

  if (entities_to_return_->size() == 1 && entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean) {
    printf("WTFFF2\n");
    output->push_back("TRUE");
    return output;
  }

//  if (table_result->get_table()->empty() && is_valid_query) {  // return entities not within clauses
//    printf("gonna do this\n");
//    return GetAllPossibleReturnResults();
//  }


  std::vector<int> indexes_of_return_entities;
  auto synonym_to_index_map = table_result->get_synonym_to_index();
  auto index_to_synonym_map = table_result->get_index_to_synonym();
  // first loop to detect all synonyms in return entities that were not in clauses
  for (ResultClause *result_clause : *entities_to_return_) {
    std::string elem = result_clause->get_elem();
    std::string synonym = result_clause->get_synonym();
    printf("CAME HERE BEFORE LIAO LA BOSS\n");
    printf("ELEM IS : %s\n", elem.c_str());
    printf("SYNONYM IS : %s\n", synonym.c_str());
    if (synonym_to_index_map->find(elem) == synonym_to_index_map->end()) {  // use the exact result as key
//      std::string new_synonym = elem;
//      if (elem.find(".") != std::string::npos) {  // retrieve select entities with attrvaluetype
//        new_synonym = elem.substr(0, elem.find("."));
//        if (synonym_to_index_map->find(new_synonym) != synonym_to_index_map->end()) {
//          std::remove(entities_to_return_->begin(), entities_to_return_->end(), elem);
//          continue;
//        }
//        std::replace(entities_to_return_->begin(), entities_to_return_->end(),
//                     elem, new_synonym);
//      }

      printf("GOT COME HERE??????\n");
      ResultTable *new_table = new ResultTable();
      if (synonym_to_index_map->find(synonym) != synonym_to_index_map->end()) {  // synonym was used previously,
        MakeTableForUsedEntity(new_table, result_clause, table_result);
      } else {
        MakeTableForUnusedEntity(new_table, result_clause);
      }

      printf("HMM1\n");
//      synonym_to_index_map->insert({elem, synonym_to_index_map->size()});
//      index_to_synonym_map->insert({index_to_synonym_map->size(), elem});
      printf("HMM2\n");
      if (table_result->get_table()->empty()) {
        printf("HMM3\n");
        table_result = new_table;
        synonym_to_index_map = table_result->get_synonym_to_index();
      } else {
        printf("MM4\n");
        table_result->NaturalJoin( *new_table);
      }
    }
  }
  printf("CAME TO HERE AFTER DONE\n");
  // second loop to retrieve synonym indexes
  for (ResultClause *result_clause : *entities_to_return_) {
    printf("issue here?\n");
    indexes_of_return_entities.push_back(synonym_to_index_map->at(result_clause->get_elem()));
    printf("no\n");
  }

  std::set<std::string> unique_results;
  for (std::vector<std::string> row : *table_result->get_table()) {
    std::stringstream ss;
    for (int i = 0; i < indexes_of_return_entities.size(); i++) {
      int index = indexes_of_return_entities.at(i);
//      ResultClause *result_clause = entities_to_return_->at(i);
//      if (result_clause->get_return_type() == ReturnType::Default)
//      || IsDefaultType(result_clause)) {
      ss << row.at(index);
//      } else {  // must be from integer to name
//        int line_num = std::stoi(row.at(index));
//        std::string exp = pkb_->get_statement(line_num)->get_expr_string();
//        ss << exp;
//      }

      if (i != indexes_of_return_entities.size() - 1) {
        ss << " ";
      }
    }
    printf("finished\n");
//    output->push_back(ss.str());
    unique_results.insert(ss.str());
  }
  output->assign(unique_results.begin(), unique_results.end());
//  std::string synonym = entities_to_return_->at(0);
//  EntityType entity_type = synonym_to_entity_dec_->at(synonym)->get_type();
//  std::vector<Entity *> entities = table_result->at(synonym);
//
//  // Populating the output vector based on return type needed.
//  if (QueryEvaluator::IsStmt(entity_type)) {
//    for (auto &stmt : entities) {
//      auto *entity = dynamic_cast<Statement *>(stmt);
//      output->push_back(std::to_string(entity->get_stmt_no()));
//    }
//  } else if (entity_type == EntityType::Variable) {
//    for (auto &var : entities) {
//      auto *entity = dynamic_cast<Variable *>(var);
//      output->push_back(entity->get_name());
//    }
//  } else if (entity_type == EntityType::Procedure) {
//    for (auto &proc : entities) {
//      auto *entity = dynamic_cast<Procedure *>(proc);
//      output->push_back(entity->get_name());
//    }
//  } else if (entity_type == EntityType::Constant) {
//    for (auto &cons : entities) {
//      auto *entity = dynamic_cast<Constant *>(cons);
//      output->push_back(entity->get_value());
//    }
//  }
  return output;
}

void QueryEvaluator::MakeTableForUnusedEntity(ResultTable *result_table, ResultClause *result_clause) {
  printf("synonym now is %s\n", result_clause->get_elem().c_str());
//  EntityType type = synonym_to_entity_dec_->at(result_clause)->get_type();
  printf("okay\n");

  std::string elem = result_clause->get_elem();
  EntityType synonym_type = result_clause->get_synonym_type();
  ReturnType return_type = result_clause->get_return_type();
  std::vector<std::string> to_add = std::vector<std::string>();

  switch (synonym_type) {
    case EntityType::ProgLine:
    case EntityType::Stmt: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_all_statements();
      for (Statement *stmt: entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Read: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Read);
      for (Statement *stmt: entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          for (auto &i : *stmt->get_modifies()) {
            to_add.push_back(i);
          }
        }
      }
      break;
    }
    case EntityType::Print: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Print);
      for (Statement *stmt: entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          printf("OR DID IT COME HERE LEL\n");
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          printf("DID IT COME HERE BIJJJJ\n");
          for (auto &i : *stmt->get_uses()) {
            to_add.push_back(i);
          }
        }
      }
      break;
    }
    case EntityType::Call: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Call);
      for (Statement *stmt: entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          to_add.push_back(stmt->get_called_proc_name());
        }
      }
      break;
    }
    case EntityType::While: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::While);
      for (Statement *stmt: entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::If: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::If);
      for (Statement *stmt: entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Assign: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Assign);
      for (Statement *stmt: entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Variable: {
      std::vector<Variable *> entities_var;
      entities_var = pkb_->get_all_variables();
      for (Variable *var: entities_var) {
        to_add.push_back(var->get_name());
      }
      break;
    }
    case EntityType::Constant: {
      std::vector<Constant *> entities_const;
      entities_const = pkb_->get_all_constants();
      for (Constant *cons: entities_const) {
        to_add.push_back(cons->get_value());
      }
      break;
    }
    case EntityType::Procedure: {
      std::vector<Procedure *> entities_proc;
      entities_proc = pkb_->get_all_procedures();
      for (Procedure *proc: entities_proc) {
        to_add.push_back(proc->get_name());
      }
      break;
    }
    case EntityType::None:
      throw std::runtime_error("Unknown EntityType!");
  }
  printf("uhhhhhhh\n");

  result_table->AddSingleColumn(elem, to_add);
}


void QueryEvaluator::MakeTableForUsedEntity(ResultTable *result_table,
                                            ResultClause *result_clause,
                                            ResultTable *other_result_table) {
  printf("synonym now is %s\n", result_clause->get_elem().c_str());
  printf("okay\n");
  std::vector<std::string> synonym_vec = other_result_table->GetColumnVec(result_clause->get_synonym());

  std::string elem = result_clause->get_elem();
  EntityType synonym_type = result_clause->get_synonym_type();
  ReturnType return_type = result_clause->get_return_type();
  std::vector<std::string> to_add = std::vector<std::string>();

  for (std::string synonym : synonym_vec) {
    switch (synonym_type) {
      case EntityType::ProgLine:
      case EntityType::While:
      case EntityType::If:
      case EntityType::Assign:
      case EntityType::Constant:
      case EntityType::Stmt: {  // guaranteed to be Integer
        to_add.push_back(synonym);
        break;
      }
      case EntityType::Read: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          for (auto &i : *stmt->get_modifies()) {
            to_add.push_back(i);
          }
        }
        break;
      }
      case EntityType::Print: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          for (auto &i : *stmt->get_uses()) {
            to_add.push_back(i);
          }
        }
        break;
      }
      case EntityType::Call: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          to_add.push_back(stmt->get_called_proc_name());
        }
        break;
      }
      case EntityType::Variable:
      case EntityType::Procedure: {  // guaranteed to be Name
        to_add.push_back(synonym);
        break;
      }
      case EntityType::None:
        throw std::runtime_error("Unknown EntityType!");
    }
  }

  printf("uhhhhhhh\n");

  result_table->AddDoubleColumns(result_clause->get_synonym(), synonym_vec, elem, to_add);
}


bool QueryEvaluator::IsDefaultType(ResultClause *result_clause) {
  EntityType type = result_clause->get_synonym_type();
  ReturnType return_type = result_clause->get_return_type();
  switch (type) {
    case EntityType::Stmt:
    case EntityType::Read:
    case EntityType::Print:
    case EntityType::Call:
    case EntityType::While:
    case EntityType::If:
    case EntityType::Assign:
    case EntityType::Constant:
      if (return_type == ReturnType::Integer) {
        return true;
      }
      break;
    case EntityType::Variable:
    case EntityType::Procedure:
      if (return_type == ReturnType::Name) {
        return true;
      }
      break;
    default:
      return false;
  }
}

//std::vector<std::string> *QueryEvaluator::GetAllPossibleReturnResults() {
//  std::vector<std::vector<std::string> *> synonym_to_entities_result;
//  for (std::string synonym : *entities_to_return_) {
//    printf("synonym now is %s\n", synonym.c_str());
//    EntityType type = synonym_to_entity_dec_->at(synonym)->get_type();
//    printf("okay\n");
//    std::vector<std::string> *to_add = new std::vector<std::string>();
//    switch (type) {
//      case EntityType::Stmt: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_all_statements();
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::Read: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::Read);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::Print: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::Print);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::Call: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::Call);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::While: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::While);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::If: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::If);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::Assign: {
//        std::vector<Statement *> entities_stmt;
//        entities_stmt = pkb_->get_statements(NodeType::Assign);
//        for (Statement *stmt: entities_stmt) {
//          to_add->push_back(std::to_string(stmt->get_stmt_no()));
//        }
//        break;
//      }
//      case EntityType::Variable: {
//        std::vector<Variable *> entities_var;
//        entities_var = pkb_->get_all_variables();
//        for (Variable *var: entities_var) {
//          to_add->push_back(var->get_name());
//        }
//        break;
//      }
//      case EntityType::Constant: {
//        std::vector<Constant *> entities_const;
//        entities_const = pkb_->get_all_constants();
//        for (Constant *cons: entities_const) {
//          to_add->push_back(cons->get_value());
//        }
//        break;
//      }
//      case EntityType::Procedure: {
//        std::vector<Procedure *> entities_proc;
//        entities_proc = pkb_->get_all_procedures();
//        for (Procedure *proc: entities_proc) {
//          to_add->push_back(proc->get_name());
//        }
//        break;
//      }
//      case EntityType::ProgLine:
//      case EntityType::None:
//        throw std::runtime_error("Unknown EntityType!");
//    }
//    // TODO: ACCOUNT FOR MULTIPLE PERMUTATIONS
//    synonym_to_entities_result.push_back(to_add);
//  }
//  printf("uhhhhhhh\n");
//  printf("size of syn to entities result: %d\n", synonym_to_entities_result.size());
//  return synonym_to_entities_result.at(0);
//
//}

//bool QueryEvaluator::IsStmt(EntityType entity_type) {
//  if (entity_type == EntityType::Stmt
//      || entity_type == EntityType::Read
//      || entity_type == EntityType::Print
//      || entity_type == EntityType::Call
//      || entity_type == EntityType::While
//      || entity_type == EntityType::If
//      || entity_type == EntityType::Assign) {
//    return true;
//  }
//  return false;
//}
