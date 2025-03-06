#version 330 core
out vec4 FragColor;

in vec3 f_Color;
in vec2 f_Uv;

uniform sampler2D texture_atlas;

void main(){
    FragColor = vec4(texture(texture_atlas, f_Uv).rgb, 1.0); // * vec4(f_Color, 1.0);
}