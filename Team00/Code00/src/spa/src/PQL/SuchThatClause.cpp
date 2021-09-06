#include "SuchThatClause.h"
#include "EntityDeclaration.h"

#

SuchThatClause::SuchThatClause(const std::string& type)
{
    if (type == "Follows") {
        this->type = RelRef::Follows;
    } else if (type == "Follows*") {
        this->type = RelRef::FollowsT;
    } else if (type == "Parent") {
        this->type = RelRef::Parent;
    } else if (type == "Parent*") {
        this->type = RelRef::ParentT;
    } else if (type == "Uses") {
        this->type = RelRef::Uses;
    } else if (type == "Modifies") {
        this->type = RelRef::Modifies;
    } else {
        this->type = RelRef::None;
    }

    this->left_ref = nullptr;
    this->right_ref = nullptr;

}

bool SuchThatClause::setRef(SuchThatRef* left, SuchThatRef* right) {
    switch(this->type) {
        case RelRef::Follows: case RelRef::FollowsT: case RelRef::Parent: case RelRef::ParentT:
            if (left->getType() == SuchThatRefType::Statement && right->getType() == SuchThatRefType::Statement) {
                this->left_ref = left;
                this->right_ref = right;
                return true;
            }
        case RelRef::Uses:
            if (right->getType() == SuchThatRefType::Entity) {
                if (left->getType() == SuchThatRefType::Statement
                    && left->getStmtRef().getType() != StmtRefType::WildCard) {
                    this->left_ref = left;
                    this->right_ref = right;
                    this->type = RelRef::UsesS;
                    return true;
                } else if (left->getType() == SuchThatRefType::Entity && left->getEntRef().getType() != EntRefType::WildCard) {
                    this->left_ref = left;
                    this->right_ref = right;
                    this->type = RelRef::UsesP;
                    return true;
                }
            }
        case RelRef::Modifies:
            if (right->getType() == SuchThatRefType::Entity) {
                if (left->getType() == SuchThatRefType::Statement
                    && left->getStmtRef().getType() != StmtRefType::WildCard) {
                    this->left_ref = left;
                    this->right_ref = right;
                    this->type = RelRef::ModifiesS;
                    return true;
                } else if (left->getType() == SuchThatRefType::Entity && left->getEntRef().getType() != EntRefType::WildCard) {
                    this->left_ref = left;
                    this->right_ref = right;
                    this->type = RelRef::ModifiesP;
                    return true;
                }
            }
        default:
            break;
    }
    return false;
}

void SuchThatClause::setType(RelRef rel) {
    this->type = rel;
}

RelRef SuchThatClause::getType() {
    return this->type;
}

std::string SuchThatClause::getTypeStr() {
    switch (this->type) {
        case RelRef::Follows:
            return "Follows";
        case RelRef::FollowsT:
            return "Follows*";
        case RelRef::Parent:
            return "Parent";
        case RelRef::ParentT:
            return "Parent*";
        case RelRef::Uses: case RelRef::UsesP: case RelRef::UsesS:
            return "Uses";
        case RelRef::Modifies: case RelRef::ModifiesP: case RelRef::ModifiesS:
            return "Modifies";
        default:
            return "Unknown Type";
    }
}

SuchThatRef* SuchThatClause::getLeftRef() {
    return this->left_ref;
}

SuchThatRef* SuchThatClause::getRightRef() {
    return this->right_ref;
}