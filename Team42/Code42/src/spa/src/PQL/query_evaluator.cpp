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
            new std::unordered_map<std::string, std::vector<Entity*>>();
    for (auto &pair : *synonym_to_entity_dec_) {
        EntityType type = pair.second->get_type();
        std::vector<Entity*> entities;
        switch (type) {  // TODO: Combine EntityType enum with AST's kind enum
            case EntityType::Stmt: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_all_statements();
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::Read: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::Read);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::Print: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::Print);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::Call: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::Call);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::While: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::While);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::If: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::If);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
            case EntityType::Assign: {
                std::vector<Statement*> entities_stmt;
                entities_stmt = pkb_->get_statements(NodeType::Assign);
                for (Statement *stmt : entities_stmt) {
                    Entity *entity = static_cast<Entity*>(stmt);
                    entities.push_back(entity);
                }
                break;
            }
//            case EntityType::Variable:
//                printf("HELLOOOOO");
//                break;
                // Get variables from PKB
//            case EntityType::Constant:
//                // Get constants from PKB
//            case EntityType::Procedure:
//                // Get procedures from PKB
        }
        synonym_to_entity_result->insert({pair.first, entities});
    }
//    if (relationships_->size() != 0 &&
//        !IsEmpty(synonym_to_entity_result)) {
//        RelationshipQueryManager relationshipQueryManager =
//                RelationshipQueryManager(synonym_to_entity_result,
//                                         relationships_,
//                                         entities_to_return_,
//                                         pkb_);
//        relationshipQueryManager.EvaluateRelationships();
//    }
    // Do same check for patterns too and pass to PatternManager
    if (patterns_->size() != 0 &&
    !IsEmpty(synonym_to_entity_result)) {
        PatternQueryManager pattern_query_manager =
                PatternQueryManager(synonym_to_entity_result,
                                         patterns_,
                                         entities_to_return_,
                                         pkb_);
        pattern_query_manager.EvaluatePatterns();
    }
    return ConvertToOutput(synonym_to_entity_result);
}

bool QueryEvaluator::IsEmpty(std::unordered_map<std::string,
                             std::vector<Entity*>> *synonym_to_entity_result) {
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
                std::vector<Entity*>>*synonym_to_entity_result) {
    auto *output = new std::vector<std::string>();
    // Will loop through the entities_to_return_ for Advanced version
    std::string synonym = entities_to_return_->at(0);
    EntityType entity_type = synonym_to_entity_dec_->at(synonym)->get_type();
    std::vector<Entity*> entities = synonym_to_entity_result->at(synonym);

    // Populating the output vector based on return type_ needed.
    if (QueryEvaluator::IsStmt(entity_type)) {
        for (int i = 0; i < entities.size(); i++) {
            auto *entity = static_cast<Statement*>(entities.at(i));
            output->push_back(std::to_string(entity->get_stmt_no()));
        }
    } else if (entity_type == EntityType::Variable) {
        for (int i = 0; i < entities.size(); i++) {
            auto *entity = static_cast<Variable*>(entities.at(i));
            output->push_back(entity->get_name());
        }
    } else if (entity_type == EntityType::Procedure) {
        for (int i = 0; i < entities.size(); i++) {
            auto *entity = static_cast<Procedure*>(entities.at(i));
            output->push_back(entity->get_name());
        }
    } else if (entity_type == EntityType::Constant) {
        // TODO: Wait for PKB code for constant
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
