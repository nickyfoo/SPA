#include <iostream>
#include <string>
#include <tuple>
#include "QueryObject.h"
#include "PQLQuery.h"

int main(int argc, char *argv[])
{
    std::stringstream ss;
//    ss << "procedure p; stmt s1; stmt s2;   stmt s3;\nSelect s1 such that Follows*(  s1   ,2)";
//ss << "stmt s; variable v; assign a; Select s pattern a(_,   _) such that Modifies(s, 'a') such that Uses(s, v)";
//ss << "assign a; while w;\nSelect a pattern a ('count', _) such that Parent* (w, a)\n";
//ss << "stmt s; variable v; while w; Select v such that Uses(10, 'count')\n";
//    ss << "stmt select; Select select";
//    ss << "stmt select; stmt    select1; Select select such that Parent*(6, 5)";
//ss << "assign a;\n"
//      "Select a pattern a ( _ , 'count + 1')";
ss << "assign a; Select a pattern a ( 'normSq' , _'cenX * cenX'_)";

//ss << "assign a; while w;\n Select a such that Parent* (w, a) pattern a ('count', _)";
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

        for (Pattern* pattern: *clause->getQueryPatterns()) {
            std::cout << "Pattern synonym is: " << pattern->getSynonym() << "\n";
            std::cout << "Is partial pattern? " << (pattern->isPartialPattern() ? "Yes" : "No") << "\n";
            std::cout << "Left is: " << pattern->getLeftRef()->getValue();
            std::cout << " Right is: " << pattern->getRightRef() << "\n" << flush;
        }
    }
}

//};

