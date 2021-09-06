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
    void setSynonym(std::string& synonym);
    void setArgument(std::string& argument);
    void setWildCard();

    EntRefType getType();
    std::string getSynonym();
    std::string getArgument();
    std::string getValue();

private:
    EntRefType type;
    std::string synonym;
    std::string argument;
};
