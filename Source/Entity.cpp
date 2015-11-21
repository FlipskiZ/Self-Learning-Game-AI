#include "Engine.h"
#include "Entity.h"

Entity::Entity(){
    this->posX = 0;
    this->posY = 0;
    this->centerX = 0;
    this->centerY = 0;
    this->width = 0;
    this->height = 0;
    this->entityId = 0;
    this->typeId = 0;
    this->active = true;
}

void Entity::setPos(float x, float y){
    this->posX = x;
    this->posY = y;
    this->updateCenter();
}
void Entity::updateCenter(){
    this->centerX = this->posX + this->width/2, this->centerY = this->posY + this->height/2;
}
void Entity::setDimensions(float width, float height){
    this->width = width;
    this->height = height;
}
void Entity::setEntityId(int entityId){
    this->entityId = entityId;
}
void Entity::setTypeId(int typeId){
    this->typeId = typeId;
}
void Entity::setActive(bool active){
    this->active = active;
}
bool Entity::checkActive(){
    return this->active;
}
void Entity::setBitmap(ALLEGRO_BITMAP *image){
    this->bitmapImage = image;
}
