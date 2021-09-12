#include "pattern_query_manager.h"
#include "entity_declaration.h"
#include "Tables/StmtTable.h"
#include "Statement.h"

PatternQueryManager::PatternQueryManager(std::unordered_map<std::string, std::vector<Entity*>> *synonymToEntityResult,
                                                   std::vector<PatternClause*> *patterns,
                                                   std::vector<std::string> *entitiesToReturn) {
    this->synonym_to_entity_result_ = synonymToEntityResult;
    this->patterns_ = patterns;
    this->entities_to_return_ = entitiesToReturn;
}

void PatternQueryManager::evaluatePatterns() {
    // Iterating through patterns_ and evaluating one by one. For basic requirements, there will be only 1 pattern.
    for (PatternClause *pattern : *patterns_) {
        ManagePatterns(pattern);
    }
}

//pattern synonym(left_ent, right_ent)
// pattern a(v, _)

void PatternQueryManager::ManagePatterns(PatternClause *pattern) {
    EntityDeclaration *synonym = pattern->get_synonym();
    EntRef *left_ent = pattern->get_left_ref();
    ExpressionSpec *rightEnt = pattern->get_right_ref();
    std::string pattern_to_check = rightEnt->get_expression();
    bool is_partial_pattern = rightEnt->IsPartialPattern();
    vector<Entity*> *entity_vec = &synonym_to_entity_result_->at(synonym->getSynonym());
    for (int i = 0; i < entity_vec->size(); i++) {
        auto *assign = (Statement*) &synonym_to_entity_result_->at(entity_vec->at(i));
        if (left_ent->get_type() == EntRefType::Synonym) {
            vector<Entity*> *variable_vec = &synonym_to_entity_result_->at(left_ent->get_synonym());
            bool found_var = false;
            for (int j = 0; j < variable_vec->size(); j++) {
                auto* variable = (Variable*) &variable_vec->at(j);
                if (!assign->getModifies()->count(variable->get_name()) ||
                !PKB.checkAssignmentRhs(assign, pattern_to_check, is_partial_pattern)) {
                    entity_vec->erase(entity_vec.begin() + i);
                    i--;
                    break;
                }
            }
        } else if (left_ent->get_type() == EntRefType::Argument) {
            if (!assign->getModifies()->count(left_ent->get_argument()) ||
            !PKB.checkAssignmentRhs(assign, pattern_to_check, is_partial_pattern)) {
                entity_vec->erase(entity_vec.begin() + i);
                i--;
            }
        } else if (left_ent->get_type() == EntRefType::WildCard) {
            if (!assign->getModifies().size() == 0 ||
            !PKB.checkAssignmentRhs(assign, pattern_to_check, is_partial_pattern)) {
                entity_vec->erase(entity_vec.begin() + i);
                i--;
            }
        }
    }
}
