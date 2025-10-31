#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_norm;

out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    uv = v_uv;

    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(v_pos, 1.0f);
}
