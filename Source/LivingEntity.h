#ifndef LIVINGENTITY_H
#define LIVINGENTITY_H

#include "Entity.h"

class LivingEntity : public Entity{
    public:
        LivingEntity();

        float deltaX, deltaY;

        bool colX, colY;

    protected:

    private:
};

#endif // LIVINGENTITY_H
