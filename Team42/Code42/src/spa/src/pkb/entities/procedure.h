#pragma once
#include <string>
#include <set>
#include "entity.hpp"

class Procedure : public Entity {
 public:
  explicit Procedure(std::string name);

  ~Procedure();

  // Gets the name of procedure.
  std::string get_name();

 private:
  // Name of procedure.
  std::string name_;
};