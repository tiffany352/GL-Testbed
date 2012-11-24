#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

char* read_file(const char* name) 
{
  size_t length;
  struct stat st;
  FILE* handle;

  handle = fopen(name, "r");  /* Open file for reading */
  if (!handle) {
    printf("Failed to open file \"%s\": %s", name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  stat(name, &st);
  length = st.st_size;
  char* buf = malloc(length + 1);   /* Allocate a buffer for the file and NUL */
  length = fread(buf, 1, length, handle); /* Read the contents of the file in to the buffer */
  fclose(handle);                   /* Close the file */
  buf = realloc(buf, length+1);
  buf[length] = 0;                  /* Null terminator */
  return buf;
}
