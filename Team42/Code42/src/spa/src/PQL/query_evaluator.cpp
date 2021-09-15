#include "query_evaluator.h"
#include "ast.h"  // Get kind enum
#include "statement.h"
#include "procedure.h"
#include "variable.h"
#include "relationship_query_manager.h"
#include "pql_query.h"
#include "pkb.h"

QueryEvaluator::QueryEvaluator(PQLQuery *pql_query, PKB *pkb) {
  if (pql_query != nullptr) {
    QueryEvaluator::entities_to_return_ = pql_query->get_query_entities();
    QueryEvaluator::relationships_ = pql_query->get_query_relationships();
    QueryEvaluator::synonym_to_entity_dec_ = pql_query->get_synonym_to_entities();
    this->pkb_ = pkb;
  } else {
    QueryEvaluator::entities_to_return_ = nullptr;
    QueryEvaluator::relationships_ = nullptr;
    QueryEvaluator::synonym_to_entity_dec_ = nullptr;
    this->pkb_ = nullptr;
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
    switch (type) {  // TODO: Combine EntityType enum with AST's kind enum
      case EntityType::Stmt: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_all_statements();
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Read: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Read);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Print: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Print);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Call: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Call);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::While: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::While);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::If: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::If);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Assign: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Assign);
        for (Statement *stmt : entities_stmt) {
          Entity *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Variable: {
        std::vector<Variable *> entities_var;
        entities_var = pkb_->get_all_variables();
        for (Variable *var : entities_var) {
          Entity *entity = static_cast<Entity *>(var);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Constant: {
        std::vector<Constant *> entities_const;
        entities_const = pkb_->get_all_constants();
        for (Constant *cons : entities_const) {
          Entity *entity = static_cast<Entity *>(cons);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Procedure: {
        std::vector<Procedure *> entities_proc;
        entities_proc = pkb_->get_all_procedures();
        for (Procedure *proc : entities_proc) {
          Entity *entity = static_cast<Entity *>(proc);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::None:
        return new std::vector<std::string>{};
    }
    synonym_to_entity_result->insert({pair.first, entities});
  }
  if (relationships_->size() != 0 &&
      !IsEmpty(synonym_to_entity_result)) {
    RelationshipQueryManager relationshipQueryManager =
        RelationshipQueryManager(synonym_to_entity_result,
                                 relationships_,
                                 entities_to_return_,
                                 pkb_);
    relationshipQueryManager.EvaluateRelationships();
  }
  // Do same check for patterns too and pass to PatternManager

  return ConvertToOutput(synonym_to_entity_result);
}

bool QueryEvaluator::IsEmpty(std::unordered_map<std::string,
                                                std::vector<Entity *>> *synonym_to_entity_result) {
  for (auto &pair : *synonym_to_entity_result) {
    if (pair.second.size() == 0) {
      return true;
    }
  }
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

  // Populating the output vector based on return type_ needed.
  if (QueryEvaluator::IsStmt(entity_type)) {
    for (auto & stmt : entities) {
      auto *entity = dynamic_cast<Statement *>(stmt);
      output->push_back(std::to_string(entity->get_stmt_no()));
    }
  } else if (entity_type == EntityType::Variable) {
    for (auto & var : entities) {
      auto *entity = dynamic_cast<Variable *>(var);
      output->push_back(entity->get_name());
    }
  } else if (entity_type == EntityType::Procedure) {
    for (auto & proc : entities) {
      auto *entity = dynamic_cast<Procedure *>(proc);
      output->push_back(entity->get_name());
    }
  } else if (entity_type == EntityType::Constant) {
    for (auto & cons : entities) {
      auto *entity = dynamic_cast<Constant *>(cons);
      output->push_back(std::to_string(entity->get_value()));
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
