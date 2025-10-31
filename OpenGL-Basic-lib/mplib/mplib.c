#include "mplib/mplib.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "cglm/struct.h"
#include <GLFW/glfw3.h>
#include <math.h>

#define FAST_OBJ_IMPLEMENTATION
#include "external/fast_obj.h"

GLFWwindow* mp_init(int win_width, int win_height, const char* window_title){
    if(!glfwInit()){
        printf("glfw window not initialized\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, window_title, NULL, NULL);
    if(!window){
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return NULL;
    }
    glad_glViewport(0, 0, win_width, win_height);

    glad_glEnable(GL_DEPTH_TEST);
    glad_glEnable(GL_CULL_FACE);
    glad_glCullFace(GL_BACK);

    return window;
}
bool mp_window_should_close(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
        return true;
    }

    return glfwWindowShouldClose(window);
}
void mp_begin_drawing(GLFWwindow* window, Color clear_color){
    glad_glClearColor(clear_color.r, clear_color.g, clear_color.b,1.0f);
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
}
void mp_end_drawing(GLFWwindow* window){
    glfwPollEvents();
    glfwSwapBuffers(window);
}
void mp_terminate(){
    glfwTerminate();
}

char* mp_get_shader_content(const char* fileName){
    FILE *fp;
    long size = 0;
    char* shaderContent;
    
    /* Read File to get size */
    fp = fopen(fileName, "rb");
    if(fp == NULL) {
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);
    /* Read File for Content */
    fp = fopen(fileName, "r");
    shaderContent = memset(malloc(size), '\0', size);
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

unsigned int mp_create_shader(const char *vert_shader_path, const char *frag_shader_path){
    const char* vert = mp_get_shader_content(vert_shader_path);
    unsigned int vert_shader = glad_glCreateShader(GL_VERTEX_SHADER);
    glad_glShaderSource(vert_shader, 1, &vert, NULL);
    glad_glCompileShader(vert_shader);

    const char* frag = mp_get_shader_content(frag_shader_path);
    unsigned int frag_shader= glad_glCreateShader(GL_FRAGMENT_SHADER);
    glad_glShaderSource(frag_shader, 1, &frag, NULL);
    glad_glCompileShader(frag_shader);
    unsigned int shader_prog = glad_glCreateProgram();
    glad_glAttachShader(shader_prog, vert_shader);
    glad_glAttachShader(shader_prog, frag_shader);
    glad_glLinkProgram(shader_prog);

    glad_glDeleteShader(vert_shader);
    glad_glDeleteShader(frag_shader);

    free((void*)vert);
    free((void*)frag);
    glad_glUseProgram(0);

    return shader_prog;
}
MPMesh mp_create_quad(){
    float positions[] = {
         0.5f, 0.5f, 0.0f,
         0.5f,-0.5f, 0.0f,
        -0.5f,-0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    float uvs[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    int  indices[]={
        0, 1, 3,
        1, 2, 3  
    };

    MPMesh m = {0};
    m.vertex_count = 4;
    m.index_count= 6;
    glad_glGenVertexArrays(1, &m.vao);
    glad_glBindVertexArray(m.vao);

    glad_glGenBuffers(2, m.vbo_ids);
    glad_glBindBuffer(GL_ARRAY_BUFFER, m.vbo_ids[0]);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(0);

    glad_glBindBuffer(GL_ARRAY_BUFFER, m.vbo_ids[1]);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2* sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(1);

    unsigned int ebo;
    glad_glGenBuffers(1, &ebo);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glad_glBindVertexArray(0);
    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return m;
}
MPModel mp_load_model_from_mesh(MPMesh mesh){
    MPModel model = {0};
    model.transform = glms_mat4_identity();
    model.mesh = mesh;
    return model;
}
MPMesh mp_load_obj(const char* path){
    MPMesh mesh_out = {0};

    fastObjMesh* mesh = fast_obj_read(path);
    assert(mesh && "Failed to read OBJ");

    unsigned int max_vertices = mesh->face_count * 3; // "face_count" = number of triangles

    mesh_out.positions = malloc(max_vertices * 3 * sizeof(float));
    mesh_out.tex_coords= malloc(max_vertices * 2 * sizeof(float));
    mesh_out.normals   = malloc(max_vertices * 3 * sizeof(float));
    mesh_out.indices   = malloc(mesh->index_count * sizeof(unsigned int));

    mesh_out.vertex_count = max_vertices;
    mesh_out.index_count  = mesh->index_count;

    for (unsigned int i = 0; i < mesh->index_count; i++) {
        fastObjIndex idx = mesh->indices[i];

        mesh_out.positions[i * 3 + 0] = mesh->positions[idx.p * 3 + 0];
        mesh_out.positions[i * 3 + 1] = mesh->positions[idx.p * 3 + 1];
        mesh_out.positions[i * 3 + 2] = mesh->positions[idx.p * 3 + 2];

        if (idx.t < mesh->texcoord_count) {
            mesh_out.tex_coords[i * 2 + 0] = mesh->texcoords[idx.t * 2 + 0];
            mesh_out.tex_coords[i * 2 + 1] = mesh->texcoords[idx.t * 2 + 1];
        } else {
            mesh_out.tex_coords[i * 2 + 0] = 0.0f;
            mesh_out.tex_coords[i * 2 + 1] = 0.0f;
        }

        if (idx.n < mesh->normal_count) {
            mesh_out.normals[i * 3 + 0] = mesh->normals[idx.n * 3 + 0];
            mesh_out.normals[i * 3 + 1] = mesh->normals[idx.n * 3 + 1];
            mesh_out.normals[i * 3 + 2] = mesh->normals[idx.n * 3 + 2];
        } else {
            mesh_out.normals[i * 3 + 0] = 0.0f;
            mesh_out.normals[i * 3 + 1] = 0.0f;
            mesh_out.normals[i * 3 + 2] = 0.0f;
        }

        mesh_out.indices[i] = i;
    }

    fast_obj_destroy(mesh);

    glad_glGenVertexArrays(1, &mesh_out.vao);
    glad_glBindVertexArray(mesh_out.vao);

    glad_glGenBuffers(3, mesh_out.vbo_ids);

    glad_glBindBuffer(GL_ARRAY_BUFFER, mesh_out.vbo_ids[0]);
    glad_glBufferData(GL_ARRAY_BUFFER, mesh_out.vertex_count * 3 * sizeof(float),
                      mesh_out.positions, GL_STATIC_DRAW);
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(0);

    glad_glBindBuffer(GL_ARRAY_BUFFER, mesh_out.vbo_ids[1]);
    glad_glBufferData(GL_ARRAY_BUFFER, mesh_out.vertex_count * 2 * sizeof(float),
                      mesh_out.tex_coords, GL_STATIC_DRAW);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(1);

    glad_glBindBuffer(GL_ARRAY_BUFFER, mesh_out.vbo_ids[2]);
    glad_glBufferData(GL_ARRAY_BUFFER, mesh_out.vertex_count * 3* sizeof(float),
                      mesh_out.normals, GL_STATIC_DRAW);
    glad_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(2);

    unsigned int ebo;
    glad_glGenBuffers(1, &ebo);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                      mesh_out.index_count * sizeof(unsigned int),
                      mesh_out.indices, GL_STATIC_DRAW);

    glad_glBindVertexArray(0);
    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return mesh_out;
}

Texture mp_load_texture(const char *texture_path){
    Texture tex = {0};
    int ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texture_path, &tex.width, &tex.height, &ch, 0);
    if(data){
        glad_glGenTextures(1, &tex.id);
        glad_glBindTexture(GL_TEXTURE_2D, tex.id);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glad_glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        printf("TEXTURE: fail to load, '%s' doesn't exist \n", texture_path);
        glad_glBindTexture(GL_TEXTURE_2D, 0);
        return  tex;;
    }
    stbi_image_free(data);
    glad_glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}
void mp_draw_model(MPModel model, Camera3D camera, Color color){
    glad_glUseProgram(model.shader_program);

    int model_loc   = glad_glGetUniformLocation(model.shader_program, "model");
    int view_loc    = glad_glGetUniformLocation(model.shader_program, "view");
    int proj_loc    = glad_glGetUniformLocation(model.shader_program, "projection");
    int ambient_loc = glad_glGetUniformLocation(model.shader_program, "ambient_color");
    glad_glUniformMatrix4fv(model_loc,1, GL_FALSE, (const float*)model.transform.raw);
    glad_glUniformMatrix4fv(view_loc, 1, GL_FALSE, (const float*)camera.cam_matrix.raw);
    glad_glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (const float*)camera.proj_matrix.raw);
    glad_glUniform4f(ambient_loc, color.r, color.g, color.b, color.a);

    if(model.albedo.id > 0){
        glad_glBindTexture(GL_TEXTURE_2D, model.albedo.id);
    }
    glad_glBindVertexArray(model.mesh.vao);
    glad_glDrawElements(GL_TRIANGLES, model.mesh.index_count, GL_UNSIGNED_INT, 0);
    // unbind texture buffer, vao and shader program
    glad_glBindVertexArray(0);
    glad_glBindTexture(GL_TEXTURE_2D, 0);
    glad_glUseProgram(0);
}

void mp_begin_3d_mode(Camera3D* camera){
    camera->cam_matrix = glms_lookat(camera->position, camera->target, camera->up);
    camera->proj_matrix = glms_perspective(camera->fovy * DEG2RAD, camera->aspect, 0.05f, 100.0f);

    // float ar = 5;
    // camera->proj_matrix = glms_ortho(-1*camera->aspect * ar, 1*camera->aspect * ar,-1 * ar, 1 *ar, 0.01f, 100);
}

double lastX = 0.0, lastY = 0.0;
double deltaX = 0.0, deltaY = 0.0;
int firstMouse = 1;
vec2s mp_get_mouse_delta(GLFWwindow *window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    deltaX = xpos - lastX;
    deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    vec2s delta = (vec2s){.x = deltaX, .y = deltaY};
    return delta;
}

float yaw = -90, pitch = 0;
void mp_update_camera_3d(GLFWwindow *window, Camera3D *camera, float move_speed, float dt){
    vec2s mouse_delta = mp_get_mouse_delta(window);
    yaw += mouse_delta.x * 0.2f;
    pitch += -mouse_delta.y * 0.2f;
    if(pitch > 89) pitch = 89;
    if(pitch <-89) pitch =-89;

    float rad_yaw = yaw * DEG2RAD;
    float rad_pitch = pitch  * DEG2RAD;

    vec3s tar = (vec3s){.x = cosf(rad_yaw)*cosf(rad_pitch),
                        .y = sinf(rad_pitch),
                        .z = sinf(rad_yaw)*cosf(rad_pitch)};

    vec3s forward = glms_vec3_normalize(glms_vec3_sub(camera->target, camera->position));
    vec3s right = glms_vec3_cross(forward, camera->up);

    vec3s dir = {0};
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir = glms_vec3_add(dir, forward);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir = glms_vec3_sub(dir, forward);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir = glms_vec3_add(dir, right);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir = glms_vec3_sub(dir, right);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) dir = glms_vec3_add(dir, camera->up);

    dir = glms_vec3_normalize(dir);

    camera->position = glms_vec3_add(camera->position, glms_vec3_scale(dir, move_speed * dt));
    camera->target = glms_vec3_add(camera->position, tar);
}

float last_time = 0;
float mp_get_frame_time(){
    float curr_time = glfwGetTime();;

    float delta =  curr_time - last_time;
    last_time = curr_time;;
    return delta;
}

void mp_unload_model(MPModel model){
    free(model.mesh.indices);
    free(model.mesh.positions);
    free(model.mesh.normals);
    free(model.mesh.tex_coords);
}

char* mp_format_text(const char* fmt, ...) {
    static char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return buffer;
}
