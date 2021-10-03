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
    QueryEvaluator::entities_to_return_ = pql_query->get_query_entities();
    QueryEvaluator::clause_groups_ = pql_query->get_clause_groups();
    QueryEvaluator::synonym_to_entity_dec_ = pql_query->get_synonym_to_entities();
    this->pkb_ = pkb;
  } else {
    QueryEvaluator::entities_to_return_ = nullptr;
    QueryEvaluator::clause_groups_ = nullptr;
    QueryEvaluator::synonym_to_entity_dec_ = nullptr;
    this->pkb_ = nullptr;
  }
}

std::vector<std::string> *QueryEvaluator::Evaluate() {
  if (synonym_to_entity_dec_ == nullptr) {
    return new std::vector<std::string>{};
  }

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
  for (int i = 0; i < clause_groups_->size(); i++) {
    vector<ClauseVertex> clause_vertexes = clause_groups.at(i).get_clauses();
    ResultTable *intermediate_table = new ResultTable(clause_vertexes.get_all_synonyms_used());
    bool first_table_entry = true;
    for (ClauseVertex clause_vertex: clause_vertexes) {
      ClauseType type = clause_vertex.get_clause().get_type();
      auto synonym_to_entities_vec = GetPossibleEntitiesVec(clause_vertex);
      ResultTable *table;
      switch (type) {
        case ClauseType::SuchThat:
          table = relationship_query_manager->EvaluateRelationship(dynamic_cast<SuchThatClause>(clause_vertex),
                                                                   synonym_to_entities_vec);
          break;
        case ClauseType::Pattern:
          table = pattern_query_manager->EvaluatePattern(dynamic_cast<PatternClause>(clause_vertex),
                                                         synonym_to_entities_vec);
          break;
        case ClauseType::With:
          table = with_query_manager->EvaluateWith(dynamic_cast<WithClause>(clause_vertex),
                                                   synonym_to_entities_vec);
          break;
        default:throw std::runtime_error("Unknown ClauseType found");
      }
      if (table == nullptr) {  // if a nullptr is received, it means that clause evaluates to false or empty results
        return ConvertToOutput(result_table, false);
      }
      if (first_table_entry) {
        intermediate_table->set_table(*table);
        first_table_entry = false;
      } else {
        intermediate_table->NaturalJoin(*table);
      }
    }
    if (i == 1) {  // first two groups which only has to evaluate true / false
      if (intermediate_table->get_table()->size() == 0) {
        return ConvertToOutput(result_table, false);
      }
    } else if (i == 2) {  // first group when the result table is initially empty
      result_table->set_table(*intermediate_table);
    } else {
      result_table->CrossJoin(*intermediate_table);
      if (result_table->get_table()->empty()) {
        return ConvertToOutput(result_table, false);
      }
    }

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
      case EntityType::ProgLine:
      case EntityType::None:
        throw std::runtime_error("Unknown EntityType!");
    }
    ret.insert({synonym, entities});
  }
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
  auto *output = new std::vector<std::string>();
  // Will loop through the entities_to_return_ for Advanced version

  if (!is_valid_query) {
    if (entities_to_return_->empty()) {  // flag to return boolean values
      output->push_back("FALSE");
    }
    return output;
  }

  if (entities_to_return_->empty()) {  // flag to return boolean values
    output->push_back("TRUE");
    return output;
  }

  std::vector<int> indexes_of_return_entities;
  auto synonym_to_index_map = table_result->get_synonym_to_index();
  for (std::string synonym : *entities_to_return_) {  // retrieve synonyms indexes
    indexes_of_return_entities.push_back(synonym_to_index_map->at(synonym);
  }

  for (std::vector<std::string> row : *table_result->get_table()) {
    std::stringstream ss;
    for (int i = 0; i < indexes_of_return_entities.size(); i++) {
      ss << indexes_of_return_entities.at(i);
      if (i != indexes_of_return_entities.size() - 1) {
        ss << " ";
      }
    }
    output->push_back(ss.str());
  }
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
