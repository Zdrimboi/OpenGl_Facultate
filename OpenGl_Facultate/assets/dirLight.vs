#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTex;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
} vs_out;

uniform mat4 model, view, projection;

void main(){
    vec4 worldPos = model * vec4(aPos,1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal  = mat3(transpose(inverse(model))) * aNormal;
    vs_out.Tex     = aTex;
    gl_Position    = projection * view * worldPos;
}
