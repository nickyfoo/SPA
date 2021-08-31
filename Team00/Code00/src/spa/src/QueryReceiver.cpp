#include <iostream>
#include <string>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

int main(int argc, char *argv[])
{
    std::stringstream ss;
//    ss << "procedure p; stmt s1; stmt s2;   stmt s3;\nSelect s1 such that Follows(s1,s2)";
    ss << "stmt select; Select select";
    QueryObject* query = new QueryObject(ss.str());
    PQLQuery *clause = query->getPQLQuery();
    if (clause == nullptr) {
        std::cout << "Invalid PQL Query!" << "\n" << flush;
    } else {
        for (Entity* entity: *clause->getQueryEntities()) {
            std::cout << "Synonym is: " << entity->getSynonym() << "\n" << flush;
        }

        for (Relationship* relationship: *clause->getQueryRelationships()) {
            std::cout << "Left is: " << relationship->getLeftRef()->getSynonym();
            std::cout << "   Right is: " << relationship->getRightRef()->getSynonym() << "\n" << flush;
        }
    }
}

//};

