#include <GL/glew.h>
#include "obj.h"
#include "glutil.h"
#include "matrix.h"
#include <math.h>
#include <sys/time.h>
#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

GLuint mvp_loc, inv_loc, light_loc;
GLint count;

void testbed_init(int argc, char **argv)
{
  GLuint vbo, vao, vertshader, fragshader, program;
  obj_file f;
  
  f = obj_readfile("data/teapot.obj");
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  vbo = obj_to_gl(f.first_mesh, &count);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/lighting.vert"));
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/lighting.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  il_Graphics_linkProgram(program);
  
  mvp_loc = glGetUniformLocation(program, "mvp");
  inv_loc = glGetUniformLocation(program, "inv");
  light_loc = glGetUniformLocation(program, "light");
  
  glUseProgram(program);
  
  obj_bindmtl_defaults(f.first_mesh->mtl, program);
  
  glEnable(GL_DEPTH_TEST);
}

void testbed_draw()
{
  // rotation
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float angle = (tv.tv_sec%12 + (tv.tv_usec/1000000.0))/12 * M_PI * 2;
  sg_Vector3 position = (sg_Vector3){sin(angle)*15, 0, cos(angle)*15};
  sg_Matrix mat = sg_Matrix_mul(
    sg_Matrix_perspective(30, 4/3.0, 0.01, 50),
    sg_Matrix_mul(
      sg_Matrix_rotate_v(-angle + M_PI, (sg_Vector3){0.0995,0.995,0}),
      sg_Matrix_translate(position)
    )
  );
  glUniformMatrix4fv(mvp_loc, 1, GL_TRUE, (const GLfloat*)&mat.data);
  
  // compute the inverse as a 3x3 matrix
  float inv[9];
  #define A(x,y) mat.data[x*4 + y]
  #define result(x,y) inv[x*3 + y]
  double determinant =    +A(0,0)*(A(1,1)*A(2,2)-A(2,1)*A(1,2))
                          -A(0,1)*(A(1,0)*A(2,2)-A(1,2)*A(2,0))
                          +A(0,2)*(A(1,0)*A(2,1)-A(1,1)*A(2,0));
  double invdet = 1/determinant;
  result(0,0) =  (A(1,1)*A(2,2)-A(2,1)*A(1,2))*invdet;
  result(1,0) = -(A(0,1)*A(2,2)-A(0,2)*A(2,1))*invdet;
  result(2,0) =  (A(0,1)*A(1,2)-A(0,2)*A(1,1))*invdet;
  result(0,1) = -(A(1,0)*A(2,2)-A(1,2)*A(2,0))*invdet;
  result(1,1) =  (A(0,0)*A(2,2)-A(0,2)*A(2,0))*invdet;
  result(2,1) = -(A(0,0)*A(1,2)-A(1,0)*A(0,2))*invdet;
  result(0,2) =  (A(1,0)*A(2,1)-A(2,0)*A(1,1))*invdet;
  result(1,2) = -(A(0,0)*A(2,1)-A(2,0)*A(0,1))*invdet;
  result(2,2) =  (A(0,0)*A(1,1)-A(1,0)*A(0,1))*invdet;
  glUniformMatrix3fv(inv_loc, 1, GL_TRUE, (const GLfloat*)inv);
  
  sg_Vector4 light = sg_Vector4_mul_m((sg_Vector4){5, 5, 5, 1}, mat);
  glUniform3f(light_loc, light.x, light.y, light.z);
  
  glDrawArrays(GL_TRIANGLES, 0, count);
}
