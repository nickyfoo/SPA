#pragma once
#include <string>
#include "Entity.h"

enum class RelationshipType {
    Follows,
    FollowsStar,
    Parent,
    ParentStar,
    Uses,
    Modifies,
    None
};

class Relationship {
public:
    Relationship(string type);
    bool setRef(Entity* left_ref, Entity* right_ref);
    RelationshipType getType();
    Entity* getLeftRef();
    Entity* getRightRef();
private:
    RelationshipType type;
    Entity* left_ref;
    Entity* right_ref;
};