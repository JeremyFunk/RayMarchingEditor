#version 430 core

out vec4 fragColor;

in vec2 f_texCoords;
uniform sampler2D tex;
layout(r16ui, binding = 1) uniform uimage2D tex_samples;

uniform int samples;
uniform int total_samples;
uniform vec2 u_resolution;

void main()
{
    //fragColor = vec4(texture(tex, f_texCoords.xy).rgb * (float(total_samples) / float(samples)), 1.0);

    vec3 epic = imageLoad(tex_samples, ivec2(f_texCoords * u_resolution)).rgb;

    fragColor = vec4(texture(tex, f_texCoords).rgb * (total_samples / epic.x), 1.0);

    //fragColor = vec4(epic * 0.2, 1.0);
}