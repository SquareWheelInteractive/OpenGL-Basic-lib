#pragma once
#include "cglm/struct.h"
#include "mplib.h"

#define MAX_LIGHTS 4

typedef enum{
    POINT_LIGHT_TYPE = 0,
    SPOT_LIGHT_TYPE  = 1,
} Light_type;

typedef struct{
    int type;
    vec3s position;
    vec3s direction;
    Color color;
    bool enabled;
    float radius;

    //shader locations
    int type_loc;
    int dir_loc;
    int pos_loc;
    int color_loc;
    int enable_loc;
    int radius_loc;
} Light;

Light mp_create_light(int type, vec3s position, Color color, float radious, vec3s direction, unsigned int shader);
void mp_update_light_values(Light light, unsigned int shader);
