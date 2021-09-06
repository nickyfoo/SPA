#include "EntRef.h"
#include "StmtRef.h"

enum class SuchThatRefType {
    Entity,
    Statement,
    None
};

class SuchThatRef {
public:
    SuchThatRef();
    SuchThatRef(EntRef entity_ref);
    SuchThatRef(StmtRef stmt_ref);

    SuchThatRefType getType();
    EntRef getEntRef();
    StmtRef getStmtRef();

private:
    SuchThatRefType type;
    EntRef entity_ref;
    StmtRef stmt_ref;
};
