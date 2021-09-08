#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Tables/ProcTable.h"
#include "Tables/StmtTable.h"
#include "Tables/VarTable.h"

class PKB {
public:
    inline static const std::vector<ast::Kind> validStmts = {
            ast::Assign,
            ast::If,
            ast::While,
            ast::Read,
            ast::Print,
            ast::Call
    };
    PKB(ast::Node* programRoot);
	~PKB();

	void initialPass(); 
	void getFollows();
	void getParent();

	std::vector<Variable*> getAllVariables();
	std::vector<Procedure*> getAllProcedures();

	void addVariable(ast::Node* node);
	void addStmt(ast::Node* node);
	void addExprString(ast::Node* node);
	void addProc(ast::Node* node);

	void followsProcessProcedureNode(ast::Node* node);
	void followsProcessIfNode(ast::Node* node);
	void followsProcessWhileNode(ast::Node* node);

	void parentProcessIfNode(ast::Node* node);
	void parentProcessWhileNode(ast::Node* node);

	// Statement table
	Statement* getStatementByLineNo(int lineNo);
	std::vector<Statement*> getAllStatements();
	std::vector<Statement*> getStatements(ast::Kind type);
	void printStmts();
	int getLargestStmtNum();

	// API called from here
	/*
	//todo: add issue about enums/discuss with people
	vector<Statement*> getStatements(ast::kind type);
	Statement* getStatementbyLineNo(lineNo);

	*/

private:
    ast::Node* root;
    ProcTable procTable;
    StmtTable stmtTable;
    VarTable varTable;
};