#version 450 core

layout (location=0) in vec2 pos;
layout (location=1) in vec2 texCoord;
layout (location=2) in vec4 color;

layout (location=0) uniform mat4 proj;

out gl_PerVertex { vec4 gl_Position; };
out vec2 linkedTexCoord;
out vec4 linkedColor;

void main()
{
  gl_Position = proj * vec4(pos, 0, 1);
  linkedTexCoord = texCoord;
  linkedColor = color;
}