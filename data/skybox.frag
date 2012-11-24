#version 140

out vec3 color;
uniform sampler2D tex;
in vec2 texcoord;

void main(void)
{
  color = texture(tex, texcoord).rgb;
  gl_FragDepth = 1.0;
}
