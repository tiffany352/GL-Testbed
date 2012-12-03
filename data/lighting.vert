#version 140

in vec4 in_Position;
in vec3 in_Texcoord;
in vec3 in_Normal;
out vec2 texcoord;
out vec3 normal;
out vec3 model_normal;
uniform mat4 mvp;
uniform mat3 inv;

void main()
{
  texcoord = in_Texcoord.xy;
  normal = normalize(inv * in_Normal);
  model_normal = in_Normal;
  gl_Position = mvp * in_Position;
}
