//#include "RelationshipQueryManager.h"
//#include "entity_declaration.h"
//#include "Tables/StmtTable.h"
//#include "Statement.h"
//
//RelationshipQueryManager::RelationshipQueryManager(std::unordered_map<std::string, std::vector<Entity*>>* synonymToEntityResult,
//                                                   std::vector<SuchThatClause*>* relationships,
//                                                   std::vector<std::string>* entitiesToReturn) {
//    this->synonymToEntityResult = synonymToEntityResult;
//    this->relationships = relationships;
//    this->entitiesToReturn = entitiesToReturn;
//}
//
//void RelationshipQueryManager::evaluateRelationships() {
//    // Iterating through relationships_ and evaluating one by one. For basic requirements, there will be only 1 relationship.
//    for (SuchThatClause *relationship: *relationships) {
//        RelRef relationshipType = relationship->getType();
//        switch (relationshipType) {
//            case RelRef::Follows:
//                Follows(relationship);
//        }
//    }
//}
//
//void RelationshipQueryManager::Follows(SuchThatClause *relationship) {
//    StmtRef leftEnt = relationship->getLeftRef()->getStmtRef();
//    StmtRef rightEnt = relationship->getRightRef()->getStmtRef();
//    // Going through 9 different cases for Follows
//    if (leftEnt.getType() == StmtRefType::StmtNum && rightEnt.getType() == StmtRefType::StmtNum) { // If both are values eg. Follows(4, 5)
//        int leftArg = leftEnt.getStmtNum();
//        int rightArg = rightEnt.getStmtNum();
//        Statement* stmt = StmtTable::getStatementByLineNo(leftArg);
//        if (!stmt->getFollowers()->count(rightArg)) {
//            synonymToEntityResult->at(entitiesToReturn->at(0)).clear(); // Clear results vector if this relationship is false
//        }
//    } else if (leftEnt.getType() == StmtRefType::Synonym && rightEnt.getType() == StmtRefType::StmtNum) { // eg. Follows(s, 4)
//        int rightArg = rightEnt.getStmtNum();
//        string synonym = leftEnt.getSynonym();
//        vector<Entity*> *entityVec = &synonymToEntityResult->at(synonym);
//        for (int i = 0; i < entityVec->size(); i++) {
//            auto *stmt = (Statement*) entityVec->at(i);
//            auto it = stmt->getFollowers()->begin();
//            if (!stmt->getFollowers()->count(rightArg)) { // For each statement that doesnt have right arg in its followers, it will be removed
//                entityVec->erase(entityVec->begin() + i); //Check if this affects vector in synonymToEntityResult
//                i--;
//            }
//        }
//    } else if (leftEnt.getType() == StmtRefType::StmtNum && rightEnt.getType() == StmtRefType::Synonym) { // eg. Follows(4, s)
//        int leftArg = leftEnt.getStmtNum();
//        string synonym = rightEnt.getSynonym();
//        vector<Entity*> *entityVec = &synonymToEntityResult->at(synonym);
//        for (int i = 0; i < entityVec->size(); i++) {
//            auto *stmt = (Statement*) entityVec->at(i);
//            if (!stmt->getFollowees()->count(leftArg)) { // For each statement that doesnt have left arg in its followees, it will be removed
//                entityVec->erase(entityVec->begin() + i); //Check if this affects vector in synonymToEntityResult
//                i--;
//            }
//        }
//    } else if (leftEnt.getType() == StmtRefType::Synonym && rightEnt.getType() == StmtRefType::Synonym) { // eg Follows(s1, s2)
//        string rightSynonym = rightEnt.getSynonym();
//        string leftSynonym = leftEnt.getSynonym();
//        vector<Entity*> *rightEntityVec = &synonymToEntityResult->at(rightSynonym);
//        vector<Entity*> *leftEntityVec = &synonymToEntityResult->at(leftSynonym);
//        for (int i = 0; i < leftEntityVec->size(); i++) {
//            Statement *stmt = (Statement*) leftEntityVec->at(i);
//            if (stmt->getFollowers()->size() == 0) {
//                leftEntityVec->erase(leftEntityVec->begin() + i); //Remove statements that do not have a follower
//                i--;
//            }
//        }
//        for (int j = 0; j < rightEntityVec->size(); j++) {
//            Statement *stmt = (Statement*) rightEntityVec->at(j);
//            if (stmt->getFollowees()->size() == 0) {
//                rightEntityVec->erase(rightEntityVec->begin() + j); //Remove statements that do not have a followee
//                j--;
//            }
//        }
//    } else if (leftEnt.getType() == StmtRefType::WildCard && rightEnt.getType() == StmtRefType::WildCard) { //eg Follows(_, _)
//        vector<Statement*> entityVec = StmtTable::getAllStatements();
//        bool foundNonEmptyVec = false;
//        for (int i = 0; i < entityVec.size(); i++) {
//            Statement *stmt = entityVec.at(i);
//            if (stmt->getFollowers()->size() != 0) {
//                foundNonEmptyVec = true;
//                break; //As long as any statement has a statement following it then it will return true and move on
//            }
//        }
//        if (!foundNonEmptyVec) {
//            synonymToEntityResult->at(entitiesToReturn->at(0)).clear(); // If none of the statements have something following it then clear results vector
//        }
//    } else if (leftEnt.getType() == StmtRefType::WildCard && rightEnt.getType() == StmtRefType::StmtNum) { // eg Follows(_, 4)
//        int rightArg = rightEnt.getStmtNum();
//        Statement* stmt = StmtTable::getStatementByLineNo(rightArg);
//        if (stmt->getFollowees()->size() == 0) {
//            synonymToEntityResult->at(entitiesToReturn->at(0)).clear(); //If statement with right arg as line number does not have any followees then clear results vector
//        }
//    } else if (leftEnt.getType() == StmtRefType::WildCard && rightEnt.getType() == StmtRefType::Synonym) { // eg Follows(_, s)
//        string rightSynonym = rightEnt.getSynonym();
//        vector<Entity*> *rightEntityVec = &synonymToEntityResult->at(rightSynonym);
//        for (int i = 0; i < rightEntityVec->size(); i++) {
//            Statement *stmt = (Statement*) rightEntityVec->at(i);
//            if (stmt->getFollowees()->size() == 0) { // For each statement that doesnt have any item in its followees, it will be removed.
//                rightEntityVec->erase(rightEntityVec->begin() + i); //Check if this affects vector in synonymToEntityResult
//                i--;
//            }
//        }
//    } else if (leftEnt.getType() == StmtRefType::StmtNum && rightEnt.getType() == StmtRefType::WildCard) { // eg Follows(4, _)
//        int leftArg = leftEnt.getStmtNum();
//        Statement* stmt = StmtTable::getStatementByLineNo(leftArg);
//        if (stmt->getFollowers()->size() == 0) {
//            synonymToEntityResult->at(entitiesToReturn->at(0)).clear(); //If statement with left arg as line number does not have any followers then clear results vector
//        }
//    } else if (leftEnt.getType() == StmtRefType::Synonym && rightEnt.getType() == StmtRefType::WildCard) { // eg Follows(s, _)
//        string leftSynonym = leftEnt.getSynonym();
//        vector<Entity*> *leftEntityVec = &synonymToEntityResult->at(leftSynonym);
//        for (int i = 0; i < leftEntityVec->size(); i++) {
//            Statement *stmt = (Statement*) leftEntityVec->at(i);
//            if (stmt->getFollowees()->size() == 0) { // For each statement that doesnt have any item in its followers, it will be removed.
//                leftEntityVec->erase(leftEntityVec->begin() + i); //Check if this affects vector in synonymToEntityResult
//                i--;
//            }
//        }
//    }
//}
//
//bool RelationshipQueryManager::isStmt(EntityType entityType) {
//    if (entityType == EntityType::Stmt
//    || entityType == EntityType::Read
//    || entityType == EntityType::Print
//    || entityType == EntityType::Call
//    || entityType == EntityType::While
//    || entityType == EntityType::If
//    || entityType == EntityType::Assign) {
//        return true;
//    }
//    return false;
//}