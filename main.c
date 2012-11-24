#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef width
#define width 800
#endif
#ifndef height
#define height 600
#endif 

void testbed(int argc, char **argv);

int GLFWCALL exit_cb()
{
  printf("hi\n");
  return GL_TRUE;
}

int main(int argc, char **argv) 
{
  if (!glfwInit()) {
    printf("glfwInit() failed\n");
    exit(EXIT_FAILURE);
  }
  
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  if (!glfwOpenWindow(width, height, 8, 8, 8, 8, 32, 8, GLFW_WINDOW)) {
    printf("Failed to create window\n");
    exit(-1);
  }
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    printf("glewInit() failed: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }
  if (!GLEW_VERSION_3_1) {
    printf("GL 3.1 does not appear to be present, trying anyway\n");
  }
  
  glfwSetWindowTitle("GL Testbed");
  glfwSwapInterval(1); // swap every vsync
  
  glfwSetWindowCloseCallback(&exit_cb);
  
  testbed(argc, argv); // start the testbed
  
  printf("Destroying window\n");
  glfwCloseWindow();
  glfwTerminate();
  
  exit(EXIT_SUCCESS);
  
  return 0;
}
