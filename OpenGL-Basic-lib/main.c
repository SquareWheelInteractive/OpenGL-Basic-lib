#include "mplib/mplib.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main(){
    GLFWwindow* window = mp_init(WIN_WIDTH, WIN_HEIGHT, "pingo");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera3D cam;
    cam.position = (vec3s){.x = 0, .y = 1.0f, .z = 2};
    cam.target = (vec3s){.x = 0, .y = 0, .z = 0};
    cam.up= (vec3s){.x = 0, .y = 1, .z = 0};
    cam.fovy = 65;
    cam.aspect = (float)WIN_WIDTH/WIN_HEIGHT;

    unsigned int shader = mp_create_shader_program("shaders/vert.glsl", "shaders/frag.glsl");

    MPModel house = mp_load_model_from_mesh(mp_load_obj("medieval_house.obj"));
    house.shader_program = shader;
    Texture m_tex = mp_load_texture("./houseTexture.png");
    house.albedo = m_tex;

    while (!mp_window_should_close(window)) {
        float dt = mp_get_frame_time();
        mp_update_camera_3d(window, &cam, 2, dt);

        mp_begin_drawing(window);

            mp_begin_3d_mode(&cam);

            mp_draw_model(house, cam);

        mp_end_drawing(window);
    }
    mp_unload_model(house);
    mp_terminate();
    return -1;
}
