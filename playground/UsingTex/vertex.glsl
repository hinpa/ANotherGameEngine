#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec4 color;
out vec2 texCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    color = vec4(aPos, 1.0f);
    texCoord = aTexCoord;
}
