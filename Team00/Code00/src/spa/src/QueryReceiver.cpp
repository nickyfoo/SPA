#include <iostream>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

// Temp main class to test the PQL Query
int main(int argc, char *argv[]) {
    std::stringstream ss;
    ss << "stmt s; variable v; assign a; Select s pattern a(_,   _) such that Modifies(s, 'a') such that Uses(s, v)";
    std:: cout << "PQL Query to process is: " << ss.str() << "\n" << std::flush;

    QueryObject* query = new QueryObject(ss.str());
    PQLQuery *clause = query->getPQLQuery();

    if (clause == nullptr) {
        std::cout << "Invalid PQL Query!" << "\n";
    } else {
        for (Entity* entity: *clause->getQueryEntities()) {
            std::cout << "Select: " << entity->getValue() << "\n";
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

