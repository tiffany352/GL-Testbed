#include <GL/glew.h>
#include <GL/glfw.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "glutil.h"
#include "helpers.h"
#include "data/grass2.h"
#include "data/snow.h"
#include "data/rock.h"

#define MAX_LOD 1
GLuint program, vertshader, fragshader, vao, vbo[MAX_LOD], tex, mvp_loc, offset_loc, tex_loc;
int vbo_bound;

GLuint load_img(int width, int height, char *ptr)
{
  int i;
  GLuint tex;
  unsigned char *img = malloc(width*height*3);
  for (i = 0; i < width*height; i++) {
    HEADER_PIXEL(ptr, (img + i*3));
  }
  
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
    GL_RGB, GL_UNSIGNED_BYTE, img);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  free(img);
  return tex;
}

void testbed_init(int argc, char **argv)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  // 100 rows
  // 2 vertices per column edge
  // 2 points per vertex
  // 100 columns
  // 20200 floats
  int row, col, colsize=4, rowsize=colsize*101;
  short *buf = malloc(rowsize * 100 * sizeof(short));
  for (row = 0; row < 100; row++) {
    for (col = 0; col < 101; col++) {
      buf[row*rowsize + col*colsize + 0] = row;
      buf[row*rowsize + col*colsize + 1] = col;
      buf[row*rowsize + col*colsize + 2] = row+1;
      buf[row*rowsize + col*colsize + 3] = col;
    }
  }
  
  glGenBuffers(1, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, rowsize * 100 * sizeof(short), buf, 
    GL_STATIC_DRAW);
  free(buf);
  vbo_bound = 1;
  
  glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  
  glActiveTexture(GL_TEXTURE0);
  load_img(grass_width, grass_height, grass_header_data);
  glActiveTexture(GL_TEXTURE0 + 1);
  load_img(rock_width, rock_height, rock_header_data);
  glActiveTexture(GL_TEXTURE0 + 2);
  load_img(snow_width, snow_height, snow_header_data);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/heightmap.vert"));
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/heightmap.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  il_Graphics_linkProgram(program);
  
  mvp_loc = glGetUniformLocation(program, "mvp");
  offset_loc = glGetUniformLocation(program, "offset");
  tex_loc = glGetUniformLocation(program, "tex");
  
  glUseProgram(program);
  
  GLint tex[] = {0, 1, 2};
  glUniform1iv(tex_loc, 3, &tex[0]);
  
  glEnable(GL_DEPTH_TEST);
}

void render_swath(int x, int y, int lod)
{
  glUniform2f(offset_loc, x, y);
  if (vbo_bound != lod) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[lod-1]);
    vbo_bound = lod;
  }
  
  GLint *first = malloc(sizeof(GLint) * (100/lod));
  GLsizei *count = malloc(sizeof(GLsizei) * (100/lod));
  int i, row = (100/lod + 1) * 2;
  for (i = 0; i < 100/lod; i++) {
    first[i] = i * row;
    count[i] = row;
  }
  glMultiDrawArrays(GL_TRIANGLE_STRIP, first, count, 100/lod);
  free(first);
  free(count);
}

void testbed_draw(int argc, char **argv)
{
  // rotation
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float angle = (tv.tv_sec%12 + (tv.tv_usec/1000000.0))/12 * M_PI * 2;
  sg_Matrix mat = sg_Matrix_mul(
    sg_Matrix_perspective(30, 4/3.0, 0.1, 1000),
    sg_Matrix_mul(
      sg_Matrix_rotate_v(angle, (sg_Vector3){0.0995,0.995,0}),
      sg_Matrix_translate((sg_Vector3){-50, 12, -50})
    )
  );
  glUniformMatrix4fv(mvp_loc, 1, GL_TRUE, (const GLfloat*)&mat.data);
  
  int x,y;
  for (y = -1; y <= 1; y++) {
    for (x = -1; x <= 1; x++) {
      render_swath(x*100, y*100, 1);
    }
  }  
}
