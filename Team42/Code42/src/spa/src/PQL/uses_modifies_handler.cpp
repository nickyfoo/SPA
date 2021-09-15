#include "uses_modifies_handler.h"

UsesModifiesHandler::UsesModifiesHandler() {}
UsesModifiesHandler *UsesModifiesHandler::instance_ = 0;

UsesModifiesHandler *UsesModifiesHandler::get_instance() {
    if (!instance_) {
        instance_ = new UsesModifiesHandler();
    }
    return instance_;
}

void UsesModifiesHandler::set_args(PKB *pkb,
std::unordered_map<std::string, std::vector<Entity*>>
*synonym_to_entity_result,
SuchThatClause *relationship,
std::vector<std::string> *entities_to_return) {
    this->pkb_ = pkb;
    this->synonym_to_entity_result_ = synonym_to_entity_result;
    this->relationship_ = relationship;
    this->entities_to_return_ = entities_to_return;
}

std::set<std::string>* UsesModifiesHandler::StatementForwarder(
        std::set<std::string> *(Statement::*function)(),
                Statement *stmt) {
    return (stmt->*function)();
}

std::set<int>* UsesModifiesHandler::VariableForwarder(
        std::set<int> *(Variable::*function)(),
                Variable *var) {
    return (var->*function)();
}

void UsesModifiesHandler::set_function_pointers(
        std::set<std::string> *(Statement::*get_normal)(),
        std::set<int> *(Variable::*get_reverse)()) {
    this->get_normal_ = get_normal;
    this->get_reverse_ = get_reverse;
}

void UsesModifiesHandler::Evaluate() {
    StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();  // this will be entref for usesp
    EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();

    // Going through 6 different cases for UsesS
    if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == EntRefType::Synonym) {  // Uses(s, v)
        std::string left_synonym = left_ent.get_synonym();
        std::string right_synonym = right_ent.get_synonym();
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        std::vector<Entity*> *right_entity_vec;
        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = dynamic_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
            if (StatementForwarder(get_normal_, stmt)->empty()) {
                // Remove statements that do not have something it uses.
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
        for (int j = 0; j < right_entity_vec->size(); j++) {
            auto *variable = dynamic_cast<Variable*>(right_entity_vec->at(j));
            if (VariableForwarder(get_reverse_, variable)->empty()) {
                // Remove variables that are not used
                right_entity_vec->erase(right_entity_vec->begin() + j);
                j--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == EntRefType::WildCard) {  // Uses(s, _)
        std::string left_synonym = left_ent.get_synonym();
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = static_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
            // Remove each statement that doesnt use anything.
            if (StatementForwarder(get_normal_, stmt)->empty()) {
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::Synonym &&
    right_ent.get_type() == EntRefType::Argument) {  // Uses(s, "x")
        std::string left_synonym = left_ent.get_synonym();
        std::string right_arg = right_ent.get_argument();
        std::vector<Entity*> *left_entity_vec;
        left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
        for (int i = 0; i < left_entity_vec->size(); i++) {
            auto *stmt = static_cast<Statement*>(left_entity_vec->at(i)); // this will be procedure for usesp
            // Remove each statement that doesnt have right arg in its uses
            if (!StatementForwarder(get_normal_, stmt)->count(right_arg)) {
                left_entity_vec->erase(left_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == EntRefType::Synonym) {  // Uses(4, v)
        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
        std::string right_synonym = right_ent.get_synonym();
        std::vector<Entity*> *right_entity_vec;
        right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
        for (int i = 0; i < right_entity_vec->size(); i++) {
            auto *variable = static_cast<Variable*>(right_entity_vec->at(i));
            // Remove each statement that doesnt have left arg in its users.
            if (!VariableForwarder(get_reverse_, variable)->count(left_arg)) {
                right_entity_vec->erase(right_entity_vec->begin() + i);
                i--;
            }
        }
    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == EntRefType::WildCard) {  // Uses(4, _)
        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
        Statement *stmt = pkb_->get_statement(left_arg);  // this will be a procedure for usesp
        if (StatementForwarder(get_normal_, stmt)->empty()) {
            // If statement with left arg as line number
            // does not use anything then clear results vector
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    } else if (left_ent.get_type() == StmtRefType::StmtNum &&
    right_ent.get_type() == EntRefType::Argument) {  // Uses(4, "x")
        int left_arg = left_ent.get_stmt_num();  // this will be a string for usesp
        std::string right_arg = right_ent.get_argument();
        Statement *stmt = pkb_->get_statement(left_arg);  // this will be a procedure for usesp
        if (!StatementForwarder(get_normal_, stmt)->count(right_arg)) {
            // Clear results vector if this relationship_ is false
            synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
        }
    }
}
