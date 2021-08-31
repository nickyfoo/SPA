#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PKB.h"
#include "../parser/AST.hpp"

using namespace std;

void PKB::initialPass() {
	functions.assign(ast::NUM_KIND, &UtilityFunctions::defaultFunction);
	functions[ast::Identifier] = &UtilityFunctions::addVariable;
	ast::visit(root, functions);
}