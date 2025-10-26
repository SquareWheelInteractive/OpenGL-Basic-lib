#include "mplib/mplib.h"
#include "mplib/mpgl.h"
#include "cglm/struct.h"

#define WIN_WIDTH 700
#define WIN_HEIGHT 500

int main(){
    GLFWwindow* window = mp_init(WIN_WIDTH, WIN_HEIGHT, "mplib");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera3D cam;
    cam.position = (vec3s){.x = 0, .y = 1.0f, .z = 2};
    cam.target = (vec3s){.x = 0, .y = 0, .z = 0};
    cam.up= (vec3s){.x = 0, .y = 1, .z = 0};
    cam.fovy = 60;
    cam.aspect = (float)WIN_WIDTH/WIN_HEIGHT;

    unsigned int shader = mp_create_shader_program("shaders/vert.glsl", "shaders/frag.glsl");

    MPModel house = mp_load_model_from_mesh(mp_load_obj("./resources/medieval_house.obj"));
    house.shader_program = shader;
    Texture m_tex = mp_load_texture("./resources/houseTexture.png");
    house.albedo = m_tex;

    MPModel ground = mp_load_model_from_mesh(mp_load_obj("./resources/ground.obj"));
    for (int i = 0; i < ground.mesh.vertex_count; i++) {
        ground.mesh.tex_coords[i*2+0] *= 1.5f;
        ground.mesh.tex_coords[i*2+1] *= 1.5f;
    }
    mgl_update_attrib(ground.mesh, 1, ground.mesh.vertex_count * 2 * sizeof(float), ground.mesh.tex_coords);
    ground.shader_program = shader;
    Texture grass_tex = mp_load_texture("./resources/grass.png");
    ground.albedo = grass_tex;

    while (!mp_window_should_close(window)) {
        float dt = mp_get_frame_time();
        mp_update_camera_3d(window, &cam, 4, dt);

        mp_begin_drawing(window, (vec3s){.r = 0.43f, 0.53f, 0.75f});

            mp_begin_3d_mode(&cam);

            mp_draw_model(house, cam);
            mp_draw_model(ground, cam);

        mp_end_drawing(window);
    }

    mp_unload_model(house);

    mp_terminate();
    return -1;
}
