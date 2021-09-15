#pragma once
#include "unordered_map"
#include "string"
#include "entity.hpp"
#include "such_that_clause.h"
#include "set"
#include "statement.h"
#include "variable.h"
#include "pkb.h"

class UsesModifiesHandler {
 public:
    static UsesModifiesHandler *get_instance();
    void set_args(PKB *pkb,
    std::unordered_map<std::string, std::vector<Entity*>>
    *synonym_to_entity_result,
    SuchThatClause *relationship,
    std::vector<std::string> *entities_to_return);
    void set_function_pointers(
        std::set<std::string> *(Statement::*get_normal)(),
            std::set<int> *(Variable::*get_reverse)());
    void Evaluate();

 private:
    static UsesModifiesHandler *instance_;
    UsesModifiesHandler();
    std::set<std::string> *(Statement::*get_normal_)();
    std::set<int> *(Variable::*get_reverse_)();
    PKB *pkb_;
    std::unordered_map<std::string, std::vector<Entity*>> *synonym_to_entity_result_;
    SuchThatClause *relationship_;
    std::vector<std::string> *entities_to_return_;
    std::set<std::string>* StatementForwarder(
            std::set<std::string> *(Statement::*function)(),
                    Statement *stmt);
    std::set<int>* VariableForwarder(
            std::set<int> *(Variable::*function)(),
                    Variable *var);
};
