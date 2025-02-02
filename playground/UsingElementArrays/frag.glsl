#version 330 core

out vec4 FragColor;
in vec4 color;

uniform sampler2D tex;
layout(binding = 1) uniform sampler2D tex2;

void main() {
    FragColor = color;
}
