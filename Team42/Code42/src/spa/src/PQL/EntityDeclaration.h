#pragma once
#include <string>

enum class EntityType {
    Stmt,
    Read,
    Print,
    Call,
    While,
    If,
    Assign,
    Variable,
    Constant,
    Procedure,
    Argument,
    None
};

class EntityDeclaration {
public:
    EntityDeclaration(EntityType type, std::string synonym);
    EntityDeclaration(std::string value);
    EntityType getType();
    std::string getSynonym();
    int getStmtRef();
    std::string getEntRef();
    std::string getValue();
    bool isSynonym();
    bool isStmtNum();
    bool isValue();
    bool isWildCard();

private:
    EntityType type;
    std::string synonym;
    int stmt_num;
    std::string value;
};