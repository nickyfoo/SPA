#include "calls_handler.h"
#include "entity_declaration.h"
#include "follows_parent_handler.h"
#include "next_handler.h"
#include "procedure.h"
#include "relationship_query_manager.h"
#include "statement.h"
#include "usesp_modifiesp_handler.h"
#include "usess_modifiess_handler.h"
#include "variable.h"
#include "affects_handler.h"

RelationshipQueryManager::RelationshipQueryManager(PKB *pkb) { this->pkb_ = pkb; }

RelationshipQueryManager::~RelationshipQueryManager() = default;

ResultTable *RelationshipQueryManager::EvaluateRelationship(
    std::shared_ptr<SuchThatClause> relationship,
    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  // Iterating through relationships_ and evaluating one by one.
  // For basic requirements, there will be only 1 relationship_.
  RelRef relationship_type = relationship->get_type();
  switch (relationship_type) {
    case RelRef::Follows: {
      FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
      follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                     &Statement::get_followees);
      follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return follows_parents_handler->Evaluate();
    }
    case RelRef::FollowsT: {
      FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
      follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                     &Statement::get_followees_star);
      follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return follows_parents_handler->Evaluate();
    }
    case RelRef::Parent: {
      FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
      follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                     &Statement::get_parents);
      follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return follows_parents_handler->Evaluate();
    }
    case RelRef::ParentT: {
      FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
      follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                     &Statement::get_parents_star);
      follows_parents_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return follows_parents_handler->Evaluate();
    }
    case RelRef::UsesS: {
      UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
      uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                   &Variable::get_stmts_using);
      uses_modifies_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return uses_modifies_handler->Evaluate();
    }
    case RelRef::ModifiesS: {
      UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
      uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                   &Variable::get_stmts_modifying);
      uses_modifies_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return uses_modifies_handler->Evaluate();
    }
    case RelRef::UsesP: {
      UsesPModifiesPHandler *usesp_modifiesp_handler = UsesPModifiesPHandler::get_instance();
      usesp_modifiesp_handler->set_function_pointers(&Procedure::get_uses,
                                                     &Variable::get_procs_using);
      usesp_modifiesp_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return usesp_modifiesp_handler->Evaluate();
    }
    case RelRef::ModifiesP: {
      UsesPModifiesPHandler *usesp_modifiesp_handler = UsesPModifiesPHandler::get_instance();
      usesp_modifiesp_handler->set_function_pointers(&Procedure::get_modifies,
                                                     &Variable::get_procs_modifying);
      usesp_modifiesp_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return usesp_modifiesp_handler->Evaluate();
    }
    case RelRef::Next: {
      NextHandler *next_handler = NextHandler::get_instance();
      next_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return next_handler->EvaluateNext();
    }
    case RelRef::NextT: {
      NextHandler *next_handler = NextHandler::get_instance();
      next_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return next_handler->EvaluateNextT();
    }
    case RelRef::Calls: {
      CallsHandler *calls_handler = CallsHandler::get_instance();
      calls_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return calls_handler->EvaluateCalls();
    }
    case RelRef::CallsT: {
      CallsHandler *calls_handler = CallsHandler::get_instance();
      calls_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return calls_handler->EvaluateCallsT();
    }
    case RelRef::Affects: {
      AffectsHandler *affects_handler = AffectsHandler::get_instance();
      affects_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return affects_handler->EvaluateAffects();
    }
    case RelRef::AffectsT: {
      AffectsHandler *affects_handler = AffectsHandler::get_instance();
      affects_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return affects_handler->EvaluateAffectsT();
    }
    case RelRef::NextBip: {
      NextHandler *next_handler = NextHandler::get_instance();
      next_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return next_handler->EvaluateNextBip();
    }
    case RelRef::NextTBip: {
      NextHandler *next_handler = NextHandler::get_instance();
      next_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return next_handler->EvaluateNextTBip();
    }
    case RelRef::AffectsBip: {
      AffectsHandler *affects_handler = AffectsHandler::get_instance();
      affects_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return affects_handler->EvaluateAffectsBip();
    }
    case RelRef::AffectsTBip: {
      AffectsHandler *affects_handler = AffectsHandler::get_instance();
      affects_handler->set_args(pkb_, relationship, synonym_to_entities_vec);
      return affects_handler->EvaluateAffectsTBip();
    }
    default:
      break;
  }
  return nullptr;
}
