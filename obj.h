#include <GL/glew.h>

typedef float obj_vertex[4];

typedef float obj_texcoord[3];

typedef float obj_normal[3];

typedef float obj_color[3];

typedef struct obj_face {
  struct obj_face *next;
  obj_vertex vertices[4];
  obj_texcoord texcoords[4];
  obj_normal normals[4];
  int vertices_index[4], texcoords_index[4], normals_index[4];
  int num;
} obj_face;

typedef struct obj_mtl {
  const char *name;
  obj_color ambient, diffuse, specular;
  float specular_co, transparency;
  int illumination;
} obj_mtl;

typedef struct obj_mesh {
  struct obj_mesh *next;
  obj_face *first_face;
  obj_mtl mtl;
  const char *name, *group;
  int smoothing;
} obj_mesh;

typedef struct obj_file {
  obj_mesh * first_mesh;
  const char *name;
} obj_file;

typedef const char* (*obj_reader)(const char *name);

const char* obj_stdio_reader(const char *filename);

obj_file obj_readstring(const char * data, obj_reader inc, const char *filename);

obj_file obj_readfile(const char *filename);

enum obj_vertextype {
  OBJ_NOVERTICES = 0,
  OBJ_X = 1,
  OBJ_Y = 2,
  OBJ_Z = 4,
  OBJ_W = 8,
  OBJ_XYZ = OBJ_X|OBJ_Y|OBJ_Z,
  OBJ_XYZW = OBJ_XYZ|OBJ_W
};

enum obj_texcoordtype {
  OBJ_NOTEXCOORDS = 0,
  OBJ_U = 1,
  OBJ_V = 2,
  // OBJ_W (already defined)
  OBJ_UV = OBJ_U|OBJ_V,
  OBJ_UVW = OBJ_UV|OBJ_W
};

enum obj_normaltype {
  OBJ_NONORMALS = 0,
  OBJ_NORMALS = 1
};

enum obj_facetype {
  OBJ_TRIANGLES,
  OBJ_QUADS
};

enum obj_vbolayout {
  OBJ_INTERLEAVED,
  OBJ_TIGHT
};

GLfloat *obj_to_vbo(obj_mesh *mesh, enum obj_vertextype vertex, 
  enum obj_texcoordtype texcoord, enum obj_normaltype normal, 
  enum obj_facetype face, enum obj_vbolayout layout, size_t *size);

GLuint obj_to_gl(obj_mesh *mesh, GLint *count);
