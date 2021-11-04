#pragma once

#include <pkb.h>

class DesignExtractor {
 public:
  DesignExtractor(PKB *pkb, Node *program_root);

  ~DesignExtractor();

  // Populates the tables with entities and relationships from AST.
  void ExtractDesigns();

 private:
  // Extracts entities from the AST root node and stores them in the pkb.
  void ExtractEntities();
  // Extracts Follows/Follows_Star relationships in the AST.
  void ExtractFollows();
  // Extracts Parent/Parent_Star relationships in the AST.
  void ExtractParent();
  // Extracts direct Uses relationships in the AST.
  void ExtractUsesModifies();
  // Extracts Calls/Calls* relationships in the AST.
  void ExtractCalls();
  // Stores adjacency list into cfg_al_
  void ExtractCFG();

  // Updates procs_using_ and procs_modifying_ in var_table_.
  void UpdateVarTableWithProcs();

  // Process and store Follows relationships for the AST procedure node.
  void FollowsProcessProcedureNode(Node *node);
  // Process and store Follows relationships for the AST if node.
  void FollowsProcessIfNode(Node *node);
  // Process and store Follows relationships for the AST while node.
  void FollowsProcessWhileNode(Node *node);

  // Process and store Parent relationships for the AST if node.
  void ParentProcessIfNode(Node *node);
  // Process and store Parent relationships for the AST while node.
  void ParentProcessWhileNode(Node *node);

  // Process and store Uses/Modifies relationships for the AST assign node.
  void UsesModifiesProcessAssignNode(Node *node, std::vector<Node *> &ancestors);
  // Process and store Uses/Modifies relationships for the AST if/while node.
  void UsesModifiesProcessContainerNode(Node *node, std::vector<Node *> &ancestors);
  // Process and store Uses/Modifies relationships for the AST read node.
  void UsesModifiesProcessReadNode(Node *node, std::vector<Node *> &ancestors);
  // Process and store Uses/Modifies relationships for the AST print node.
  void UsesModifiesProcessPrintNode(Node *node, std::vector<Node *> &ancestors);

  // Process and store Calls relationships for the AST call node.
  void CallsProcessCallNode(Node *node, std::vector<Node *> &ancestors);

  // Recursively gets the last stmts of a statement.
  std::set<int> GetLastStmts(StatementNode *node);
  // Recursively gets the last stmts of a procedure starting at first_stmt.
  void LastStmtsOfProcedure(int u, std::set<int> &visited, std::set<int> &ans);
  // Process and store the AST procedure node into the CFG.
  void CFGProcessProcedureNode(Node *node);
  // Process and store the AST if node into the CFG.
  void CFGProcessIfNode(Node *node);
  // Process and store the AST while node into the CFG.
  void CFGProcessWhileNode(Node *node);
  // Adds missing links between call stmts and procedures being called
  void LinkProcedures();
  // Adds missing interprocedure return links
  void AddInterprocedureReturnLinks(std::set<std::string>&visited,std::set<int>& last_stmts, std::string& proc_name);
  // Adds a call stack to each assign stmt, to store the state that it can be in when encountered.
  void AddCallStacks();
  // Traverses the CFGBip, keeping track of the call stack
  void ProcessCallStacks(std::set<std::pair<int, std::string>> &visited,
                         std::vector<int> &call_stack, int u);

  PKB *pkb_;
  // Root AST node of the program.
  Node *root_;
};
