#include "PQLQuery.h"

PQLQuery::PQLQuery(vector<Entity*> *return_entities, vector<Relationship*> *relationships, unordered_map<string, Entity*> *synonym_to_entities) {
    this->entities = return_entities;
    this->relationships = relationships;
    this->synonym_to_entities = synonym_to_entities;
}

void PQLQuery::setQueryEntities(vector<Entity*> *entities) {
    this->entities = entities;
}

void PQLQuery::setQueryRelationships(vector<Relationship*> *relationships) {
    this->relationships = relationships;
}

vector<Entity*> *PQLQuery::getQueryEntities() {
    return this->entities;
}

vector<Relationship*> *PQLQuery::getQueryRelationships() {
    return this->relationships;
}

unordered_map<string, Entity*> *PQLQuery::getSynonymToEntities() {
    return this->synonym_to_entities;
}