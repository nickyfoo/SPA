#pragma once
#include "entity.hpp"

class Constant : public Entity {
 public:
  Constant();

  Constant(int value);

  ~Constant();

  // Gets the value of constant.
  int GetValue();

 private:
  // Value of constant.
  int value_;
};