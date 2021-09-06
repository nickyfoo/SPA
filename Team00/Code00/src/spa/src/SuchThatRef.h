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
    explicit SuchThatRef(EntRef entity_ref);
    explicit SuchThatRef(StmtRef stmt_ref);

    SuchThatRefType getType();
    EntRef getEntRef();
    StmtRef getStmtRef();

private:
    SuchThatRefType type;
    EntRef entity_ref;
    StmtRef stmt_ref;
};
