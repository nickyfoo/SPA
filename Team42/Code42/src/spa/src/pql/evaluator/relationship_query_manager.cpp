#include "relationship_query_manager.h"
#include "entity_declaration.h"
#include "statement.h"
#include "follows_parents_handler.h"
#include "usess_modifiess_handler.h"
#include "usesp_modifiesp_handler.h"
#include "variable.h"
#include "procedure.h"

RelationshipQueryManager::RelationshipQueryManager(
    std::unordered_map<std::string, std::vector<Entity *>>
    *synonym_to_entity_result,
    std::vector<SuchThatClause *> *relationships,
    std::vector<std::string> *entities_to_return,
    PKB *pkb) {
  this->synonym_to_entity_result_ = synonym_to_entity_result;
  this->relationships_ = relationships;
  this->entities_to_return_ = entities_to_return;
  this->pkb_ = pkb;
}

void RelationshipQueryManager::EvaluateRelationships() {
  // Iterating through relationships_ and evaluating one by one.
  // For basic requirements, there will be only 1 relationship_.
  for (SuchThatClause *relationship : *relationships_) {
    RelRef relationship_type = relationship->get_type();
    switch (relationship_type) {
      case RelRef::Follows: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                       &Statement::get_followees);
        follows_parents_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        follows_parents_handler->Evaluate();
        break;
      }
      case RelRef::FollowsT: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                       &Statement::get_followees_star);
        follows_parents_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        follows_parents_handler->Evaluate();
        break;
      }
      case RelRef::Parent: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                       &Statement::get_parents);
        follows_parents_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        follows_parents_handler->Evaluate();
        break;
      }
      case RelRef::ParentT: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                       &Statement::get_parents_star);
        follows_parents_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        follows_parents_handler->Evaluate();
        break;
      }
      case RelRef::UsesS: {
        UsesSModifiesSHandler *uses_modifies_handler =
            UsesSModifiesSHandler::get_instance();
        uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                     &Variable::get_stmts_using);
        uses_modifies_handler->set_args(pkb_,
                                        synonym_to_entity_result_,
                                        relationship,
                                        entities_to_return_);
        uses_modifies_handler->Evaluate();
        break;
      }
      case RelRef::ModifiesS: {
        UsesSModifiesSHandler *uses_modifies_handler =
            UsesSModifiesSHandler::get_instance();
        uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                     &Variable::get_stmts_modifying);
        uses_modifies_handler->set_args(pkb_,
                                        synonym_to_entity_result_,
                                        relationship,
                                        entities_to_return_);
        uses_modifies_handler->Evaluate();
        break;
      }
      case RelRef::UsesP: {
        UsesPModifiesPHandler *usesp_modifiesp_handler =
            UsesPModifiesPHandler::get_instance();
        usesp_modifiesp_handler->set_function_pointers(&Procedure::get_uses,
                                                       &Variable::get_procs_using);
        usesp_modifiesp_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        usesp_modifiesp_handler->Evaluate();
        break;
      }
      case RelRef::ModifiesP: {
        UsesPModifiesPHandler *usesp_modifiesp_handler =
            UsesPModifiesPHandler::get_instance();
        usesp_modifiesp_handler->set_function_pointers(&Procedure::get_modifies,
                                                       &Variable::get_procs_modifying);
        usesp_modifiesp_handler->set_args(pkb_,
                                          synonym_to_entity_result_,
                                          relationship,
                                          entities_to_return_);
        usesp_modifiesp_handler->Evaluate();
        break;
      }
      default:break;
    }
  }
}
