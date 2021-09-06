#pragma once
#include <string>
#include "SuchThatRef.h"

enum class RelRef {
    Follows,
    FollowsT,
    Parent,
    ParentT,
    Uses,
    UsesS,
    UsesP,
    Modifies,
    ModifiesS,
    ModifiesP,
    None
};

class SuchThatClause {
public:
    explicit SuchThatClause(const std::string& type);
    bool setRef(SuchThatRef* left_ref, SuchThatRef* right_ref);
    void setType(RelRef type);
    RelRef getType();
    std::string getTypeStr();
    SuchThatRef* getLeftRef();
    SuchThatRef* getRightRef();
private:
    RelRef type;
    SuchThatRef* left_ref;
    SuchThatRef* right_ref;
};