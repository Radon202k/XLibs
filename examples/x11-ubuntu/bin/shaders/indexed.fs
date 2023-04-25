#version 450 core

in vec2 linkedTexCoord;

layout (binding=0)
uniform sampler2D sampler;

layout (location=0)
out vec4 outColor;

void main() {
  outColor = texture(sampler, linkedTexCoord);
}