#version 450 core

in vec2 linkedTexCoord;
in vec4 linkedColor;

layout (binding=0)
uniform sampler2D sampler;

layout (location=0)
out vec4 outColor;

void main() {
  vec4 texel = texture(sampler, linkedTexCoord);
  outColor = linkedColor * texel;
}