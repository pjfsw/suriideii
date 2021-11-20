#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "object.h"

// Squared distance between colliders, negative value indicates collision
float physics_object_distance2(Object *one, Object *other);


#endif