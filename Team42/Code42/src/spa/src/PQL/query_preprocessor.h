#pragma once

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "select_clause_parser.h"
#include "entity_declaration_parser.h"

class QueryPreprocessor {
 public:
    explicit QueryPreprocessor(std::string input);
    PQLQuery *get_pql_query();
 private:
    std::unordered_map<std::string, EntityDeclaration> syn_to_entity_;
    PQLQuery *clauses_;
    static std::tuple<std::vector<std::string>*, std::string>
            SplitEntitiesAndSelectClause(std::string input);
    static std::unordered_map<std::string, EntityDeclaration*>
            *MakeEntitiesMap(std::vector<std::string> *entities);
    static PQLQuery *MakePQLQuery(
            std::unordered_map<std::string, EntityDeclaration*> *entities_map,
            const std::string& select_clause);
};
