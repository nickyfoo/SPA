#pragma once
#include <string>
#include <set>
#include "Entity.hpp"

class Procedure : public Entity {
 public:
  explicit Procedure(std::string name);

  ~Procedure();

  // Gets the name of procedure.
  std::string GetName();

 private:
  // Name of procedure.
  std::string name_;
};