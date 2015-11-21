#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

Player::Player(){
}

void Player::update(){
    if(deltaX != 0){
        this->colX = false, this->colY = false;

        if(!insideMap((this->posX+this->deltaX)*tileSize, this->posY*tileSize, this->width, this->height)){
            this->colX = true;
        }else if(!this->colX){
            this->posX += deltaX;
        }

        this->deltaX = 0;
        this->deltaY = 0;
    }
}

void Player::draw(){
    al_draw_bitmap(this->bitmapImage, this->posX*tileSize, this->posY*tileSize, NULL);
}
