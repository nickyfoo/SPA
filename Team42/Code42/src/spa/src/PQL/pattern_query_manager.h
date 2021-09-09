#pragma once

#include <vector>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "pattern_clause.h"
#include "Entity.h"


class PatternQueryManager {
public:
    PatternQueryManager(std::unordered_map<std::string, std::vector<Entity*>>* synonymToEntityResult,
                             std::vector<PatternClause*>* patterns,
                             std::vector<std::string>* entitiesToReturn);
    void evaluatePatterns();

private:
    std::unordered_map<std::string, std::vector<Entity*>>* synonym_to_entity_result_;
    std::vector<PatternClause*>* patterns_;
    std::vector<std::string>* entities_to_return_;
    void ManagePatterns(PatternClause* pattern);
};
