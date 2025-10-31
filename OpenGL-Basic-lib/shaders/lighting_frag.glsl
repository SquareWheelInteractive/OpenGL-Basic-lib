#version 330

out vec4 frag_color;

in vec2 uv;
in vec3 frag_pos;
in vec3 normal;

#define MAX_LIGHTS 4

struct Light {
    int enabled;
    vec3 position;
    vec4 color;
    float radius;
};

uniform sampler2D tex;
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient_color;

void main()
{
    vec3 normal = normalize(normal);
    vec4 texelColor = texture(tex, uv);
    vec4 diffuse = vec4(0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 dir_to_light = normalize(lights[i].position - frag_pos);

            float dot = max(dot(normal, dir_to_light), 0.0);

            float distance = length(lights[i].position - frag_pos);
            float attenuation = (lights[i].radius - distance) / (lights[i].radius + distance);
            attenuation = clamp(attenuation, 0, 1);

            diffuse += normalize(lights[i].color) * dot * attenuation;
        }
    }
    frag_color = texelColor*(ambient_color + diffuse);
}
