#include "algebra.h"
#include "light.h"

void light_view_matrix(DirectionalLight *light, Vector3f *camera_pos, Matrix4f *m) {
    Vector3f eye;
    Vector3f at;
    Vector3f up; // sweat
    vector3f_copy(camera_pos, &at);
    Vector3f dir;
    vector3f_copy(&light->direction, &dir);
    vector3f_scale(2, &dir);
    vector3f_sub(&at, &dir, &eye);

    // TODO
    vector3f_set(&up, 0,0,1);

    Vector3f zaxis;
    Vector3f xaxis;
    Vector3f yaxis;

    // vec3 zaxis = normalize(at - eye);    
    vector3f_sub(&at, &eye, &zaxis);
    vector3f_normalize(&zaxis);


    //vec3 xaxis = normalize(cross(zaxis, up));
    vector3f_cross(&zaxis, &up, &xaxis);
    vector3f_normalize(&xaxis);

    //vec3 yaxis = cross(xaxis, zaxis);
    vector3f_cross(&xaxis, &zaxis, &yaxis);

    // negate(zaxis);
    vector3f_scale(-1, &zaxis);

    matrix4f_set(m,
        xaxis.x, xaxis.y, xaxis.z, -vector3f_dot(&xaxis, &eye),
        yaxis.x, yaxis.y, yaxis.z, -vector3f_dot(&yaxis, &eye),
        zaxis.x, zaxis.y, zaxis.z, -vector3f_dot(&zaxis, &eye),
        0,0,0,1);

  
  /*mat4 viewMatrix = {
    vec4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)),
    vec4(yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)),
    vec4(zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)),
    vec4(0, 0, 0, 1)
  };*/
}