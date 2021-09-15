#pragma once
#include <string>

class ExpressionSpec {
 public:
  ExpressionSpec();
  std::string get_expression();
  void set_expression(const std::string &str);
  void set_partial_pattern(bool is_partial_pattern);
  void set_wild_card();
  bool IsPartialPattern() const;
  bool IsWildCard() const;
 private:
  std::string expression_;
  bool partial_pattern_;
  bool wild_card_;
};
