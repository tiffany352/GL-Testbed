#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "helpers.h"
#include "glutil.h"
#include "matrix.h"

static const struct {
  float pos[4*3*6];
  unsigned char texcoord[4*2*6];
} skybox = {
  { // pos
     0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f,

     0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f,  0.5f,
     
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f
  },
  { // texcoord
    0, 1,  1, 1,  1, 0,  0, 0,
    0, 1,  1, 1,  1, 0,  0, 0,
    0, 1,  1, 1,  1, 0,  0, 0,
    0, 1,  1, 1,  1, 0,  0, 0,
    0, 1,  1, 1,  1, 0,  0, 0,
    0, 1,  1, 1,  1, 0,  0, 0,
  }
};

static unsigned char texdata[] = {
  255, 255, 255, 255,
  0,   0,   255, 255,
  0,   255, 0,   255,
  255, 0,   0,   255
};

GLuint vao, vbo, program, vertshader, fragshader, textures[6], rotation_loc, tex_loc;

void testbed_init(int argc, char **argv)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  printf("skybox.c\n");
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), &skybox, GL_STATIC_DRAW);
  printf("vbo = %u\n", vbo);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/skybox.vert")), 
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/skybox.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  glBindAttribLocation(program, 0, "in_Position");
  glBindAttribLocation(program, 1, "in_Texcoord");
  
  // skybox.pos
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  // skybox.texcoord
  glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, 0, 
    (void*)((char*)&skybox.texcoord - (char*)&skybox));
  glEnableVertexAttribArray(1);
  
  il_Graphics_linkProgram(program);
  
  rotation_loc = glGetUniformLocation(program, "rotation");
  
  tex_loc = glGetUniformLocation(program, "tex");
  
  glGenTextures(6, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, textures[4]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, textures[5]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glUseProgram(program);
  
  glActiveTexture(GL_TEXTURE0);
}

void testbed_draw()
{
  // rotation
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float angle = (tv.tv_sec%4 + (tv.tv_usec/1000000.0))/4 * M_PI * 2;
  sg_Matrix mat = sg_Matrix_mul(
    sg_Matrix_perspective(30, 4/3.0, 0.001, 10),
    sg_Matrix_rotate_v(angle, (sg_Vector3){0.242,0.970,0})
  );
  glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, (const GLfloat*)&mat.data);
  // sampler
  glUniform1i(tex_loc, 0);
  
  // Render the front quad
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  // Render the left quad
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
  // Render the back quad
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
  // Render the right quad
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
  // Render the top quad
  glBindTexture(GL_TEXTURE_2D, textures[4]);
  glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
  // Render the bottom quad
  glBindTexture(GL_TEXTURE_2D, textures[5]);
  glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
