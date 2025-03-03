#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Remove translation from the view matrix
    mat4 viewNoTrans = mat4(mat3(view));
    vec4 pos = projection * viewNoTrans * vec4(aPos, 1.0);
    gl_Position = pos.xyww;  // Force depth to 1.0
    TexCoords = aPos;
}
