#version 140

out vec4 color;
uniform sampler2D tex;
in vec2 texcoord;

void main(void)
{
  color = texture(tex, texcoord);
  gl_FragDepth = 1.0;
}
