#ifndef ENTITY_H
#define ENTITY_H

#include "Engine.h"

class Entity{
    public:
        Entity();
        void setPos(float x, float y);
        void updateCenter();
        void setDimensions(float width, float height);
        void setEntityId(int entityId);
        void setTypeId(int entityId);
        void setActive(bool active);
        bool checkActive();
        void setBitmap(ALLEGRO_BITMAP *image);
        virtual void draw() = 0;
        virtual void update() = 0;

        float posX, posY, centerX, centerY;
        float width, height;

        int entityId, typeId;

        ALLEGRO_BITMAP *bitmapImage;

    protected:

        bool active;

    private:

};

#endif // ENTITY_H
