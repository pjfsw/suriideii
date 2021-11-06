#include "shader_program.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <stdio.h>
#include <string.h>

bool _sp_create_shader(GLuint shader_program, int shader_type, char *shader) {
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", shader_type);
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
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, info_log);
        return false;
    }
    glAttachShader(shader_program, shader_obj);
    return true;
}

bool _sp_compile_shaders(char *vertex_shader, char *fragment_shader) {
    GLuint shader_program = glCreateProgram();
    if (shader_program == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return false;
    }

    if (!_sp_create_shader(shader_program, GL_VERTEX_SHADER, vertex_shader)) {
        return false;
    }
    if (!_sp_create_shader(shader_program, GL_FRAGMENT_SHADER, fragment_shader)) {
        return false;
    }
    glLinkProgram(shader_program);

    GLint success;
    int log_size = 1024;
    GLchar info_log[log_size];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Error linking shader program: '%s'\n", info_log);
        return false;
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Invalid shader program: '%s'\n", info_log);
        return false;
    }

    glUseProgram(shader_program);

    return true;
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

bool shader_program_build(char *vs_name, char *fs_name) {
    char *vertex_shader = _load_file(vs_name);
    if (vertex_shader == NULL) {
        return false;
    }
    char *fragment_shader = _load_file(fs_name);
    if (fragment_shader == NULL) {
        free(vertex_shader);
        return false;
    }

    bool status = _sp_compile_shaders(vertex_shader, fragment_shader);
    free(vertex_shader);
    free(fragment_shader);
    return status;
}
