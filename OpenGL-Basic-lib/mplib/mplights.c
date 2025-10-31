#include "mplights.h"
#include "mplib.h"

unsigned char light_num = 0;
Light mp_create_light(vec3s position, Color color, float radious, unsigned int shader){
    Light light = {0};
    if(light_num < MAX_LIGHTS){
        light.enabled = 1;
        light.position = position;
        light.color = color;
        light.radius = radious;

        light.enable_loc = glad_glGetUniformLocation(shader, mp_format_text("lights[%i].enabled",light_num));
        light.pos_loc    = glad_glGetUniformLocation(shader, mp_format_text("lights[%i].position",light_num));
        light.color_loc  = glad_glGetUniformLocation(shader, mp_format_text("lights[%i].color"   ,light_num));
        light.radius_loc = glad_glGetUniformLocation(shader, mp_format_text("lights[%i].radius"  ,light_num));

        mp_update_light_values(light, shader);
        light_num++;
    }
    return light;
}
void mp_update_light_values(Light light, unsigned int shader){
    glad_glUseProgram(shader);

    glad_glUniform1i(light.enable_loc, light.enabled);
    glad_glUniform3f(light.pos_loc, light.position.x, light.position.y, light.position.z);
    glad_glUniform4f(light.color_loc, light.color.r, light.color.g, light.color.b, light.color.a);
    glad_glUniform1f(light.radius_loc, light.radius);

    glad_glUseProgram(0);
}
