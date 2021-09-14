#include "pattern_query_manager.h"
#include "entity_declaration.h"
#include "stmt_table.h"
#include "statement.h"

PatternQueryManager::PatternQueryManager(std::unordered_map<std::string, std::vector<Entity*>> *synonymToEntityResult,
                                                   std::vector<PatternClause*> *patterns,
                                                   std::vector<std::string> *entitiesToReturn,
                                                   PKB *pkb) {
    this->synonym_to_entity_result_ = synonymToEntityResult;
    this->patterns_ = patterns;
    this->entities_to_return_ = entitiesToReturn;
    this->pkb_ = pkb;
}

void PatternQueryManager::EvaluatePatterns() {
    // Iterating through patterns_ and evaluating one by one. For basic requirements, there will be only 1 pattern.
    for (PatternClause *pattern : *patterns_) {
        ManagePatterns(pattern);
    }
}

void PatternQueryManager::ManagePatterns(PatternClause *pattern) {
    EntityDeclaration *synonym = pattern->get_synonym();
    EntRef *left_ent = pattern->get_left_ref();
    ExpressionSpec *rightEnt = pattern->get_right_ref();
    std::string pattern_to_check = rightEnt->get_expression();
    bool is_partial_pattern = rightEnt->IsPartialPattern();
    std::vector<Entity *> *entity_vec = &synonym_to_entity_result_->at(synonym->get_synonym()); // list of assignment object
    std::set<Entity *> var_set;
    std::string left_synonym;
    for (int i = 0; i < entity_vec->size(); i++) {
        auto *assign = (Statement*) entity_vec->at(i); // for each assign object
        if (assign->get_modifies()->size() == 0 || !pkb_->TestAssignmentPattern(assign, pattern_to_check, is_partial_pattern)) {
            entity_vec->erase(entity_vec->begin() + i);
            i--;
        } else {
            if (left_ent->get_type() == EntRefType::Synonym) { // eg pattern a(v, "sth")
                left_synonym = left_ent->get_synonym();
                std::vector<Entity*> *variable_vec = &synonym_to_entity_result_->at(left_ent->get_synonym()); // getting list of variable objects
                for (int j = 0; j < variable_vec->size(); j++) {
                    auto* variable = (Variable*) &variable_vec->at(j); // for each variable object
                    if (assign->get_modifies()->count(variable->get_name())) {
                        var_set.insert(variable_vec->at(j));
                    }
                }
            } else if (left_ent->get_type() == EntRefType::Argument) {
                if (!assign->get_modifies()->count(left_ent->get_argument())) {
                    entity_vec->erase(entity_vec->begin() + i);
                    i--;
                }
            }
        }
    }
    if (!var_set.empty()) {
        std::vector<Entity*> *variable_vec = &synonym_to_entity_result_->at(left_synonym); // getting list of variable objects
        for (int k = 0; k < variable_vec->size(); k++) {
            if (!var_set.count(&variable_vec->at(k)->)) {
                variable_vec->erase(variable_vec->begin() + k);
                k--;
            }
        }
    }
}