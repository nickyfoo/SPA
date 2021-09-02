#include "Relationship.h"

Relationship::Relationship(std::string type)
{
    if (type == "Follows") {
        this->type = RelationshipType::Follows;
    } else if (type == "Follows*") {
        this->type = RelationshipType::FollowsStar;
    } else if (type == "Parent") {
        this->type = RelationshipType::Parent;
    } else if (type == "Parent*") {
        this->type = RelationshipType::ParentStar;
//    } else if (type == "Uses") {
//        this->type = RelationshipType::Uses;
//    } else if (type == "Modifies") {
//        this->type = RelationshipType::Modifies;
    } else {
        this->type = RelationshipType::None;
    }

}

bool Relationship::setRef(Entity* left_ref, Entity* right_ref)
{
    switch(this->type) {
        case RelationshipType::Follows:
            if ((left_ref->getType() == EntityType::Stmt || left_ref->isStmtNum())
                    && (right_ref->getType() == EntityType::Stmt || right_ref->isStmtNum())) {
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        case RelationshipType::FollowsStar:
            if ((left_ref->getType() == EntityType::Stmt || left_ref->isStmtNum())
                    && (right_ref->getType() == EntityType::Stmt || right_ref->isStmtNum())) {
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        case RelationshipType::Parent:
            if ((left_ref->getType() == EntityType::Stmt || left_ref->isStmtNum())
                    && (right_ref->getType() == EntityType::Stmt || right_ref->isStmtNum())) {
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        case RelationshipType::ParentStar:
            if ((left_ref->getType() == EntityType::Stmt || left_ref->isStmtNum())
                    && (right_ref->getType() == EntityType::Stmt || right_ref->isStmtNum())) {
                this->left_ref = left_ref;
                this->right_ref = right_ref;
                return true;
            }
        default:
            return false;
        return false;
    }
}

RelationshipType Relationship::getType() {
    return this->type;
}

string Relationship::getTypeStr() {
    switch (this->type) {
        case RelationshipType::Follows:
            return "Follows";
        case RelationshipType::FollowsStar:
            return "Follows*";
        case RelationshipType::Parent:
            return "Parent";
        case RelationshipType::ParentStar:
            return "Parent*";
        default:
            return "Unknown Type";
    }
}

Entity* Relationship::getLeftRef() {
    return this->left_ref;
}

Entity* Relationship::getRightRef() {
    return this->right_ref;
}