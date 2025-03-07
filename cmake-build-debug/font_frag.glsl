#version 330 core
out vec4 FragColor;

in vec3 f_Color;
in vec2 f_Uv;

uniform sampler2D u_FontAtlas;

void main(){
    vec4 col = texture(u_FontAtlas, f_Uv);

    if((col.r == 0.0 && col.g == 0.0 && col.b == 0.0) || col.a == 0.0){
        discard;
    }

    FragColor = texture(u_FontAtlas, f_Uv) * vec4(f_Color, 1.0);
}