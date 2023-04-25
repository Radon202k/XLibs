#version 450 core

layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;

layout (location=0) uniform mat4 proj;
layout (location=1) uniform mat4 view;
layout (location=2) uniform mat4 model;

out gl_PerVertex { vec4 gl_Position; };
out vec2 linkedTexCoord;

void main()
{
  gl_Position = proj * view * model * vec4(pos, 1);
  linkedTexCoord = texCoord;
}