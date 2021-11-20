#include "physics.h"

float physics_object_distance2(Object *one, Object *other) {
    // currently only sphere to sphere supported
    if (one->collider.type != COLLIDER_SPHERE || other->collider.type != COLLIDER_SPHERE) {
        return 100.0; // high enough? 
    }
    float center_distance2 = vector3f_dist2(&one->transform.position, &other->transform.position);
    float radius2 = one->collider.sphere.radius + other->collider.sphere.radius;
    radius2 *= radius2;
    return center_distance2-radius2;
}