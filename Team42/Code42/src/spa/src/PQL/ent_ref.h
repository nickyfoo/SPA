#pragma once
#include <string>

enum class EntRefType {
    Argument,
    Synonym,
    WildCard,
    None
};

class EntRef {
 public:
    EntRef();
    void set_synonym(const std::string& syn);
    void set_argument(const std::string& arg);
    void set_wild_card();

    EntRefType get_type();
    std::string get_synonym();
    std::string get_argument();
    std::string get_value();

 private:
    EntRefType type_;
    std::string synonym_;
    std::string argument_;
};
