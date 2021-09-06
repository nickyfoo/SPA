#include <iostream>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

// Temp main class to test the PQL Query
int main(int argc, char *argv[]) {
    std::stringstream ss;
//    ss << "assign pattern; variable select;\nSelect pattern pattern pattern (select, _) such that Uses (pattern, select)";
ss << "assign pattern; variable select;"
      "Select pattern pattern pattern (select, _) such that such that Uses (pattern, select)";
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
        for (Relationship* relationship: *clause->getQueryRelationships()) {
            std::cout << "Relationship type: " << relationship->getTypeStr() << "\n";
            std::cout << "Left ref: " << relationship->getLeftRef()->getValue();
            std::cout << " Right ref: " << relationship->getRightRef()->getValue() << "\n";
        }
        std::cout << "\n" << std::flush;
        for (Pattern* pattern: *clause->getQueryPatterns()) {
            std::cout << "Pattern synonym: " << pattern->getSynonym() << "\n";
            std::cout << "Is partial pattern? " << (pattern->isPartialPattern() ? "Yes" : "No") << "\n";
            std::cout << "Left ref: " << pattern->getLeftRef()->getValue();
            std::cout << " Right ref: " << pattern->getRightRef() << "\n";
        }
    }
}

//};

