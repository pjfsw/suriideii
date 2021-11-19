#include <GL/glew.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "hud_shader.h"
#include "light.h"
#include "mesh.h"
#include "renderer.h"
#include "render_shader.h"
#include "shadow_shader.h"
#include "shadowmap.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

typedef struct {
    int width;
    int height;
    SDL_Window *window;
    SDL_GLContext context;
} RendererWindow;

struct _Renderer {
    RenderShader *render_shader;
    ShadowShader *shadow_shader;  
    HudShader *hud_shader;
    RendererWindow *renderer_window;
    Shadowmap *shadowmap;
    DirectionalLight *light;
};

void _renderer_destroy_window(RendererWindow *window) {
    if (window == NULL) {
        return;
    }
    if (window->window != NULL) {
        SDL_DestroyWindow(window->window);
    }
    free(window);
}

RendererWindow *_renderer_create_window() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, REQ_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, REQ_MINOR_VERSION);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    printf("Display reports size %dx%d\n", dm.w, dm.h);

    RendererWindow *window = calloc(1, sizeof(RendererWindow));
    window->width = dm.w;
    window->height = dm.h;
    window->window = SDL_CreateWindow("The Window Name", 0, 0, window->width, window->height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (window->window == NULL) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        _renderer_destroy_window(window);
        return NULL;
    }
    if ((window->context = SDL_GL_CreateContext(window->window)) == NULL) {
        fprintf(stderr, "Failed to create GL context: %s\n", SDL_GetError());
        _renderer_destroy_window(window);
        return NULL;
    }
    int major;
    int minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    printf("OpenGL version %d.%d\n", major, minor);
    int r;
    int g;
    int b;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
    printf("Color depth R=%d, G=%d, B=%d\n", r, g, b);

    glewExperimental = true;
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "glew Init: '%s'\n", glewGetErrorString(res));
        _renderer_destroy_window(window);
        return NULL;
    }    

    const char *gl_version = (const char*)glGetString(GL_VERSION);
    if (gl_version == NULL) {
        fprintf(stderr, "OpenGL %d.%d+ required\n", REQ_MAJOR_VERSION, REQ_MINOR_VERSION);
        return false;
    }
    printf("GL Version %s\n", gl_version);

    int texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texture_units);
    printf("Max texture units: %d\n", texture_units);
    if (texture_units < 2) {
        fprintf(stderr, "At least 2 texture units required\n");
        _renderer_destroy_window(window);
        return NULL;
    }

    return window;
}

void _renderer_update_transforms(Renderer *renderer, double fov) {
    int w = renderer->renderer_window->width;
    int h = renderer->renderer_window->height;
    glViewport(0, 0, w, h);

    printf("Viewport is %dx%d\n", w, h);

    fov *= M_PI/180.0;
    float ar = (float)h / (float)w;

    float near_z = 1;
    float far_z = 60;
    float z_range = near_z - far_z;
    float perspective_a = (-far_z - near_z) / z_range;
    float perspective_b = 2.0f * far_z * near_z / z_range;

    Matrix4f projection;

    matrix4f_perspective(&projection, fov, ar, perspective_a, perspective_b);
    render_shader_set_projection(renderer->render_shader, &projection);

    matrix4f_ortho(&projection, -30, 30, -30, 30, -30, 30);
    shadow_shader_set_projection(renderer->shadow_shader, &projection);
    render_shader_set_light_projection(renderer->render_shader, &projection);
}

void _renderer_create_lights(Renderer *renderer) {
    Lighting *lighting = render_shader_get_lighting(renderer->render_shader);
    lighting_set_default_reflection(lighting, 0.6, 0.4, 0.5, 32);
    renderer->light = lighting_create_directional(lighting, 1, -0.8, 1, 1, 1, 1);
    lighting_set_shadow_strength(lighting, 0.75); 
}

Renderer *renderer_create(double fov) {
    Renderer *renderer = calloc(1, sizeof(Renderer));

    if ((renderer->renderer_window = _renderer_create_window(renderer)) == NULL ||
        (renderer->shadowmap = shadowmap_create(renderer->renderer_window->width, renderer->renderer_window->height)) == NULL ||
        (renderer->render_shader = render_shader_create(renderer->shadowmap)) == NULL ||
        (renderer->shadow_shader = shadow_shader_create(renderer->shadowmap)) == NULL ||
        (renderer->hud_shader = hud_shader_create()) == NULL) {
        renderer_destroy(renderer);
        return NULL;
    }

    _renderer_update_transforms(renderer, fov);
    _renderer_create_lights(renderer);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_CONSTANT_ALPHA);    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return renderer;
}

void renderer_destroy(Renderer *renderer) {
    if (renderer == NULL) {
        return;
    }
    _renderer_destroy_window(renderer->renderer_window);
}

void _renderer_create_shadows(Renderer *renderer, Object **objects, int object_count) {
    shadow_shader_select(renderer->shadow_shader);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < object_count; i++) {
        Object *object = objects[i];
        if (object != NULL) {
            shadow_shader_set_world(renderer->shadow_shader, &object->transform.m);
            mesh_render(object->mesh);
        }
    }
}

void _renderer_render_object(Renderer *renderer, Object *object) {
    render_shader_bind_texture(object->texture);
    render_shader_set_world(renderer->render_shader, &object->transform.m);
    mesh_render(object->mesh);
}    

void _renderer_render_scene(Renderer *renderer, Object **objects, int object_count) {
    render_shader_select(renderer->render_shader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < object_count; i++) {
        if (objects[i] != NULL) {
            _renderer_render_object(renderer, objects[i]);
        }
    }
}

void renderer_draw(Renderer *renderer, Object **objects, int object_count) {
    _renderer_create_shadows(renderer, objects, object_count);
    _renderer_render_scene(renderer, objects, object_count);
    SDL_GL_SwapWindow(renderer->renderer_window->window);
}

void renderer_set_camera(Renderer *renderer, Matrix4f *camera, Vector3f *camera_pos) {
    render_shader_set_camera(renderer->render_shader, camera, camera_pos);
    Matrix4f light_view;
    light_view_matrix(renderer->light, camera_pos, &light_view);
    render_shader_set_light_view(renderer->render_shader, &light_view);
    shadow_shader_set_camera(renderer->shadow_shader, &light_view);
}