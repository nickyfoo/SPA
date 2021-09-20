#pragma once
#include "entity.hpp"

class Constant : public Entity {
 public:
  Constant();

  explicit Constant(std::string value);

  ~Constant();

  // Gets the value of constant.
  std::string get_value();

 private:
  // Value of constant.
  std::string value_;
};
