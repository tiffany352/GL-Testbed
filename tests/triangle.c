#include <GL/glew.h>
#include <GL/glfw.h>

GLfloat triangle[] = {
  -1.0, -1.0,
   0.0,  1.0,
   1.0, -1.0
};

GLuint vao, vbo, program, vertshader, fragshader;

void testbed_init(int argc, char **argv)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  
  vertshader = il_Graphics_makeShader(GL_VERTEX_SHADER, 
    read_file("data/triangle.vert")), 
  fragshader = il_Graphics_makeShader(GL_FRAGMENT_SHADER, 
    read_file("data/triangle.frag"));
  
  program = glCreateProgram();
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  
  il_Graphics_linkProgram(program);
  
  glUseProgram(program);
}

void testbed_draw()
{
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
