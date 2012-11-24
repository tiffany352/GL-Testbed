#include "glutil.h"

#include <stdlib.h>
#include <GL/glew.h>
#include <stdarg.h>
#include <stdio.h>

#include "matrix.h"

const char * il_Graphics_strerror(GLenum err) {
  char * res;
  switch(err) {
    case GL_NO_ERROR:           res = "GL_NO_ERROR"; break;
    case GL_INVALID_ENUM:       res = "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE:      res = "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:  res = "GL_INVALID_OPERATION"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  
      res = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
    case GL_OUT_OF_MEMORY:      res = "GL_OUT_OF_MEMORY"; break;
    case GL_STACK_OVERFLOW:     res = "GL_STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW:    res = "GL_STACK_UNDERFLOW"; break;
    default:                    res = "UNKNOWN ERROR"; break;
  }
  return res;
}

void il_Graphics_testError_(const char *file, int line, const char *func, 
  const char* fmt, ...) {
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    printf("%s:%i (%s): ", file, line, func);
  
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    
    printf(": %s (%i)\n", il_Graphics_strerror(err), err);
  }
}

GLuint il_Graphics_makeShader(GLenum type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  
  GLint status, len;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
  if (len > 1) {
    char * str = calloc(1, len);
    glGetShaderInfoLog(shader, len, NULL, str);
    printf(
    "Shader info log: \n"
    "---- BEGIN SHADER INFO LOG ----\n"
    "%s\n"
    "---- END SHADER INFO LOG ----\n", str);
    free(str);
  }

  return shader;
}

void il_Graphics_linkProgram(GLuint program) {
  glLinkProgram(program);
  
  GLint status, len;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
  if (len > 1) {
    char * str = calloc(1, len);
    glGetProgramInfoLog(program, len, NULL, str);
    printf( "Program info log: \n"
    "---- BEGIN PROGRAM INFO LOG ----\n"
    "%s\n"
    "---- END PROGRAM INFO LOG ----\n", str);
    free(str);
  }
  
  glValidateProgram(program);
  
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
  if (len > 1) {
    char * str = calloc(1, len);
    glGetProgramInfoLog(program, len, NULL, str);
    printf( "Program info log: \n"
    "---- BEGIN PROGRAM INFO LOG ----\n"
    "%s\n"
    "---- END PROGRAM INFO LOG ----\n", str);
    free(str);
  }
}
