#include "entity_declaration_parser.h"
#include <algorithm>

EntityDeclarationParser::EntityDeclarationParser() = default;
EntityDeclarationParser *EntityDeclarationParser::instance_ = nullptr;

EntityDeclarationParser *EntityDeclarationParser::get_instance() {
  if (!instance_) {
    instance_ = new EntityDeclarationParser();
  }
  return instance_;
}

void EntityDeclarationParser::set_entities(std::vector<std::string> *entities) {
  entity_declaration_strings_ = entities;
}

std::unordered_map<std::string, EntityDeclaration *>
*EntityDeclarationParser::get_entities_map() {
  auto *entities_map =
      new std::unordered_map<std::string, EntityDeclaration *>;
  for (std::string entity_str : *entity_declaration_strings_) {
    // Trim entity_str first and find EntityType declaration
    entity_str = EntityDeclarationParser::trim(entity_str);
    int index_of_entity = entity_str.find(" ");
    std::string entity = entity_str.substr(0, index_of_entity);
    // Trimming entity_str to remove entity type.
    entity_str = entity_str.substr(index_of_entity + 1);
    entity_str = EntityDeclarationParser::trim(entity_str);
    // If there are no commas but multiple for the
    // same entity type, or if there's only 1 word, return nullptr. eg stmt s p c;
    std::vector<std::string> tokens = SplitString(entity_str, " ");
    size_t num_of_comma = std::count(entity_str.begin(), entity_str.end(), ',');
//    if (((entity_str.find(",") == -1 &&
//        tokens.size() > 1)) ||
//        index_of_entity == -1) {
//      return nullptr;
//    }
    if (num_of_comma < tokens.size() - 1 || index_of_entity == -1) {
      return nullptr;
    }
    auto *synonyms = new std::vector<std::string>();
    EntityType entity_type = CheckEntityType(entity);
    if (entity_type != EntityType::None &&
        IsValidSynonym(tokens, synonyms) &&
        AddToEntitiesMap(entities_map, synonyms, entity_type)) {
    } else {
      return nullptr;
    }
  }
  return entities_map;
}

std::string EntityDeclarationParser::trim(std::string str) {
  auto start = str.begin();
  while (start != str.end() && std::isspace(*start)) {
    start++;
  }

  auto end = str.end();
  do {
    end--;
  } while (std::distance(start, end) > -1 && std::isspace(*end));

  return {start, end + 1};
}

bool EntityDeclarationParser::AddToEntitiesMap(
    std::unordered_map<std::string, EntityDeclaration *> *entities_map,
    std::vector<std::string> *synonyms,
    EntityType entity_type) {
  for (const std::string &synonym : *synonyms) {
    auto *entity_obj =
        new EntityDeclaration(entity_type,
                              synonym);
    if (entities_map->find(synonym) != entities_map->end()) {
      return false;
    }
    entities_map->insert({synonym, entity_obj});
  }
  return true;
}

bool EntityDeclarationParser::IsValidIdentifier(std::string str) {
  // If first character is invalid
  if (!((str[0] >= 'a' && str[0] <= 'z')
      || (str[0] >= 'A' && str[0] <= 'Z'))) {
    return false;
  }

  // Traverse the string for the rest of the characters
  for (int i = 1; i < str.length(); i++) {
    if (!((str[i] >= 'a' && str[i] <= 'z')
        || (str[i] >= 'A' && str[i] <= 'Z')
        || (str[i] >= '0' && str[i] <= '9'))) {
      return false;
    }
  }

  // String is a valid identifier
  return true;
}

// Checking if entity declared is part of the valid design entities.
EntityType EntityDeclarationParser::CheckEntityType(std::string type) {
  if (type == "stmt") {
    return EntityType::Stmt;
  } else if (type == "read") {
    return EntityType::Read;
  } else if (type == "print") {
    return EntityType::Print;
  } else if (type == "call") {
    return EntityType::Call;
  } else if (type == "while") {
    return EntityType::While;
  } else if (type == "if") {
    return EntityType::If;
  } else if (type == "assign") {
    return EntityType::Assign;
  } else if (type == "variable") {
    return EntityType::Variable;
  } else if (type == "constant") {
    return EntityType::Constant;
  } else if (type == "procedure") {
    return EntityType::Procedure;
  } else if (type == "prog_line") {
    return EntityType::ProgLine;
  } else {
    return EntityType::None;
  }
}

bool EntityDeclarationParser::IsValidSynonym(
    std::vector<std::string> tokens,
    std::vector<std::string> *synonyms) {
  for (std::string token : tokens) {
    // Splitting token further by ","
    // for multiple declarations for the same design entity.
    std::vector<std::string> split_tokens = SplitString(token, ",");
    for (std::string split_token : split_tokens) {
      // Checking if each token is a valid identifier
      // and doesn't already exist in synonyms.
      if (!IsValidIdentifier(split_token) ||
          std::count(synonyms->begin(), synonyms->end(), split_token)) {
        return false;
      } else {
        synonyms->push_back(split_token);
      }
    }
  }
  return true;
}

std::vector<std::string> EntityDeclarationParser::SplitString(
    std::string str,
    std::string delimiter) {
  // Splitting string by delimiter and putting tokens into a vector.
  std::vector<std::string> tokens;
  int start = 0;
  int end = str.find(delimiter);
  std::string token;
  while (end != -1) {
    token = str.substr(start, end - start);
    start = end + delimiter.size();
    end = str.find(delimiter, start);
    if (token != "") {
      tokens.push_back(token);
    }
  }
  if (start < str.length()) {
    tokens.push_back(str.substr(start));
  }
  return tokens;
}
