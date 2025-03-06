#version 330 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec3 v_Color;
layout(location = 3) in vec2 v_Uv;

out vec3 f_Color;
out vec2 f_Uv;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main(){
    gl_Position = u_Projection * u_View * u_Model * vec4(v_Position, 1.0);

    f_Color = v_Color;
    f_Uv = v_Uv;
}

