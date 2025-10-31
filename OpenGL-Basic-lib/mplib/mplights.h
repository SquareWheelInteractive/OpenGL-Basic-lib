#pragma once
#include "cglm/struct.h"
#include "mplib.h"

#define MAX_LIGHTS 4

typedef struct{
    vec3s position;
    Color color;
    bool enabled;
    float radius;

    //shader locations
    int pos_loc;
    int color_loc;
    int enable_loc;
    int radius_loc;
} Light;

Light mp_create_light(vec3s position, Color color, float radious, unsigned int shader);
void mp_update_light_values(Light light, unsigned int shader);
