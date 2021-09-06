#include <string>

enum class StmtRefType {
    StmtNum,
    Synonym,
    WildCard,
    None
};

class StmtRef {
public:
    StmtRef();
    void setSynonym(std::string& synonym);
    void setStmtNum(int stmt_num);
    void setWildCard();

    StmtRefType getType();
    std::string getSynonym();
    int getStmtNum();
    std::string getValue();

private:
    StmtRefType type;
    std::string synonym;
    int stmt_num;
};
