#include <math.h>

#include "physics.h"

float physics_object_distance(Vector3f *pos1, Collider *collider1, Vector3f *pos2, Collider *collider2) {
    // currently only sphere to sphere supported
    if (collider1->type != COLLIDER_SPHERE || collider2->type != COLLIDER_SPHERE) {
        return 100.0; // high enough? 
    }
    float center_distance2 = fabs(vector3f_dist2(pos1, pos2));
    float radius2 = collider1->sphere.radius + collider2->sphere.radius;
    radius2 *= radius2;
    return sqrt(center_distance2)-sqrt(radius2);
}