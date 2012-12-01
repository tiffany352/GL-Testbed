#include "obj.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "matrix.h"

GLuint mvp_loc;
GLint count;

void testbed_init(int argc, char **argv)
{
  GLuint vbo, vao, vertshader, fragshader, program;
  obj_file f;
  
  if (argc < 2) {
    printf("Usage: %s obj_file", argv[0]);
    exit(EXIT_SUCCESS);
  }
  
  f = obj_readfile(argv[1]);
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  vbo = obj_to_gl(f.first_mesh, &count);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/obj_test.vert"));
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/obj_test.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  il_Graphics_linkProgram(program);
  
  mvp_loc = glGetUniformLocation(program, "mvp");
  
  glUseProgram(program);
  
}

void testbed_draw()
{
  // rotation
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float angle = (tv.tv_sec%12 + (tv.tv_usec/1000000.0))/12 * M_PI * 2;
  sg_Vector3 position = (sg_Vector3){sin(angle)*5, 0, cos(angle)*5};
  sg_Matrix mat = sg_Matrix_mul(
    sg_Matrix_perspective(30, 4/3.0, 0.01, 10),
    sg_Matrix_mul(
      sg_Matrix_rotate_v(-angle + M_PI, (sg_Vector3){0.0995,0.995,0}),
      sg_Matrix_translate(position)
    )
  );
  glUniformMatrix4fv(mvp_loc, 1, GL_TRUE, (const GLfloat*)&mat.data);
  
  glDrawArrays(GL_TRIANGLES, 0, count);
}
