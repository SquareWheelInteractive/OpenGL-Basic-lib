#version 330 core

out vec4 frag_color;

in vec2 uv;
uniform sampler2D tex;
uniform vec3 ambient_color;

void main(){
    frag_color = texture(tex, uv) * vec4(ambient_color, 1.0f);
}
