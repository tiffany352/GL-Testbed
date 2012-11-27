#version 140

in vec3 pos;
in vec3 normal;
out vec4 color;
uniform sampler2D tex[3];

void main()
{
  /*vec3 min_color = vec3(0.0, 1.0, 0.2);
  vec3 max_color = vec3(0.4, 1.0, 0.5);
  color = vec4((max_color - min_color)*(1-pos.y/10) + min_color, 1.0);*/
  vec2 texcoord = vec2(pos.x - floor(pos.x), pos.z - floor(pos.z));
  vec4 grass  = texture(tex[0], texcoord);
  vec4 rock   = texture(tex[1], texcoord);
  vec4 snow   = texture(tex[2], texcoord);
  vec4 vegetation = mix(rock, grass, dot(vec3(0,1,0), normal));
  vec4 frost = vec4(snow.xyz, (1-clamp(pos.y/10, 0, 1)) * dot(vec3(0,1,0), normal));
  //color = mix(vec4(0), frost, frost.a);
  color = mix(vegetation, frost, frost.a);
  //color = vec4(normal, 1.0);
}
