#include <iostream>
#include <string>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

int main(int argc, char *argv[])
{
    std::stringstream ss;
//    ss << "procedure p; stmt s1; stmt s2;   stmt s3;\nSelect s1 such that Follows*(  s1   ,2)";
ss << "stmt s; variable v; Select s such that Modifies(s, 'a')";
//    ss << "stmt select; Select select";
//    ss << "stmt select; stmt    select1; Select select such that Parent*(6, 5)";
    QueryObject* query = new QueryObject(ss.str());
    PQLQuery *clause = query->getPQLQuery();
    if (clause == nullptr) {
        std::cout << "Invalid PQL Query!" << "\n" << flush;
    } else {
        for (Entity* entity: *clause->getQueryEntities()) {
            std::cout << "Select " << entity->getValue() << "\n" << flush;
        }

        for (Relationship* relationship: *clause->getQueryRelationships()) {
            std::cout << "Relationship type is: " << relationship->getTypeStr() << "\n";
            std::cout << "Left is: " << relationship->getLeftRef()->getValue();
            std::cout << " Right is: " << relationship->getRightRef()->getValue() << "\n" << flush;
        }
    }
}

//};

