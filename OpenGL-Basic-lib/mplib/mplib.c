#include "mplib/mplib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#include "cglm/struct.h"
#include <GLFW/glfw3.h>
#include <math.h>

#define DEG2RAD 0.0174532f
#define RAD2DEG 57.295779f

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

    return window;
}
bool mp_window_should_close(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
        return true;
    }

    return glfwWindowShouldClose(window);
}
void mp_begin_drawing(GLFWwindow* window){
    glad_glClearColor(0.4f,0.35f,0.5f,1);
    glad_glClear(GL_COLOR_BUFFER_BIT);
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

unsigned int mp_create_shader_program(const char *vert_shader_path, const char *frag_shader_path){
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

    glad_glUseProgram(0);
    free((void*)vert);
    free((void*)frag);

    return shader_prog;
}
Model mp_create_quad(){
    float positions[] = {
         0.5f, 0.5f, 0.0f,    1.0f, 1.0f,
         0.5f,-0.5f, 0.0f,    1.0f, 0.0f,
        -0.5f,-0.5f, 0.0f,    0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,    0.0f, 1.0f
    };
    int  indices[]={
        0, 1, 3,
        1, 2, 3  
    };

    Model m = {0};
    m.transform = glms_mat4_identity();
    m.vertex_count = 4;
    m.indices_count = 6;
    glad_glGenVertexArrays(1, &m.vao);
    glad_glBindVertexArray(m.vao);

    glad_glGenBuffers(1, &m.vbo);
    glad_glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
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
        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glad_glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        printf("TEXTURE: fail to load, '%s' doesn't exist \n", texture_path);
    }
    stbi_image_free(data);
    glad_glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}
void mp_draw_model(Model model, Camera3D camera){
    mat4s mvp; glms_mat4_mul(camera.proj_matrix, glms_mat4_mul(camera.cam_matrix, model.transform));

    glad_glUseProgram(model.shader_program);
    int mvp_loc = glad_glGetUniformLocation(model.shader_program, "mvp");
    glad_glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (const GLfloat*)mvp.raw);
    if(model.albedo.id > 0){
        glad_glBindTexture(GL_TEXTURE_2D, model.albedo.id);
    }
    glad_glBindVertexArray(model.vao);
    glad_glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, 0);
    glad_glUseProgram(0);
}

void mp_begin_3d_mode(Camera3D* camera){
    camera->cam_matrix = glms_lookat(camera->position, camera->target, camera->up);
    camera->proj_matrix = glms_perspective(camera->fovy * DEG2RAD, camera->aspect, 0.05f, 100.0f);
}

double lastX = 0.0, lastY = 0.0;
double deltaX = 0.0, deltaY = 0.0;
int firstMouse = 1;
vec2s mp_get_mouse_delta(GLFWwindow *window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }
    deltaX = xpos - lastX;
    deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    vec2s delta = (vec2s){.x = deltaX, .y = deltaY};
    return delta;
}

float yaw = 0, pitch = 0;
void mp_update_camera_3d(GLFWwindow *window, Camera3D *camera, float dt){
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

    if(glms_vec3_norm(dir) > 1) dir = glms_vec3_normalize(dir);

    camera->position = glms_vec3_add(camera->position, glms_vec3_scale(dir, dt));
    camera->target = glms_vec3_add(camera->position, tar);
}

float last_time = 0;
float mp_get_frame_time(){
    float curr_time = glfwGetTime();;

    float delta =  curr_time - last_time;
    last_time = curr_time;;
    return delta;
}
