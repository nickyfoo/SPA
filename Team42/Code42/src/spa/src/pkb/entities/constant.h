#pragma once
#include "entity.hpp"

class Constant : public Entity {
 public:
  Constant();

  explicit Constant(int value);

  ~Constant();

  // Gets the value of constant.
  int get_value();

 private:
  // Value of constant.
  int value_;
};
