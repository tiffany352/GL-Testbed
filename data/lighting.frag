#version 140

in vec2 texcoord;
in vec3 normal;
in vec3 model_normal;
out vec3 color;
uniform vec3 light;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float specular_co;

void main()
{
  vec3 L = normalize(light - gl_FragCoord.xyz);
  vec3 E = normalize(-gl_FragCoord.xyz);
  vec3 R = normalize(reflect(-L, normal));
  
  color = ambient;
  color += clamp(diffuse * dot(L, model_normal), 0.0, 1.0);
  color += clamp(pow(max(dot(R, E), 0), 0.3*specular_co) * specular, 0.0, 1.0);
}
