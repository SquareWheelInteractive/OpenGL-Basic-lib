#include "mplib/mplib.h"
#include "mplib/mpgl.h"
#include "cglm/struct.h"
#include <GLFW/glfw3.h>
#include "mplib/mplights.h"

#define WIN_WIDTH 1000
#define WIN_HEIGHT 800

int main(){
    GLFWwindow* window = mp_init(WIN_WIDTH, WIN_HEIGHT, "mplib");

    Camera3D cam;
    cam.position = (vec3s){.x = 0, .y = 1.0f, .z = 2};
    cam.target = (vec3s){.x = 0, .y = 0, .z = 0};
    cam.up= (vec3s){.x = 0, .y = 1, .z = 0};
    cam.fovy = 60;
    cam.aspect = (float)WIN_WIDTH/WIN_HEIGHT;

    unsigned int shader = mp_create_shader("shaders/lighting_vert.glsl", "shaders/lighting_frag.glsl");

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

    Light lights[3];
    lights[0]= mp_create_light(POINT_LIGHT_TYPE, (vec3s){.x=4, .y=1, .z= 0}, (Color){1, 0, 0, 1.0f}, 8, glms_vec3_zero(), shader);
    lights[1]= mp_create_light(POINT_LIGHT_TYPE, (vec3s){.x=-4, .y=1, .z=0}, (Color){0, 1, 0, 1.0f}, 8, glms_vec3_zero(), shader);
    lights[2]= mp_create_light(POINT_LIGHT_TYPE, (vec3s){.x=0, .y=1, .z=4}, (Color){0, 0, 1, 1.0f}, 8, glms_vec3_zero(), shader);

    Light spot = mp_create_light(SPOT_LIGHT_TYPE, (vec3s){.x=0, .y=1, .z=4}, (Color){1, 1, 1, 1.0f}, 25, glms_vec3_zero(), shader);

    int view_pos_loc = glad_glGetUniformLocation(shader, "view_pos");

    while (!mp_window_should_close(window)) {
        float dt = mp_get_frame_time();
        mp_update_camera_3d(window, &cam, 4, dt);

        glUseProgram(shader);
        glad_glUniform3f(view_pos_loc, cam.position.x, cam.position.y, cam.position.z);
        glUseProgram(0);

        spot.position = cam.position;
        spot.direction = glms_vec3_sub(cam.target, cam.position);
        mp_update_light_values(spot, shader);

        for (int i = 0; i < 3; i++) {
            vec4s pos = (vec4s){.x = lights[i].position.x,
                                .y = lights[i].position.y,
                                .z = lights[i].position.z,
                                1.0f};
            pos = glms_mat4_mulv(glms_rotate_y(glms_mat4_identity(), 50 * DEG2RAD * dt), pos);
            lights[i].position.x = pos.x;
            lights[i].position.y = pos.y;
            lights[i].position.z = pos.z;

            mp_update_light_values(lights[i], shader);
        }

        mp_begin_drawing(window, DARK_GRAY);
            mp_begin_3d_mode(&cam);
          
            mp_draw_model(house,  cam, DARK_GRAY);
            mp_draw_model(ground, cam, DARK_GRAY);

        mp_end_drawing(window);
    }

    mp_unload_model(house);

    mp_terminate();
    return -1;
}
