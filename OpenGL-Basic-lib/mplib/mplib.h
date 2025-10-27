#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "cglm/struct.h"

#define PI 3.1415926f
#define DEG2RAD 0.0174532f
#define RAD2DEG 57.295779f

// Colors
#define WHITE (Color){1,1,1,1}
#define BLACK (Color){0,0,0,1}
#define MAGENTA (Color){0.4f, 0.35f, 0.55f, 1.0f}
#define DARK_GRAY (Color){0.3f, 0.3f, 0.3f, 1.0f}

typedef struct{
    float r,g,b,a;
} Color;

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
    unsigned int vbo_ids[2]; // array of vbo (one per vertex attrib (pos, tex_coords)) #just those two for now
    unsigned int vertex_count;
    float* positions;
    unsigned int position_count;
    float* tex_coords;
    unsigned int tex_coord_count;

    unsigned int index_count;
    unsigned int* indices;
} MPMesh;
typedef struct{
    MPMesh mesh;
    unsigned int shader_program;
    mat4s transform;
    Texture albedo;
} MPModel;


MPMesh mp_create_quad();
MPModel mp_load_model_from_mesh(MPMesh mesh);
void mp_update_camera_3d(GLFWwindow* window, Camera3D* camera, float move_speed, float dt);
MPMesh mp_load_obj(const char* path);
void mp_unload_model(MPModel model);

//init- window related
GLFWwindow* mp_init(int win_width, int win_height, const char* window_title);
bool mp_window_should_close(GLFWwindow* window);
void mp_terminate();

vec2s mp_get_mouse_delta(GLFWwindow *window);
float mp_get_frame_time();

//drawing related
void mp_begin_drawing(GLFWwindow* window, Color clear_color);
void mp_end_drawing(GLFWwindow* window);
void mp_begin_3d_mode(Camera3D* camera);

//model related
char* mp_get_shader_content(const char* fileName);
unsigned int mp_create_shader_program(const char* vert_shader_path, const char* frag_shader_path);
Texture mp_load_texture(const char* texture_path);
void mp_draw_model(MPModel model, Camera3D camera, Color color);
