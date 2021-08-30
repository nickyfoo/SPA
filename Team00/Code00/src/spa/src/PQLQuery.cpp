#include "PQLQuery.h"

PQLQuery::PQLQuery(vector<Entity*> *entities, vector<Relationship*> *relationships) {
    this->entities = entities;
    this->relationships = relationships;
}

void PQLQuery::setQueryEntities(vector<Entity*> *entities) {
    this->entities = entities;
}

void PQLQuery::setQueryRelationships(vector<Relationship*> *relationships) {
    this->relationships = relationships;
}

vector<Entity*> *PQLQuery::getQueryEntities() {
    return this->entities;
};

vector<Relationship*> *PQLQuery::getQueryRelationships() {
    return this->relationships;
};