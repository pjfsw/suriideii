#include <string.h>

#include "transform.h"

void _transform_build_matrix(Transform *transform) {
    Matrix4f scale;
    matrix4f_scale(&scale, transform->scale, transform->scale, transform->scale);
    Matrix4f rotation;
    matrix4f_rotation(&rotation, transform->rotation.x, transform->rotation.y, transform->rotation.z);
    Matrix4f translation;
    matrix4f_translation(&translation, transform->position.x, transform->position.y, transform->position.z);

    matrix4f_multiply_target(&rotation, &scale, &transform->m);
    matrix4f_multiply(&translation, &transform->m);
}

void transform_reset(Transform *transform) {
    vector3f_zero(&transform->position);
    vector3f_zero(&transform->rotation);
    transform->scale = 1;
    _transform_build_matrix(transform);
}

void transform_rebuild(Transform *transform) {
    _transform_build_matrix(transform);
}
