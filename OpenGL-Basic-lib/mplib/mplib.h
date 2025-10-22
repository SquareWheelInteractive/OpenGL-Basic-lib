#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "cglm/struct.h"

typedef struct{
    vec3s position;
    vec3s target;
    vec3s up;
    float fovy;
    float aspect;
    mat4s cam_matrix;
    mat4s proj_matrix;
} Camera3D;
typedef struct{
    unsigned int id;
    int width;
    int height;
} Texture;
typedef struct{
    unsigned int vao;
    unsigned int vbo;
    unsigned int shader_program;
    unsigned int vertex_count;
    unsigned int indices_count;
    Texture albedo;
    mat4s transform;
} Model;


Model mp_create_quad();
void mp_update_camera_3d(GLFWwindow* window, Camera3D* camera, float dt);

//init- window related
GLFWwindow* mp_init(int win_width, int win_height, const char* window_title);
bool mp_window_should_close(GLFWwindow* window);
void mp_terminate();

vec2s mp_get_mouse_delta(GLFWwindow *window);
float mp_get_frame_time();

//drawing related
void mp_begin_drawing(GLFWwindow* window);
void mp_end_drawing(GLFWwindow* window);
void mp_begin_3d_mode(Camera3D* camera);

//model related
char* mp_get_shader_content(const char* fileName);
unsigned int mp_create_shader_program(const char* vert_shader_path, const char* frag_shader_path);
Texture mp_load_texture(const char* texture_path);
void mp_draw_model(Model model, Camera3D camera);
