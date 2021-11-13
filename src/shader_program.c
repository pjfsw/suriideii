#include "shader_program.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool _sp_create_shader(GLuint shader_program, int shader_type, char *shader) {
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        fprintf(stderr, "Error creating for: %s\n", shader);
        return false;
    }

    const GLchar *p[1];
    p[0] = shader;
    GLint lengths[1];
    lengths[0] = strlen(shader);

    glShaderSource(shader_obj, 1, p, lengths);
    glCompileShader(shader_obj);
    
    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (success == 0) {
        int log_size = 1024;
        GLchar info_log[log_size];
        glGetShaderInfoLog(shader_obj, log_size, NULL, info_log);
        fprintf(stderr, "Error compiling shader %s: '%s'\n", shader, info_log);
        return false;
    }
    glAttachShader(shader_program, shader_obj);
    return true;
}

GLuint _sp_compile_shaders(char *vertex_shader, char *fragment_shader) {
    GLuint shader_program = glCreateProgram();
    if (shader_program == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return 0;
    }
    printf("Created program\n");
    if (!_sp_create_shader(shader_program, GL_VERTEX_SHADER, vertex_shader)) {
        return 0;
    }
    printf("Created vertex shader\n");

    if (!_sp_create_shader(shader_program, GL_FRAGMENT_SHADER, fragment_shader)) {
        return 0;
    }
    printf("Created fragment shader\n");
    glLinkProgram(shader_program);
    printf("Linked shader programs\n");

    GLint success;
    int log_size = 1024;
    GLchar info_log[log_size];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Error linking shader program: '%s'\n", info_log);
        return 0;
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Invalid shader program: '%s'\n", info_log);
        return 0;
    }

    return shader_program;
}

char *_load_file(char *file_name) {
    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file: '%s'\n", file_name);
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *str = calloc(size+1,sizeof(char));
    fread(str, sizeof(char), size, fp);
    fclose(fp);
    return str;
}

GLuint shader_program_build(char *vs_name, char *fs_name) {
    char *vertex_shader = _load_file(vs_name);
    if (vertex_shader == NULL) {
        return 0;
    }
    printf("Loaded vertex shader %s\n", vs_name);
    char *fragment_shader = _load_file(fs_name);
    if (fragment_shader == NULL) {
        free(vertex_shader);
        return 0;
    }
    printf("Loaded fragment shader %s\n", fs_name);

    GLuint program = _sp_compile_shaders(vertex_shader, fragment_shader);
    if (program > 0) {
        fprintf(stderr, "Compiled shader program\n");
    } else {
        fprintf(stderr, "Failed to compile shader program\n");
    }
    free(vertex_shader);
    free(fragment_shader);
    return program;
}
