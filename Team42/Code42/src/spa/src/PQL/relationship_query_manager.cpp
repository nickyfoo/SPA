#include "relationship_query_manager.h"
#include "entity_declaration.h"
#include "statement.h"
#include "follows_parents_handler.h"
#include "uses_modifies_handler.h"
#include "usesp_modifiesp_handler.h"
#include "variable.h"
#include "procedure.h"

RelationshipQueryManager::RelationshipQueryManager(
        std::unordered_map<std::string, std::vector<Entity*>>
        *synonym_to_entity_result,
        std::vector<SuchThatClause*> *relationships,
        std::vector<std::string> *entities_to_return,
        PKB *pkb) {
    this->synonym_to_entity_result_ = synonym_to_entity_result;
    this->relationships_ = relationships;
    this->entities_to_return_ = entities_to_return;
    this->pkb_ = pkb;
}

void RelationshipQueryManager::EvaluateRelationships() {
    // Iterating through relationships_ and evaluating one by one.
    // For basic requirements, there will be only 1 relationship_.
    for (SuchThatClause *relationship : *relationships_) {
        RelRef relationship_type = relationship->get_type();
        switch (relationship_type) {
            case RelRef::Follows: {
                FollowsParentsHandler *follows_parents_handler =
                        FollowsParentsHandler::get_instance();
                follows_parents_handler->set_function_pointers(&Statement::get_followers, &Statement::get_followees);
                follows_parents_handler->set_args(pkb_, synonym_to_entity_result_, relationship, entities_to_return_);
                follows_parents_handler->Evaluate();
                break;
            }
            case RelRef::FollowsT: {
                FollowsParentsHandler *follows_parents_handler =
                        FollowsParentsHandler::get_instance();
                follows_parents_handler->set_function_pointers(&Statement::get_followees_star, &Statement::get_followees_star);
                follows_parents_handler->set_args(pkb_, synonym_to_entity_result_, relationship, entities_to_return_);
                follows_parents_handler->Evaluate();
                break;
            }
            case RelRef::Parent: {
                FollowsParentsHandler *follows_parents_handler =
                        FollowsParentsHandler::get_instance();
                follows_parents_handler->set_function_pointers(&Statement::get_children, &Statement::get_parents);
                follows_parents_handler->set_args(pkb_, synonym_to_entity_result_, relationship, entities_to_return_);
                follows_parents_handler->Evaluate();
                break;
            }
            case RelRef::ParentT: {
                FollowsParentsHandler *follows_parents_handler =
                        FollowsParentsHandler::get_instance();
                follows_parents_handler->set_function_pointers(&Statement::get_children_star, &Statement::get_parents_star);
                follows_parents_handler->set_args(pkb_, synonym_to_entity_result_, relationship, entities_to_return_);
                follows_parents_handler->Evaluate();
                break;
            }
//            case RelRef::UsesS: {
//                UsesModifiesHandler *uses_modifies_handler =
//                        UsesModifiesHandler::get_instance();
//                //add uses functions here
//                break;
//            }
//            case RelRef::ModifiesS: {
//              UsesModifiesHandler *uses_modifies_handler =
//                  UsesModifiesHandler::get_instance();
//              //add uses functions here
//              break;
//            }
        }
    }
}

//void RelationshipQueryManager::Uses(SuchThatClause relationship) {
//    StmtRef left_ent = relationship.get_left_ref()->get_stmt_ref();  // this will be entref for usesp
//    EntRef right_ent = relationship.get_right_ref()->get_ent_ref();
//
//    // Going through 6 different cases for UsesS
//    if (left_ent.get_type() == StmtRefType::Synonym &&
//    right_ent.getType() == EntRefType::Synonym) {  // Uses(s, v)
//        std::string left_synonym = left_ent.get_synonym();
//        std::string right_synonym = right_ent.get_synonym();
//        std::vector<Entity*> *left_entity_vec;
//        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
//        std::vector<Entity*> *right_entity_vec;
//        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
//        for (int i = 0; i < left_entity_vec->size(); i++) {
//            auto *stmt = static_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
//            if (stmt->get_uses()->empty()) {
//                // Remove statements that do not have something it uses.
//                left_entity_vec->erase(left_entity_vec->begin() + i);
//                i--;
//            }
//        }
//        for (int j = 0; j < right_entity_vec->size(); j++) {
//            auto *variable = static_cast<Variable*>(right_entity_vec->at(j));
//            if (variable->get_used()->empty()) {
//                // Remove variables that are not used
//                right_entity_vec->erase(right_entity_vec->begin() + j);
//                j--;
//            }
//        }
//    } else if (left_ent.get_type() == StmtRefType::Synonym &&
//    right_ent.getType() == EntRefType::WildCard) {  // Uses(s, _)
//        std::string left_synonym = left_ent.get_synonym();
//        std::vector<Entity*> *left_entity_vec;
//        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
//        for (int i = 0; i < left_entity_vec->size(); i++) {
//            auto *stmt = static_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
//            // Remove each statement that doesnt use anything.
//            if (stmt->get_uses()->empty()) {
//                left_entity_vec->erase(left_entity_vec->begin() + i);
//                i--;
//            }
//        }
//    } else if (left_ent.get_type() == StmtRefType::Synonym &&
//    right_ent.getType() == EntRefType::Argument) {  // Uses(s, "x")
//        std::string left_synonym = left_ent.get_synonym();
//        std::string right_arg = right_ent.get_argument();
//        std::vector<Entity*> *left_entity_vec;
//        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
//        for (int i = 0; i < left_entity_vec->size(); i++) {
//            auto *stmt = static_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
//            // Remove each statement that doesnt have right arg in its uses
//            if (!stmt->get_uses()->count(right_arg)) {
//                left_entity_vec->erase(left_entity_vec->begin() + i);
//                i--;
//            }
//        }
//    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
//    right_ent.getType() == EntRefType::Synonym) {  // Uses(4, v)
//        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
//        std::string right_synonym = right_ent.get_synonym();
//        std::vector<Entity*> *right_entity_vec;
//        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
//        for (int i = 0; i < right_entity_vec->size(); i++) {
//            auto *variable = static_cast<Variable*>(right_entity_vec->at(i));
//            // Remove each statement that doesnt have left arg in its users.
//            if (!variable->get_users()->count(left_arg)) {
//                right_entity_vec->erase(right_entity_vec->begin() + i);
//                i--;
//            }
//        }
//    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
//    right_ent.getType() == EntRefType::WildCard) {  // Uses(4, _)
//        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
//        Statement *stmt = StmtTable::get_statment_by_line_no(left_arg);  // this will be a procedure for usesp
//        if (stmt->get_uses()->empty()) {
//            // If statement with left arg as line number
//            // does not use anything then clear results vector
//            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
//        }
//    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
//    right_ent.getType() == EntRefType::Argument) {  // Uses(4, "x")
//        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
//        std::string right_arg = right_ent.get_argument();
//        Statement *stmt = StmtTable::get_statment_by_line_no(left_arg);  // this will be a procedure for usesp
//        if (!stmt->get_uses()->count(right_arg)) {
//            // Clear results vector if this relationship_ is false
//            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
//        }
//    }
//}



//void PatternQueryManager::ManagePatterns(PatternClause *pattern) {
//    EntityDeclaration *synonym = pattern->get_synonym();
//    EntRef *left_ent = pattern->get_left_ref();
//    ExpressionSpec *rightEnt = pattern->get_right_ref();
//    std::string pattern_to_check = rightEnt->get_expression();
//    bool is_partial_pattern = rightEnt->IsPartialPattern();
//    vector<Entity*> *entity_vec = &synonym_to_entity_result_->at(synonym->get_synonym()); // list of assignment object
//    std::set<Entity*> var_set;
//    std::string left_synonym;
//    for (int i = 0; i < entity_vec->size(); i++) {
//        auto *assign = (Statement*) entity_vec->at(i); // for each assign object
//        if (assign->get_modifies()->size() == 0 || !PKB.check_assignment_rhs(assign, pattern_to_check, is_partial_pattern)) {
//            entity_vec->erase(entity_vec.begin() + i);
//            i--;
//        } else {
//            if (left_ent->get_type() == EntRefType::Synonym) { // eg pattern a(v, "sth")
//                left_synonym = left_ent->get_synonym;
//                std::vector<Entity*> *variable_vec = &synonym_to_entity_result_->at(left_ent->get_synonym()); // getting list of variable objects
//                for (int j = 0; j < variable_vec->size(); j++) {
//                    auto* variable = (Variable*) &variable_vec->at(j); // for each variable object
//                    if (assign->get_modifies()->count(variable->get_name())) {
//                        var_set.insert(variable_vec->at(j));
//                    }
//                }
//            } else if (left_ent->get_type() == EntRefType::Argument) {
//                if (!assign->get_modifies()->count(left_ent->get_argument())) {
//                    entity_vec->erase(entity_vec.begin() + i);
//                    i--;
//                }
//            }
//        }
//    }
//    if (!var_set.empty()) {
//        std::vector<Entity*> *variable_vec = &synonym_to_entity_result_->at(left_synonym); // getting list of variable objects
//        for (int k = 0; k < variable_vec->size(); k++) {
//            if (!var_set->count(&variable_vec->at(k))) {
//                variable_vec->erase(variable_vec.begin() + k);
//                k--;
//            }
//        }
//    }
//}
