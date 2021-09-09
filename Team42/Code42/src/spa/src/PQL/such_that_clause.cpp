#include "such_that_clause.h"
#include "entity_declaration.h"

SuchThatClause::SuchThatClause(const std::string &type) {
    if (type == "Follows") {
        this->type_ = RelRef::Follows;
    } else if (type == "Follows*") {
        this->type_ = RelRef::FollowsT;
    } else if (type == "Parent") {
        this->type_ = RelRef::Parent;
    } else if (type == "Parent*") {
        this->type_ = RelRef::ParentT;
    } else if (type == "Uses") {
        this->type_ = RelRef::Uses;
    } else if (type == "Modifies") {
        this->type_ = RelRef::Modifies;
    } else {
        this->type_ = RelRef::None;
    }

    this->left_ref_ = nullptr;
    this->right_ref_ = nullptr;
}

bool SuchThatClause::set_ref(SuchThatRef *left, SuchThatRef *right) {
    switch (this->type_) {
      case RelRef::Follows:
      case RelRef::FollowsT:
      case RelRef::Parent:
      case RelRef::ParentT:
        if (left->get_type() == SuchThatRefType::Statement
        && right->get_type() == SuchThatRefType::Statement) {
            this->left_ref_ = left;
            this->right_ref_ = right;
            return true;
        }
      case RelRef::Uses:
        if (right->get_type() == SuchThatRefType::Entity) {
            if (left->get_type() == SuchThatRefType::Statement
                && left->get_stmt_ref().get_type()
                != StmtRefType::WildCard) {
                this->left_ref_ = left;
                this->right_ref_ = right;
                this->type_ = RelRef::UsesS;
                return true;
            } else if (left->get_type() == SuchThatRefType::Entity
            && left->get_ent_ref().get_type() != EntRefType::WildCard) {
                this->left_ref_ = left;
                this->right_ref_ = right;
                this->type_ = RelRef::UsesP;
                return true;
            }
        }
      case RelRef::Modifies:
        if (right->get_type() == SuchThatRefType::Entity) {
            if (left->get_type() == SuchThatRefType::Statement
                && left->get_stmt_ref().get_type()
                != StmtRefType::WildCard) {
                this->left_ref_ = left;
                this->right_ref_ = right;
                this->type_ = RelRef::ModifiesS;
                return true;
            } else if (left->get_type() == SuchThatRefType::Entity
            && left->get_ent_ref().get_type() != EntRefType::WildCard) {
                this->left_ref_ = left;
                this->right_ref_ = right;
                this->type_ = RelRef::ModifiesP;
                return true;
            }
        }
      default:
        break;
    }
    return false;
}

void SuchThatClause::set_type(RelRef rel) {
    this->type_ = rel;
}

RelRef SuchThatClause::get_type() {
    return this->type_;
}

std::string SuchThatClause::get_type_str() {
    switch (this->type_) {
      case RelRef::Follows:
        return "Follows";
      case RelRef::FollowsT:
        return "Follows*";
      case RelRef::Parent:
        return "Parent";
      case RelRef::ParentT:
        return "Parent*";
      case RelRef::Uses:
      case RelRef::UsesP:
      case RelRef::UsesS:
        return "Uses";
      case RelRef::Modifies:
      case RelRef::ModifiesP:
      case RelRef::ModifiesS:
        return "Modifies";
      default:
        return "Unknown Type";
    }
}

SuchThatRef *SuchThatClause::get_left_ref() {
    return this->left_ref_;
}

SuchThatRef *SuchThatClause::get_right_ref() {
    return this->right_ref_;
}
