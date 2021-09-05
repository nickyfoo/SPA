#pragma once
#include <string>
#include "EntityDeclaration.h"

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
    explicit Relationship(const std::string& type);
    bool setRef(EntityDeclaration* left_ref, EntityDeclaration* right_ref);
    RelationshipType getType();
    std::string getTypeStr();
    EntityDeclaration* getLeftRef();
    EntityDeclaration* getRightRef();
private:
    RelationshipType type;
    EntityDeclaration* left_ref;
    EntityDeclaration* right_ref;
};