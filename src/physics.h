#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "object.h"

// Distance between colliders, negative value indicates collision
float physics_object_distance2(Vector3f *pos1, Collider *collider1, Vector3f *pos2, Collider *collider2);


#endif