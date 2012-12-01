#version 140

in vec3 pos;
in vec3 normal;
in vec3 light_color;
out vec4 color;
uniform sampler2D tex[3];

void main()
{
  /*vec3 min_color = vec3(0.0, 1.0, 0.2);
  vec3 max_color = vec3(0.4, 1.0, 0.5);
  color = vec4((max_color - min_color)*(1-pos.y/10) + min_color, 1.0);*/
  vec2 texcoord = vec2(pos.x - floor(pos.x/3)*3, pos.z - floor(pos.z/3)*3);
  vec4 grass  = texture(tex[0], texcoord/3);
  vec4 rock   = texture(tex[1], texcoord/3);
  vec4 snow   = texture(tex[2], texcoord/3);
  vec4 vegetation = mix(rock, grass, (clamp(pos.y/15, 0, 1)) * (1-clamp(pow(dot(vec3(0,1,0), normal), 20), 0, 1)));
  vec4 frost = vec4(snow.xyz, (1-clamp(pos.y/10, 0, 1)) * pow(dot(vec3(0,1,0), normal), 1.5));
  //color = mix(vec4(0), frost, frost.a);
  color = mix(vegetation, frost, frost.a) * vec4(light_color, 1.0);
  //color = vec4(normal, 1.0);
}
