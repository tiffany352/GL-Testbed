#version 140

in  vec3 in_Position;
uniform mat4 rotation;
in vec2 in_Texcoord;
out vec2 texcoord;

void main(void) 
{
  gl_Position = rotation * vec4(in_Position, 1.0);
  texcoord = in_Texcoord;
}
