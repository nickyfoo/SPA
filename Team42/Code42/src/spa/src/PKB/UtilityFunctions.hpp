#pragma once
#include "../parser/AST.hpp"
#include "VariableTable.h"

class UtilityFunctions {
public:
	static void defaultFunction(ast::Node* node) {
		return;
	}

	static void addVariable(ast::Node* node) {
		ast::IdentifierNode* castedIdentifierNode = (ast::IdentifierNode*)node;
		VariableTable::addVariable(castedIdentifierNode->name);
		return;
	}
};