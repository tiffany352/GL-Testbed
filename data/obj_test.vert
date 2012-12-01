#version 140

in vec4 in_Position;
in vec3 in_Texcoord;
in vec3 in_Normal;
uniform mat4 mvp;

void main()
{
  gl_Position = mvp * in_Position;
}
