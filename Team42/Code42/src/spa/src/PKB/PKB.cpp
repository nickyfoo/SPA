#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PKB.h"
#include "../parser/AST.hpp"

PKB::PKB(ast::Node* programRoot) {
    root = programRoot;
}

PKB::~PKB() {}

// Add variables, add statements
void PKB::initialPass() {
    std::map<ast::Kind, std::vector<std::function<void(ast::Node*)>>> functions;

    functions[ast::Identifier].push_back([this](ast::Node* node) { PKB::addVariable(node); });
	for (int i = ast::Identifier; i < ast::NUM_KIND; i++) {
	    if (std::find(validStmts.begin(), validStmts.end(), i) != validStmts.end()) {
	        functions[(ast::Kind) i].push_back([this](ast::Node* node) { PKB::addStmt(node); });
	    }
	}

	functions[ast::Assign].push_back([this](ast::Node* node) { PKB::addExprString(node); });
	functions[ast::Procedure].push_back([this](ast::Node* node) { PKB::addProc(node); });
	ast::visit(root, functions);

	stmtTable.printStmts();
	procTable.printProcs();
	varTable.printVars();
}

void PKB::getFollows() {
    std::map<ast::Kind, std::vector<std::function<void(ast::Node*)>>> functions;
    functions[ast::Procedure].push_back([this](ast::Node* node) { PKB::followsProcessProcedureNode(node); });
    functions[ast::If].push_back([this](ast::Node* node) { PKB::followsProcessIfNode(node); });
    functions[ast::While].push_back([this](ast::Node* node) { PKB::followsProcessWhileNode(node); });
	ast::visit(root, functions);
	stmtTable.processFollows();
	stmtTable.processFollowsStar();
}


void PKB::getParent() {
    std::map<ast::Kind, std::vector<std::function<void(ast::Node*)>>> functions;
    functions[ast::If].push_back([this](ast::Node* node) { PKB::parentProcessIfNode(node); });
    functions[ast::While].push_back([this](ast::Node* node) { PKB::parentProcessWhileNode(node); });
	ast::visit(root, functions);
	stmtTable.processParent();
	stmtTable.processParentStar();
}

void PKB::addVariable(ast::Node* node) {
    ast::IdentifierNode* castedIdentifierNode = (ast::IdentifierNode*)node;
    varTable.addVariable(castedIdentifierNode->name);
}

void PKB::addStmt(ast::Node* node) {
    stmtTable.addStmt(node);
}

void PKB::addExprString(ast::Node* node) {
    if (node->kind == ast::Assign) {
        ast::AssignNode* castedAssignNode{ static_cast<ast::AssignNode*>(node) };
        stmtTable.getStatementByLineNo(castedAssignNode->stmtNo)->setExprString(castedAssignNode->expr->exprString);
    }
}

void PKB::addProc(ast::Node* node) {
    ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*) node;
    procTable.addProcedure(castedProcedureNode->name);
}

void PKB::followsProcessProcedureNode(ast::Node* node) {
    ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*)node;
    std::vector<int> lineNumbers;

    for (ast::Node* n : castedProcedureNode->stmtLst) {
        lineNumbers.push_back(Statement::getStmtNo(n));
    }
    sort(lineNumbers.begin(), lineNumbers.end());
    for (int i = 1; i < lineNumbers.size(); i++) {
        stmtTable.getStatementByLineNo(lineNumbers[i - 1])->addFollower(lineNumbers[i]);
        stmtTable.getStatementByLineNo(lineNumbers[i])->addFollowee(lineNumbers[i - 1]);
    }
}

void PKB::followsProcessIfNode(ast::Node* node) {
    ast::IfNode* castedIfNode = (ast::IfNode*)node;
    std::vector<int> thenLineNumbers, elseLineNumbers;

    for (ast::Node* n : castedIfNode->thenStmtLst) {
        thenLineNumbers.push_back(Statement::getStmtNo(n));
    }
    sort(thenLineNumbers.begin(), thenLineNumbers.end());
    for (int i = 1; i < thenLineNumbers.size(); i++) {
        stmtTable.getStatementByLineNo(thenLineNumbers[i - 1])->addFollower(thenLineNumbers[i]);
        stmtTable.getStatementByLineNo(thenLineNumbers[i])->addFollowee(thenLineNumbers[i - 1]);
    }

    for (ast::Node* n : castedIfNode->thenStmtLst) {
        elseLineNumbers.push_back(Statement::getStmtNo(n));
    }
    sort(elseLineNumbers.begin(), elseLineNumbers.end());
    for (int i = 1; i < elseLineNumbers.size(); i++) {
        stmtTable.getStatementByLineNo(elseLineNumbers[i - 1])->addFollower(elseLineNumbers[i]);
        stmtTable.getStatementByLineNo(elseLineNumbers[i])->addFollowee(elseLineNumbers[i - 1]);
    }

}

void PKB::followsProcessWhileNode(ast::Node* node) {
    ast::WhileNode* castedWhileNode = (ast::WhileNode*)node;
    //todo: not sure how the stmtlst is organized/sorted, so I'll store line numbers for now and sort them
    std::vector<int> lineNumbers;
    for (ast::Node* n : castedWhileNode->stmtLst) {
        lineNumbers.push_back(Statement::getStmtNo(n));
    }
    sort(lineNumbers.begin(), lineNumbers.end());
    for (int i = 1; i < lineNumbers.size(); i++) {
        stmtTable.getStatementByLineNo(lineNumbers[i - 1])->addFollower(lineNumbers[i]);
        stmtTable.getStatementByLineNo(lineNumbers[i])->addFollowee(lineNumbers[i - 1]);
    }
}

void PKB::parentProcessIfNode(ast::Node* node) {
    ast::IfNode* castedIfNode = (ast::IfNode*)node;
    Statement* ifStatement = stmtTable.getStatementByLineNo(castedIfNode->stmtNo);
    for (ast::Node* n : castedIfNode->thenStmtLst) {
        ifStatement->addChild(Statement::getStmtNo(n));
        stmtTable.getStatementByLineNo(Statement::getStmtNo(n))->addParent(ifStatement->getStmtNo()); //might want to do error checking here if NULL
    }

    for (ast::Node* n : castedIfNode->thenStmtLst) {
        ifStatement->addChild(Statement::getStmtNo(n));
        stmtTable.getStatementByLineNo(Statement::getStmtNo(n))->addParent(ifStatement->getStmtNo()); //might want to do error checking here if NULL
    }
}

void PKB::parentProcessWhileNode(ast::Node* node) {
    ast::WhileNode* castedWhileNode = (ast::WhileNode*)node;
    Statement* whileStatement = stmtTable.getStatementByLineNo(castedWhileNode->stmtNo);
    for (ast::Node* n : castedWhileNode->stmtLst) {
        whileStatement->addChild(Statement::getStmtNo(n));
        stmtTable.getStatementByLineNo(Statement::getStmtNo(n))->addParent(whileStatement->getStmtNo()); //might want to do error checking here if NULL
    }
}

Statement* PKB::getStatementByLineNo(int lineNo) {
    return stmtTable.getStatementByLineNo(lineNo);
}

std::vector<Statement*> PKB::getAllStatements() {
    return stmtTable.getAllStatements();
}

std::vector<Statement*> PKB::getStatements(ast::Kind type) {
    return stmtTable.getStatements(type);
}

void PKB::printStmts() {
    stmtTable.printStmts();
}

int PKB::getLargestStmtNum() {
    return stmtTable.getLargestStmtNum();
}


std::vector<Variable*> PKB::getAllVariables() {
    return varTable.getAllVariables();
}

std::vector<Procedure*> PKB::getAllProcedures() {
    return procTable.getAllProcedures();
}