#include "relationship_query_manager.h"
#include "entity_declaration.h"
#include "statement.h"
#include "follows_parents_handler.h"
#include "usess_modifiess_handler.h"
#include "usesp_modifiesp_handler.h"
#include "variable.h"
#include "procedure.h"

RelationshipQueryManager::RelationshipQueryManager(PKB *pkb) {
  this->pkb_ = pkb;
}

RelationshipQueryManager::~RelationshipQueryManager() = default;

ResultTable* RelationshipQueryManager::EvaluateRelationship(std::shared_ptr<SuchThatClause> relationship,
                                                            std::unordered_map<std::string,
                                                                                std::vector<Entity *>> synonym_to_entities_vec) {
  // Iterating through relationships_ and evaluating one by one.
  // For basic requirements, there will be only 1 relationship_.
//  for (SuchThatClause *relationship : *relationships_) {
    printf("3\n");
    RelRef relationship_type = relationship->get_type();
    switch (relationship_type) {
      case RelRef::Follows: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                       &Statement::get_followees);
        follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
        return follows_parents_handler->Evaluate();
      }
      case RelRef::FollowsT: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                       &Statement::get_followees_star);
        follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
        return follows_parents_handler->Evaluate();
      }
      case RelRef::Parent: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                       &Statement::get_parents);
        follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
        return follows_parents_handler->Evaluate();
      }
      case RelRef::ParentT: {
        FollowsParentsHandler *follows_parents_handler =
            FollowsParentsHandler::get_instance();
        follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                       &Statement::get_parents_star);
        follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
        return follows_parents_handler->Evaluate();
      }
      case RelRef::UsesS: {
        UsesSModifiesSHandler *uses_modifies_handler =
            UsesSModifiesSHandler::get_instance();
        uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                     &Variable::get_stmts_using);
        uses_modifies_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
        return uses_modifies_handler->Evaluate();
      }
      case RelRef::ModifiesS: {
        UsesSModifiesSHandler *uses_modifies_handler =
            UsesSModifiesSHandler::get_instance();
        uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                     &Variable::get_stmts_modifying);
        uses_modifies_handler->set_args(pkb_,
                                        relationship,
                                        synonym_to_entities_vec);
        return uses_modifies_handler->Evaluate();
      }
      case RelRef::UsesP: {
        UsesPModifiesPHandler *usesp_modifiesp_handler =
            UsesPModifiesPHandler::get_instance();
        usesp_modifiesp_handler->set_function_pointers(&Procedure::get_uses,
                                                       &Variable::get_procs_using);
        usesp_modifiesp_handler->set_args(pkb_,
                                          relationship,
                                          synonym_to_entities_vec);
        return usesp_modifiesp_handler->Evaluate();
      }
      case RelRef::ModifiesP: {
        printf("MIHERE\n");
        UsesPModifiesPHandler *usesp_modifiesp_handler =
            UsesPModifiesPHandler::get_instance();
        usesp_modifiesp_handler->set_function_pointers(&Procedure::get_modifies,
                                                       &Variable::get_procs_modifying);
        usesp_modifiesp_handler->set_args(pkb_,
                                          relationship,
                                          synonym_to_entities_vec);
        return usesp_modifiesp_handler->Evaluate();
      }
      default:
        break;
    }
    return nullptr;
//  }
}
