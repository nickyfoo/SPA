//#pragma once
//
//#include <vector>
//#include "unordered_map"
//#include "string"
//#include "entity_declaration.h"
//#include "SuchThatClause.h"
//#include "Entity.h"
//
//
//class RelationshipQueryManager {
//public:
//    RelationshipQueryManager(std::unordered_map<std::string, std::vector<Entity*>>* synonymToEntityResult,
//                             std::vector<SuchThatClause*>* relationships,
//                             std::vector<std::string>* entitiesToReturn);
//    void evaluateRelationships();
//
//private:
//    std::unordered_map<std::string, std::vector<Entity*>>* synonymToEntityResult;
//    std::vector<SuchThatClause*>* relationships;
//    std::vector<std::string>* entitiesToReturn;
//    void Follows(SuchThatClause* relationship);
//    bool isStmt(EntityType entityType);
//};
