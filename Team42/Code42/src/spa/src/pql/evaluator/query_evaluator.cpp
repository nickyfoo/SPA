#include "query_evaluator.h"
#include "ast.h"  // Get kind enum
#include "statement.h"
#include "procedure.h"
#include "variable.h"
#include "pattern_query_manager.h"
#include "relationship_query_manager.h"
#include "pql_query.h"
#include "pkb.h"

QueryEvaluator::QueryEvaluator(PQLQuery *pql_query, PKB *pkb) {
  if (pql_query != nullptr) {
    QueryEvaluator::entities_to_return_ = pql_query->get_query_entities();
    QueryEvaluator::relationships_ = pql_query->get_query_relationships();
    QueryEvaluator::patterns_ = pql_query->get_query_patterns();
    QueryEvaluator::synonym_to_entity_dec_ = pql_query->get_synonym_to_entities();
    this->pkb_ = pkb;
    this->has_one_repeated_synonym_ = pql_query->has_one_repeated_synonym();
    this->has_two_repeated_synonyms_ = pql_query->has_two_repeated_synonyms();
  } else {
    QueryEvaluator::entities_to_return_ = nullptr;
    QueryEvaluator::relationships_ = nullptr;
    QueryEvaluator::patterns_ = nullptr;
    QueryEvaluator::synonym_to_entity_dec_ = nullptr;
    this->pkb_ = nullptr;
    this->has_one_repeated_synonym_ = false;
    this->has_two_repeated_synonyms_ = false;
  }
}

std::vector<std::string> *QueryEvaluator::Evaluate() {
  if (synonym_to_entity_dec_ == nullptr) {
    return new std::vector<std::string>{};
  }

  // Initialising an unordered map mapping synonym_ to Entity objects.
  auto *synonym_to_entity_result =
      new std::unordered_map<std::string, std::vector<Entity *>>();
  for (auto &pair : *synonym_to_entity_dec_) {
    EntityType type = pair.second->get_type();
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
      case EntityType::None:
        return new std::vector<std::string>{};
    }
    synonym_to_entity_result->insert({pair.first, entities});
  }
  RelationshipQueryManager *relationship_query_manager;
  PatternQueryManager *pattern_query_manager;
  if (!relationships_->empty() &&
      !IsEmpty(synonym_to_entity_result)) {
    relationship_query_manager = new RelationshipQueryManager(synonym_to_entity_result,
                                                              relationships_,
                                                              entities_to_return_,
                                                              pkb_,
                                                              has_two_repeated_synonyms_);

    relationship_query_manager->EvaluateRelationships();
  }
  // Check if any entity vector is empty
  // If it is, return empty result.
  if (IsEmpty(synonym_to_entity_result)) {
    return new std::vector<std::string>{};
  }

  if (!patterns_->empty() &&
      !IsEmpty(synonym_to_entity_result)) {
    pattern_query_manager = new PatternQueryManager(synonym_to_entity_result,
                                                    patterns_,
                                                    entities_to_return_,
                                                    pkb_,
                                                    has_two_repeated_synonyms_);

    pattern_query_manager->EvaluatePatterns();
  }
  // Check if any entity vector is empty
  // If it is, return empty result.
  if (IsEmpty(synonym_to_entity_result)) {
    return new std::vector<std::string>{};
  }

  // If there are repeated synonyms between relationship and pattern
  // run relationship manager again
  if (has_one_repeated_synonym_) {
    relationship_query_manager->EvaluateRelationships();
  }

  if (has_two_repeated_synonyms_) {
    std::vector<std::pair<int, std::string>> *relationship_vec =
        relationship_query_manager->get_stmt_var_pair_vector();
    std::vector<std::pair<int, std::string>> *pattern_vec =
        pattern_query_manager->get_vec_results();
    sort(relationship_vec->begin(), relationship_vec->end());
    sort(pattern_vec->begin(), pattern_vec->end());
    std::vector<std::pair<int, std::string>> result;
    std::set_intersection(relationship_vec->begin(), relationship_vec->end(),
                          pattern_vec->begin(), pattern_vec->end(), std::back_inserter(result));

    auto *output = new std::vector<std::string>();
    if (patterns_->at(0)->get_synonym()->get_synonym() == entities_to_return_->at(0)) {
      for (std::pair<int, std::string> pair : result) {
        // Add item to results vector if it doesn't already exist in vector.
        if (std::find(output->begin(), output->end(), std::to_string(pair.first)) ==
        output->end()) {
          output->push_back(std::to_string(pair.first));
        }
      }
      return output;
    } else if (patterns_->at(0)->get_left_ref()->get_synonym() == entities_to_return_->at(0)) {
      for (std::pair<int, std::string> pair : result) {
        // Add item to results vector if it doesn't already exist in vector.
        if (std::find(output->begin(), output->end(), pair.second) == output->end()) {
          output->push_back(pair.second);
        }
      }
      return output;
    } else if (!result.empty()) {
      ConvertToOutput(synonym_to_entity_result);
    } else {
      return {};
    }
  }
  return ConvertToOutput(synonym_to_entity_result);
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
*QueryEvaluator::ConvertToOutput(
    std::unordered_map<std::string,
                       std::vector<Entity *>> *synonym_to_entity_result) {
  auto *output = new std::vector<std::string>();
  // Will loop through the entities_to_return_ for Advanced version
  std::string synonym = entities_to_return_->at(0);
  EntityType entity_type = synonym_to_entity_dec_->at(synonym)->get_type();
  std::vector<Entity *> entities = synonym_to_entity_result->at(synonym);

  // Populating the output vector based on return type needed.
  if (QueryEvaluator::IsStmt(entity_type)) {
    for (auto &stmt : entities) {
      auto *entity = dynamic_cast<Statement *>(stmt);
      output->push_back(std::to_string(entity->get_stmt_no()));
    }
  } else if (entity_type == EntityType::Variable) {
    for (auto &var : entities) {
      auto *entity = dynamic_cast<Variable *>(var);
      output->push_back(entity->get_name());
    }
  } else if (entity_type == EntityType::Procedure) {
    for (auto &proc : entities) {
      auto *entity = dynamic_cast<Procedure *>(proc);
      output->push_back(entity->get_name());
    }
  } else if (entity_type == EntityType::Constant) {
    for (auto &cons : entities) {
      auto *entity = dynamic_cast<Constant *>(cons);
      output->push_back(entity->get_value());
    }
  }
  return output;
}

bool QueryEvaluator::IsStmt(EntityType entity_type) {
  if (entity_type == EntityType::Stmt
      || entity_type == EntityType::Read
      || entity_type == EntityType::Print
      || entity_type == EntityType::Call
      || entity_type == EntityType::While
      || entity_type == EntityType::If
      || entity_type == EntityType::Assign) {
    return true;
  }
  return false;
}
