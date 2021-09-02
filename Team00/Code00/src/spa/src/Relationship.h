#pragma once
#include <string>
#include "Entity.h"

enum class RelationshipType {
    Follows,
    FollowsT,
    Parent,
    ParentT,
    Uses,
    Modifies,
    None
};

class Relationship {
public:
    Relationship(string type);
    bool setRef(Entity* left_ref, Entity* right_ref);
    RelationshipType getType();
    string getTypeStr();
    Entity* getLeftRef();
    Entity* getRightRef();
private:
    RelationshipType type;
    Entity* left_ref;
    Entity* right_ref;
};