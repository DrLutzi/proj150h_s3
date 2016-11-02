#version 330

uniform vec3 fsu_color;

out vec4 color_final;

void main()
{
    color_final = fsu_color;
}
