#include <string>

class ExpressionSpec {
public:
    ExpressionSpec();
    std::string getExpression();
    void setExpression(std::string& str);
    void setPartialPattern(bool is_partial_pattern);
    void setWildCard();
    bool isPartialPattern() const;
    bool isWildCard() const;
private:
    std::string expression;
    bool partial_pattern;
    bool wild_card;
};
