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
    void setSynonym(std::string& syn);
    void setStmtNum(int line_num);
    void setWildCard();

    StmtRefType getType();
    std::string getSynonym();
    int getStmtNum() const;
    std::string getValue();

private:
    StmtRefType type;
    std::string synonym;
    int stmt_num;
};
