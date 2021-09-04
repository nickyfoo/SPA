#include "AST.hpp"
#include <iostream>
using namespace ast;

std::vector<Node*> ast::nextNodes(Node* node) {
    std::vector<Node*> nextNodes;
    switch (node->kind) {
    case Identifier:
    case Constant:
        break;
    case Expression: {
        ExpressionNode* castedExpressionNode = (ExpressionNode*)node;
        nextNodes.push_back(castedExpressionNode->left);
        nextNodes.push_back(castedExpressionNode->right);
        break;
    }
    case RelExpression: {
        RelExpressionNode* castedRelExpressionNode = (RelExpressionNode*)node;
        nextNodes.push_back(castedRelExpressionNode->left);
        nextNodes.push_back(castedRelExpressionNode->right);
        break;
    }
    case CondExpression: {
        CondExpressionNode* castedCondExpressionNode = (CondExpressionNode*)node;
        nextNodes.push_back(castedCondExpressionNode->left);
        nextNodes.push_back(castedCondExpressionNode->right);
        break;
    }
    case Assign: {
        AssignNode* castedAssignNode = (AssignNode*)node;
        nextNodes.push_back(castedAssignNode->var);
        nextNodes.push_back(castedAssignNode->expr);
        break;
    }
    case If: {
        IfNode* castedIfNode = (IfNode*)node;
        nextNodes.push_back(castedIfNode->cond);
        for (Node* n : castedIfNode->thenStmtLst) {
            nextNodes.push_back(n);
        }
        for (Node* n : castedIfNode->elseStmtLst) {
            nextNodes.push_back(n);
        }
        break;
    }
    case While: {
        WhileNode* castedWhileNode = (WhileNode*)node;
        nextNodes.push_back(castedWhileNode->cond);
        for (Node* n : castedWhileNode->stmtLst) {
            nextNodes.push_back(n);
        }
        break;
    }
    case Read: {
        ReadNode* castedReadNode = (ReadNode*)node;
        nextNodes.push_back(castedReadNode->var);
        break;
    }
    case Print: {
        PrintNode* castedPrintNode = (PrintNode*)node;
        nextNodes.push_back(castedPrintNode->var);
        break;
    }
    case Call: {
        CallNode* castedCallNode = (CallNode*)node;
        nextNodes.push_back(castedCallNode->proc);
        break;
    }
    case Procedure: {
        ProcedureNode* castedProcedureNode = (ProcedureNode*)node;
        for (Node* n : castedProcedureNode->stmtLst) {
            nextNodes.push_back(n);
        }
        break;
    }
    case Program: {
        ProgramNode* castedProgramNode = (ProgramNode*)node;
        for (Node* n : castedProgramNode->procedures) {
            nextNodes.push_back(n);
        }
        break;
    }
    }
    return nextNodes;
}


//currently can only take 1 function, can consider:vector of function pointers that is iterated over, or some other wrapper function thing
void ast::visit(Node* node, std::vector<std::vector<void (*)(Node* currentNode)>>functions) {
    // Execute the corresponding function taking the node as a parameter
    if (node) {
        for (void (*func) (Node* currentNode) : functions[node->kind]) {
            func(node);
        }
    }
    // Continue traversing the AST
    for (Node* n : nextNodes(node)) {
        if(n)visit(n, functions);
    }
}

void ast::visitWithAncestors(Node* node, std::vector<Node*>& ancestorList, std::vector<void (*) (Node* currentNode, std::vector<Node*>ancestorList)> functions) {
    functions[node->kind](node, ancestorList);
    ancestorList.push_back(node);
    for (Node* n : nextNodes(node)) {
        visitWithAncestors(n, ancestorList, functions);
    }
    ancestorList.pop_back();
}
