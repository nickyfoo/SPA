#include <iostream>
#include <tuple>
#include "query_preprocessor.h"
#include "pql_query.h"

// Temp main class to test the PQL Query
int main(int argc, char *argv[]) {
    std::stringstream ss;
    ss << "assign a;\nSelect a pattern a ( _ , 'count + 1')";
    std::cout << "PQL Query to process is: " << ss.str() << "\n" << std::flush;

    auto* query = new QueryPreprocessor(ss.str());
    PQLQuery *clause = query->get_pql_query();

    if (clause == nullptr) {
        std::cout << "Invalid PQL Query!" << "\n";
    } else {
        for (std::string entity : *clause->get_query_entities()) {
            std::cout << "Select: " << entity << "\n";
        }
        std::cout << "\n" << std::flush;
        for (SuchThatClause* relationship : *clause->get_query_relationships()) {
            std::cout << "SuchThatClause type: "
            << relationship->get_type_str() << "\n";
            if (relationship->get_left_ref()->get_type()
            == SuchThatRefType::Statement) {
                std::cout << "Left ref: "
                << relationship->get_left_ref()->get_stmt_ref().get_value();
            } else if (relationship->get_left_ref()->get_type()
            == SuchThatRefType::Entity) {
                std::cout << "Left ref: "
                << relationship->get_left_ref()->get_ent_ref().get_value();
            }

            if (relationship->get_right_ref()->get_type()
            == SuchThatRefType::Statement) {
                std::cout << "Right ref: "
                << relationship->get_right_ref()->get_stmt_ref().get_value();
            } else if (relationship->get_right_ref()->get_type()
            == SuchThatRefType::Entity) {
                std::cout << "Right ref: "
                << relationship->get_right_ref()->get_ent_ref().get_value();
            }
        }
        std::cout << "\n" << std::flush;
        for (PatternClause* pattern : *clause->get_query_patterns()) {
            std::cout << "PatternClause synonym_: "
            << pattern->get_synonym()->getSynonym()
            << "\nIs partial pattern? "
            << (pattern->get_right_ref()->IsPartialPattern() ? "Yes" : "No")
            << "\nLeft ref: "
            << pattern->get_left_ref()->get_value()
            << " Right ref: "<< (pattern->get_right_ref()->IsWildCard()
            ? "_" : pattern->get_right_ref()->get_expression()) << "\n";
        }
    }
}

//};

