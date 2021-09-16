#include "follows_parents_handler.h"

FollowsParentsHandler::FollowsParentsHandler() {}
FollowsParentsHandler *FollowsParentsHandler::instance_ = 0;

FollowsParentsHandler *FollowsParentsHandler::get_instance() {
    if (!instance_) {
        instance_ = new FollowsParentsHandler();
    }
    return instance_;
}

void FollowsParentsHandler::set_function_pointers(
        std::set<int> *(Statement::*get_normal)(),
        std::set<int> *(Statement::*get_reverse)()) {
    this->get_normal_ = get_normal;
    this->get_reverse_ = get_reverse;
}

void FollowsParentsHandler::set_args(PKB *pkb,
                                     std::unordered_map<std::string, std::vector<Entity*>>
                                     *synonym_to_entity_result,
                                     SuchThatClause *relationship,
                                     std::vector<std::string> *entities_to_return) {
    this->pkb_ = pkb;
    this->synonym_to_entity_result_ = synonym_to_entity_result;
    this->relationship_ = relationship;
    this->entities_to_return_ = entities_to_return;
}

std::set<int>* FollowsParentsHandler::Forwarder(std::set<int> *(Statement::*function)(), Statement *stmt) {
    return (stmt->*function)();
}

void FollowsParentsHandler::Evaluate() {
    StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();
    StmtRef right_ent = relationship_->get_right_ref()->get_stmt_ref();
    // Going through 9 different cases for Follows
    if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == StmtRefType::StmtNum) {  // eg. Follows/Parents(4, 5)
        int left_arg = left_ent.get_stmt_num();
        int right_arg = right_ent.get_stmt_num();
        Statement *stmt = pkb_->get_statement(left_arg);
        if (!Forwarder(get_normal_, stmt)->count(right_arg)) {
            // Clear results vector if this relationship_ is false
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    } else if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == StmtRefType::StmtNum) {  // eg. Follows(s, 4)
        int right_arg = right_ent.get_stmt_num();
        std::string left_synonym = left_ent.get_synonym();
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(left_entity_vec->at(i));
            // Remove each statement that doesnt have right arg in its followers
            if (!Forwarder(get_normal_, stmt)->count(right_arg)) {
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == StmtRefType::Synonym) {  // eg. Follows(4, s)
        int left_arg = left_ent.get_stmt_num();
        std::string right_synonym = right_ent.get_synonym();
        std::vector<Entity*> *right_entity_vec;
        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
        for (int i = 0; i < right_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(right_entity_vec->at(i));
            // Remove each statement that doesnt have left arg in its followees,
            if (!Forwarder(get_reverse_, stmt)->count(left_arg)) {
                right_entity_vec->erase(right_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(s1, s2)
        std::string right_synonym = right_ent.get_synonym();
        std::string left_synonym = left_ent.get_synonym();
        std::vector<Entity*> *right_entity_vec;
        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(left_entity_vec->at(i));
            if (Forwarder(get_normal_, stmt)->empty()) {
                // Remove statements that do not have a follower
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
        for (int j = 0; j < right_entity_vec->size(); j++) {
            auto *stmt = static_cast<Statement*>(right_entity_vec->at(j));
            if (Forwarder(get_reverse_, stmt)->empty()) {
                // Remove statements that do not have a followee
                right_entity_vec->erase(right_entity_vec->begin() + j);
                j--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::WildCard &&
    right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(_, _)
        std::vector<Statement*> entity_vec;
        entity_vec = pkb_->get_all_statements();
        bool found_non_empty_vec = false;
        for (int i = 0; i < entity_vec.size(); i++) {
            Statement *stmt = entity_vec.at(i);
            if (!Forwarder(get_normal_, stmt)->empty()) {
                found_non_empty_vec = true;
                // As long as any statement has a statement following it
                // then it will return true and move on
                break;
            }
        }
        if (!found_non_empty_vec) {
            // If none of the statements have something following it
            // then clear results vector
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    } else if (left_ent.get_type() == StmtRefType::WildCard &&
    right_ent.get_type() == StmtRefType::StmtNum) {  // eg Follows(_, 4)
        int right_arg = right_ent.get_stmt_num();
        Statement *stmt = pkb_->get_statement(right_arg);
        if (Forwarder(get_reverse_, stmt)->empty()) {
            // If statement with right arg as line number
            // does not have any followees then clear results vector
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    } else if (left_ent.get_type() == StmtRefType::WildCard &&
    right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(_, s)
        std::string right_synonym = right_ent.get_synonym();
        std::vector<Entity*> *right_entity_vec;
        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
        for (int i = 0; i < right_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(right_entity_vec->at(i));
            // Remove each statement that doesnt have any item in its followees.
            if (Forwarder(get_reverse_, stmt)->empty()) {
                right_entity_vec->erase(right_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(4, _)
        int left_arg = left_ent.get_stmt_num();
        Statement *stmt = pkb_->get_statement(left_arg);
        if (Forwarder(get_normal_, stmt)->empty()) {
            // If statement with left arg as line number
            // does not have any followers then clear results vector
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    } else if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(s, _)
        std::string left_synonym = left_ent.get_synonym();
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(left_entity_vec->at(i));
            // Remove each statement that doesnt have any item in its followers.
            if (Forwarder(get_normal_, stmt)->empty()) {
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
    }
}

