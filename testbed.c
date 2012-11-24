#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>

extern void testbed_init(int argc, char **argv);
extern void testbed_draw();

void testbed(int argc, char **argv)
{
  printf("Starting Testbed\n");
  testbed_init(argc, argv);
  printf("Beginning loop\n");
  while(1) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    testbed_draw();
    glfwSwapBuffers();
    if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE)
      break;
  }
  printf("Shutting down\n");
}
