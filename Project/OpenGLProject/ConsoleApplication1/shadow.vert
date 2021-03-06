#version 150
in vec4 a_position;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out float v_viewPosz;

void main(){
    vec4 p = a_position * worldMatrix;
    p = p * viewMatrix;
    //negate because RHS
    v_viewPosz = -p.z;
    gl_Position = p * projMatrix;
}