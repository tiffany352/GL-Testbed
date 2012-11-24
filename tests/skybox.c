#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>
#include <math.h>

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
     0.5f,  0.5f,  0.5f
     
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
    0, 0,  1, 0,  1, 1,  0, 1,
    0, 0,  1, 0,  1, 1,  0, 1,
    0, 0,  1, 0,  1, 1,  0, 1,
    0, 0,  1, 0,  1, 1,  0, 1,
    0, 0,  1, 0,  1, 1,  0, 1,
    0, 0,  1, 0,  1, 1,  0, 1 
  }
};

static unsigned char texdata[] = {
  0,   0,   0,   255,
  0,   0,   255, 255,
  0,   255, 0,   255,
  255, 0,   0,   255
};

GLuint vao, vbo, program, vertshader, fragshader, textures[6], rotation_loc, tex_loc;

void testbed_init(int argc, char **argv)
{
  printf("skybox.c\n");
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), &skybox, GL_STATIC_DRAW);
  printf("vbo = %u\n", vbo);
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  // skybox.pos
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  // skybox.texcoord
  glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, 0, 
    (void*)((char*)&skybox.texcoord - (char*)&skybox));
  printf("(char*)&skybox.texcoord - (char*)&skybox == %p\n", 
    (char*)&skybox.texcoord - (char*)&skybox);
  glEnableVertexAttribArray(1);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/skybox.vert")), 
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/skybox.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  il_Graphics_linkProgram(program);
  
  rotation_loc = glGetUniformLocation(program, "rotation");
  tex_loc = glGetUniformLocation(program, "tex");
  
  glGenTextures(6, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glBindTexture(GL_TEXTURE_2D, textures[4]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);
  glBindTexture(GL_TEXTURE_2D, textures[5]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata[0]);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glUseProgram(program);
  
  glActiveTexture(GL_TEXTURE0);
}

void testbed_draw()
{
  // rotation
  sg_Matrix mat = sg_Matrix_perspective(90, 4/3.0, 0.001, sqrt(2));
  glUniformMatrix4fv(rotation_loc, 1, GL_TRUE, (const GLfloat*)&mat.data);
  // sampler
  glUniform1i(tex_loc, 0);
  
  // Render the front quad
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // Render the left quad
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
  // Render the back quad
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
  // Render the right quad
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
  // Render the top quad
  glBindTexture(GL_TEXTURE_2D, textures[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
  // Render the bottom quad
  glBindTexture(GL_TEXTURE_2D, textures[5]);
  glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}
