#include <iostream>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

// Temp main class to test the PQL Query
int main(int argc, char *argv[]) {
    std::stringstream ss;
//    ss << "assign pattern; variable select;\nSelect pattern pattern pattern (select, _) such that Uses (pattern, select)";
//ss << "assign pattern; variable select;"
//      "Select pattern pattern pattern (select, _) such that such that Uses (pattern, select)";
ss << "assign a;\n"
      "Select a pattern a ( _ , 'count + 1')";
    std::cout << "PQL Query to process is: " << ss.str() << "\n" << std::flush;

    auto* query = new QueryObject(ss.str());
    PQLQuery *clause = query->getPQLQuery();

    if (clause == nullptr) {
        std::cout << "Invalid PQL Query!" << "\n";
    } else {
        for (std::string entity: *clause->getQueryEntities()) {
            std::cout << "Select: " << entity << "\n";
        }
        std::cout << "\n" << std::flush;
        for (SuchThatClause* relationship: *clause->getQueryRelationships()) {
            std::cout << "SuchThatClause type: " << relationship->getTypeStr() << "\n";
            if (relationship->getLeftRef()->getType() == SuchThatRefType::Statement) {
                std::cout << "Left ref: " << relationship->getLeftRef()->getStmtRef().getValue();
            } else if (relationship->getLeftRef()->getType() == SuchThatRefType::Entity) {
                std::cout << "Left ref: " << relationship->getLeftRef()->getEntRef().getValue();
            }

            if (relationship->getRightRef()->getType() == SuchThatRefType::Statement) {
                std::cout << "Right ref: " << relationship->getRightRef()->getStmtRef().getValue();
            } else if (relationship->getRightRef()->getType() == SuchThatRefType::Entity) {
                std::cout << "Right ref: " << relationship->getRightRef()->getEntRef().getValue();
            }
        }
        std::cout << "\n" << std::flush;
        for (PatternClause* pattern: *clause->getQueryPatterns()) {
            std::cout << "PatternClause synonym: " << pattern->getSynonym()->getValue() << "\n";
            std::cout << "Is partial pattern? " << (pattern->getRightRef()->isPartialPattern() ? "Yes" : "No") << "\n";
            std::cout << "Left ref: " << pattern->getLeftRef()->getValue();
            std::cout << " Right ref: " << (pattern->getRightRef()->isWildCard() ? "_" : pattern->getRightRef()->getExpression()) << "\n";
        }
    }
}

//};

