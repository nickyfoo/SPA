#pragma once
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
    void set_synonym(const std::string &syn);
    void set_stmt_num(int line_num);
    void set_wild_card();

    StmtRefType get_type();
    std::string get_synonym();
    int get_stmt_num() const;
    std::string get_value();

 private:
    StmtRefType type_;
    std::string synonym_;
    int stmt_num_;
};
