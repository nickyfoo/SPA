#include <sstream>
#include "query_evaluator.h"
#include "ast.h"  // Get kind enum
#include "statement.h"
#include "procedure.h"
#include "variable.h"
#include "pattern_query_manager.h"
#include "relationship_query_manager.h"
#include "with_query_manager.h"
#include "pql_query.h"
#include "pkb.h"

QueryEvaluator::QueryEvaluator(PQLQuery *pql_query, PKB *pkb) {
  if (pql_query != nullptr) {
    this->entities_to_return_ = pql_query->get_query_entities();
    this->synonym_to_entity_dec_ = pql_query->get_synonym_to_entities();
    this->is_syntactically_valid_ = pql_query->is_syntactically_valid();
    this->is_semantically_valid_ = pql_query->is_semantically_valid();
    this->pkb_ = pkb;
    if (pql_query->is_syntactically_valid() && pql_query->is_semantically_valid()) {
      this->clause_groups_ = pql_query->get_clause_groups();
      set_used_synonyms();
    }
  } else {
    this->entities_to_return_ = nullptr;
    this->synonym_to_entity_dec_ = nullptr;
    this->is_syntactically_valid_ = false;
    this->is_semantically_valid_ = false;
    this->pkb_ = nullptr;
  }
}

std::vector<std::string> *QueryEvaluator::Evaluate() {
  if (!is_syntactically_valid_ || !is_semantically_valid_) {  // if not valid query
    if (entities_to_return_ != nullptr &&
    entities_to_return_->size() == 1 &&
    entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean) {
      if (is_syntactically_valid_) {
        return new std::vector<std::string>{"FALSE"};
      }
    }
    return new std::vector<std::string>{};
  }
  RelationshipQueryManager *relationship_query_manager;
  PatternQueryManager *pattern_query_manager;
  WithQueryManager *with_query_manager;
  ResultTable *result_table = new ResultTable();
  relationship_query_manager = new RelationshipQueryManager(pkb_);
  pattern_query_manager = new PatternQueryManager(pkb_);
  with_query_manager = new WithQueryManager();

  // guaranteed to have at least 3 clause groups,
  // where first group is without synonyms,
  // second group is without any return synonyms,
  // and third groups on are those with synonyms in results
  for (int i = 0; i < clause_groups_.size(); i++) {
    std::vector<ClauseVertex> clause_vertexes = clause_groups_.at(i)->get_clauses();
    ResultTable *intermediate_table = new ResultTable();
    bool first_table_entry = true;
    for (ClauseVertex clause_vertex : clause_vertexes) {
      ClauseType type = clause_vertex.get_clause()->get_type();
      auto synonym_to_entities_vec = GetPossibleEntitiesVec(clause_vertex);
      bool has_synonyms = !clause_vertex.get_synonyms_used().empty();
      ResultTable *table;
      switch (type) {
        case ClauseType::SuchThatClause:
          table = relationship_query_manager->EvaluateRelationship(
              std::dynamic_pointer_cast<SuchThatClause>(
                  clause_vertex.get_clause()), synonym_to_entities_vec);
          break;
        case ClauseType::PatternClause:
          table = pattern_query_manager->EvaluatePattern(std::dynamic_pointer_cast<PatternClause>(
              clause_vertex.get_clause()), synonym_to_entities_vec);
          break;
        case ClauseType::WithClause:
          table = with_query_manager->EvaluateWith(std::dynamic_pointer_cast<WithClause>(
              clause_vertex.get_clause()), synonym_to_entities_vec);
          break;
        default:throw std::runtime_error("Unknown ClauseType found");
      }
      // if a nullptr is received, means that clause evaluates to false or empty results
      if (table == nullptr) {
        return ConvertToOutput(result_table, false);
      }
      if (first_table_entry && has_synonyms) {
        intermediate_table->set_table(*table);
        first_table_entry = false;
      } else if (has_synonyms) {
        intermediate_table->NaturalJoin(*table);
      }
    }
    if (i == 1) {  // first two groups which only has to evaluate true / false
      if (intermediate_table->get_table()->size() == 0 && !first_table_entry) {
        return ConvertToOutput(result_table, false);
      }
    } else if (i == 2) {  // first group when the result table is initially empty
      if (!first_table_entry && intermediate_table->get_table()->empty()) {  // found a clause
        return ConvertToOutput(result_table, false);
      }
      result_table->set_table(*intermediate_table);
    } else if (i > 2) {
      result_table->CrossJoin(*intermediate_table, used_synonyms_);
      if (!first_table_entry && result_table->get_table()->empty()) {
        return ConvertToOutput(result_table, false);
      }
    }
  }
  return ConvertToOutput(result_table, true);
}

std::unordered_map<std::string, std::vector<Entity *>>
QueryEvaluator::GetPossibleEntitiesVec(ClauseVertex clause_vertex) {
  std::vector<std::string> synonyms_used = clause_vertex.get_synonyms_used();
  std::unordered_map<std::string, std::vector<Entity *>> ret;
  for (std::string synonym : synonyms_used) {
    EntityType type = synonym_to_entity_dec_->at(synonym)->get_type();
    std::vector<Entity *> entities;
    switch (type) {
      case EntityType::ProgLine:
      case EntityType::Stmt: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_all_statements();
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Read: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Read);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Print: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Print);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Call: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Call);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::While: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::While);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::If: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::If);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Assign: {
        std::vector<Statement *> entities_stmt;
        entities_stmt = pkb_->get_statements(NodeType::Assign);
        for (Statement *stmt : entities_stmt) {
          auto *entity = static_cast<Entity *>(stmt);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Variable: {
        std::vector<Variable *> entities_var;
        entities_var = pkb_->get_all_variables();
        for (Variable *var : entities_var) {
          auto *entity = static_cast<Entity *>(var);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Constant: {
        std::vector<Constant *> entities_const;
        entities_const = pkb_->get_all_constants();
        for (Constant *cons : entities_const) {
          auto *entity = static_cast<Entity *>(cons);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::Procedure: {
        std::vector<Procedure *> entities_proc;
        entities_proc = pkb_->get_all_procedures();
        for (Procedure *proc : entities_proc) {
          auto *entity = static_cast<Entity *>(proc);
          entities.push_back(entity);
        }
        break;
      }
      case EntityType::None:
        throw std::runtime_error("Unknown EntityType!");
    }
    ret.insert({synonym, entities});
  }
  return ret;
}

// Converting from vector Entity objects to vector of strings to output.
std::vector<std::string>
*QueryEvaluator::ConvertToOutput(ResultTable *table_result, bool is_valid_query) {
  auto *output = new std::vector<std::string>();
  if (!is_valid_query) {
    if (entities_to_return_->size() == 1
    && entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean) {
      output->push_back("FALSE");
    }
    return output;
  }

  if (entities_to_return_->size() == 1
  && entities_to_return_->at(0)->get_return_type() == ReturnType::Boolean) {
    output->push_back("TRUE");
    return output;
  }
  std::vector<int> indexes_of_return_entities;
  auto synonym_to_index_map = table_result->get_synonym_to_index();
  auto index_to_synonym_map = table_result->get_index_to_synonym();
  // first loop to detect all synonyms in return entities that were not in clauses
  for (ResultClause *result_clause : *entities_to_return_) {
    std::string elem = result_clause->get_elem();
    std::string synonym = result_clause->get_synonym();
    // use the exact result as key

    if (synonym_to_index_map->find(elem) == synonym_to_index_map->end()) {

      ResultTable *new_table = new ResultTable();
      // synonym was used previously,
      if (synonym_to_index_map->find(synonym) != synonym_to_index_map->end()) {
        MakeTableForUsedEntity(new_table, result_clause, table_result);
      } else {
        MakeTableForUnusedEntity(new_table, result_clause);
      }
      // empty result for the result clause
      if (new_table->get_table()->empty()) {
        return output;
      }
      if (table_result->get_table()->empty()) {
        table_result->set_table(*new_table);
        synonym_to_index_map = new_table->get_synonym_to_index();
        index_to_synonym_map = new_table->get_index_to_synonym();
      } else {
        table_result->NaturalJoin(*new_table);
      }
    }
  }
  // second loop to retrieve synonym indexes
  for (ResultClause *result_clause : *entities_to_return_) {
    indexes_of_return_entities.push_back(synonym_to_index_map->at(result_clause->get_elem()));
  }
  std::set<std::string> unique_results;
  for (std::vector<std::string> row : *table_result->get_table()) {
    std::stringstream ss;
    for (int i = 0; i < indexes_of_return_entities.size(); i++) {
      int index = indexes_of_return_entities.at(i);
      ss << row.at(index);
      if (i != indexes_of_return_entities.size() - 1) {
        ss << " ";
      }
    }
    unique_results.insert(ss.str());
  }
  output->assign(unique_results.begin(), unique_results.end());
  return output;
}

void QueryEvaluator::MakeTableForUnusedEntity(ResultTable *result_table,
                                              ResultClause *result_clause) {
  std::string elem = result_clause->get_elem();
  EntityType synonym_type = result_clause->get_synonym_type();
  ReturnType return_type = result_clause->get_return_type();
  std::vector<std::string> to_add = std::vector<std::string>();

  switch (synonym_type) {
    case EntityType::ProgLine:
    case EntityType::Stmt: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_all_statements();
      for (Statement *stmt : entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Read: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Read);
      for (Statement *stmt : entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          for (auto &i : *stmt->get_modifies()) {
            to_add.push_back(i);
          }
        }
      }
      break;
    }
    case EntityType::Print: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Print);
      for (Statement *stmt : entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          for (auto &i : *stmt->get_uses()) {
            to_add.push_back(i);
          }
        }
      }
      break;
    }
    case EntityType::Call: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Call);
      for (Statement *stmt : entities_stmt) {
        if (return_type == ReturnType::Default || return_type == ReturnType::Integer) {
          to_add.push_back(std::to_string(stmt->get_stmt_no()));
        } else {
          to_add.push_back(stmt->get_called_proc_name());
        }
      }
      break;
    }
    case EntityType::While: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::While);
      for (Statement *stmt : entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::If: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::If);
      for (Statement *stmt : entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Assign: {
      std::vector<Statement *> entities_stmt;
      entities_stmt = pkb_->get_statements(NodeType::Assign);
      for (Statement *stmt : entities_stmt) {
        to_add.push_back(std::to_string(stmt->get_stmt_no()));
      }
      break;
    }
    case EntityType::Variable: {
      std::vector<Variable *> entities_var;
      entities_var = pkb_->get_all_variables();
      for (Variable *var : entities_var) {
        to_add.push_back(var->get_name());
      }
      break;
    }
    case EntityType::Constant: {
      std::vector<Constant *> entities_const;
      entities_const = pkb_->get_all_constants();
      for (Constant *cons : entities_const) {
        to_add.push_back(cons->get_value());
      }
      break;
    }
    case EntityType::Procedure: {
      std::vector<Procedure *> entities_proc;
      entities_proc = pkb_->get_all_procedures();
      for (Procedure *proc : entities_proc) {
        to_add.push_back(proc->get_name());
      }
      break;
    }
    case EntityType::None:
      throw std::runtime_error("Unknown EntityType!");
  }
  result_table->AddSingleColumn(elem, to_add);
}


void QueryEvaluator::MakeTableForUsedEntity(ResultTable *result_table,
                                            ResultClause *result_clause,
                                            ResultTable *other_result_table) {
  std::vector<std::string> synonym_vec = other_result_table->GetColumnVec(
      result_clause->get_synonym());

  // removing duplicates in the column vec, since we are doing natural join later
  sort(synonym_vec.begin(), synonym_vec.end());
  synonym_vec.erase( unique( synonym_vec.begin(), synonym_vec.end() ), synonym_vec.end() );

  std::string elem = result_clause->get_elem();
  EntityType synonym_type = result_clause->get_synonym_type();
  ReturnType return_type = result_clause->get_return_type();
  std::vector<std::string> to_add = std::vector<std::string>();

  for (std::string synonym : synonym_vec) {
    switch (synonym_type) {
      case EntityType::ProgLine:
      case EntityType::While:
      case EntityType::If:
      case EntityType::Assign:
      case EntityType::Constant:
      case EntityType::Stmt: {  // guaranteed to be Integer
        to_add.push_back(synonym);
        break;
      }
      case EntityType::Read: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          for (auto &i : *stmt->get_modifies()) {
            to_add.push_back(i);
          }
        }
        break;
      }
      case EntityType::Print: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          for (auto &i : *stmt->get_uses()) {
            to_add.push_back(i);
          }
        }
        break;
      }
      case EntityType::Call: {
        if (return_type == ReturnType::Integer) {
          to_add.push_back(synonym);
        } else if (return_type == ReturnType::Name) {
          Statement *stmt = pkb_->get_statement(stoi(synonym));
          to_add.push_back(stmt->get_called_proc_name());
        }
        break;
      }
      case EntityType::Variable:
      case EntityType::Procedure: {  // guaranteed to be Name
        to_add.push_back(synonym);
        break;
      }
      case EntityType::None:
        throw std::runtime_error("Unknown EntityType!");
    }
  }

  result_table->AddDoubleColumns(result_clause->get_synonym(),
                                 synonym_vec, elem, to_add);
}

void QueryEvaluator::set_used_synonyms() {
  for (ResultClause* result_clause : *entities_to_return_) {
    used_synonyms_.insert(result_clause->get_synonym());
  }
}
