#include "Relationship.h"

Relationship::Relationship(const std::string& type)
{
    if (type == "Follows") {
        this->type = RelationshipType::Follows;
    } else if (type == "Follows*") {
        this->type = RelationshipType::FollowsT;
    } else if (type == "Parent") {
        this->type = RelationshipType::Parent;
    } else if (type == "Parent*") {
        this->type = RelationshipType::ParentT;
    } else if (type == "Uses") {
        this->type = RelationshipType::Uses;
    } else if (type == "Modifies") {
        this->type = RelationshipType::Modifies;
    } else {
        this->type = RelationshipType::None;
    }

    this->left_ref = nullptr;
    this->right_ref = nullptr;

}

bool Relationship::setRef(EntityDeclaration* left_ref, EntityDeclaration* right_ref)
{
    switch(this->type) {
        case RelationshipType::Follows: case RelationshipType::FollowsT:
            case RelationshipType::Parent: case RelationshipType::ParentT:
            if ((left_ref->getType() == EntityType::Assign || left_ref->getType() == EntityType::Print ||
            left_ref->getType() == EntityType::If || left_ref->getType() == EntityType::While ||
            left_ref->getType() == EntityType::Call || left_ref->getType() == EntityType::Stmt ||
            left_ref->isStmtNum() || left_ref->isWildCard())
            && (right_ref->getType() == EntityType::Assign || right_ref->getType() == EntityType::Print ||
            right_ref->getType() == EntityType::If || right_ref->getType() == EntityType::While ||
            right_ref->getType() == EntityType::Call || right_ref->getType() == EntityType::Stmt ||
            right_ref->isStmtNum() || right_ref->isWildCard())) {
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        case RelationshipType::Uses: case RelationshipType::Modifies:
            if ((left_ref->getType() == EntityType::Assign || left_ref->getType() == EntityType::Print ||
                    left_ref->getType() == EntityType::If || left_ref->getType() == EntityType::While ||
                    left_ref->getType() == EntityType::Procedure || left_ref->getType() == EntityType::Call ||
                    left_ref->getType() == EntityType::Stmt || left_ref->getType() == EntityType::Argument)
                    && (right_ref->getType() == EntityType::Variable || right_ref->getType() == EntityType::Argument)) {
                if (left_ref->getValue() == "_" || right_ref->isStmtNum()) {
                    return false;
                }
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        default:
            return false;
    }
}

RelationshipType Relationship::getType() {
    return this->type;
}

std::string Relationship::getTypeStr() {
    switch (this->type) {
        case RelationshipType::Follows:
            return "Follows";
        case RelationshipType::FollowsT:
            return "Follows*";
        case RelationshipType::Parent:
            return "Parent";
        case RelationshipType::ParentT:
            return "Parent*";
        case RelationshipType::Uses:
            return "Uses";
        case RelationshipType::Modifies:
            return "Modifies";
        default:
            return "Unknown Type";
    }
}

EntityDeclaration* Relationship::getLeftRef() {
    return this->left_ref;
}

EntityDeclaration* Relationship::getRightRef() {
    return this->right_ref;
}