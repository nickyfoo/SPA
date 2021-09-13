#pragma once
#include <string>
#include "entity.hpp"

class Variable : public Entity {
 public:
  Variable(int index, std::string name);

  ~Variable();

  // Gets the index mapping to the variable.
  int get_index();
  // Gets the name of the variable.
  std::string get_name();

 private:
  // Mapped index of variable.
  int index_;
  // Name of variable.
  std::string name_;
};