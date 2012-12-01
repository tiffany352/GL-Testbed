#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "obj.h"
#include <GL/glew.h>

enum obj_line_type {
  OBJ_ERROR,
  OBJ_COMMENT,
  OBJ_VERTEX,
  OBJ_TEXCOORD,
  OBJ_NORMAL,
  OBJ_FACE,
  OBJ_MTLLIB,
  OBJ_USEMTL,
  OBJ_ONAME,
  OBJ_GNAME,
  OBJ_SMOOTHING
};

struct obj_line {
  struct obj_line *next;
  enum obj_line_type type;
  union {
    obj_vertex vertex;
    obj_texcoord texcoord;
    obj_normal normal;
    obj_face face;
    const char *mtllib, *usemtl, *oname, *gname, *smoothing, *error;
  } line;
};

struct obj_ctx {
  struct obj_line *first;
  int line;
};

static struct obj_line* obj_lookup(const struct obj_ctx* ctx, enum obj_line_type t, int i)
{
  struct obj_line *cur = ctx->first;
  while (cur) {
    if (cur->type == t)
      i--;
    if (i == 0)
      return cur;
    cur = cur->next;
  }
}

static struct obj_line *obj_readline(const struct obj_ctx* ctx, const char *line)
{
  struct obj_line *res = calloc(1, sizeof(struct obj_line));
  int line_len = strlen(line);
  {
    int len = 0;
    sscanf(line, "%*[ \t\n]%n", &len);
    if (len == line_len) {
      res->type = OBJ_COMMENT; // treat empty lines as comments
      return res;
    }
  }
  switch(line[0]) {
    case '#':
      res->type = OBJ_COMMENT;
      break;
    case 'v':
      switch(line[1]) {
        case ' ': 
        case '\t':
        case '\n': {
          res->type = OBJ_VERTEX;
          float *p = &res->line.vertex[0];
          int len, matched;
          matched = sscanf(line+2, "%f %f %f", p, p+1, p+2);
          if (matched < 3) {
            res->type = OBJ_ERROR;
            res->line.error = "Too few components in vertex";
            break;
          }
          matched += sscanf(line+2, "%*f %*f %*f %f%n", p+3, &len);
          if (matched < 4)
            res->line.vertex[3] = 1.0;
          if (len + 2 < line_len) {
            res->type = OBJ_ERROR;
            res->line.error = "Junk data after vertex";
          }
          break;
        }
        case 't': {
          res->type = OBJ_TEXCOORD;
          float *p = &res->line.texcoord[0];
          int matched, len;
          matched = sscanf(line+2, "%f %f%n", p, p+1, &len);
          if (matched < 2) {
            res->type = OBJ_ERROR;
            res->line.error = "Too few components in texcoord";
            break;
          }
          matched += sscanf(line+2, "%*f %*f %f%n", p+2, &len);
          if (len + 2 < line_len) {
            res->type = OBJ_ERROR;
            res->line.error = "Junk data after texcoord";
          }
          break;
        }
        case 'n': {
          res->type = OBJ_NORMAL;
          float *p = &res->line.normal[0];
          int len, matched;
          matched = sscanf(line+2, "%f %f %f%n", p, p+1, p+2, &len);
          if (matched < 3) {
            res->type = OBJ_ERROR;
            res->line.error = "Too few components in normal";
          }
          if (len + 2 < line_len) {
            res->type = OBJ_ERROR;
            res->line.error = "Junk data after normal";
          }
          break;
        }
        default: 
          res->type = OBJ_ERROR;
          res->line.error = "Invalid/corrupt line";
          break;
      }
      break;
    case 'f': {
      res->type = OBJ_FACE;
      int *v  = &res->line.face.vertices_index[0],
          *vt = &res->line.face.texcoords_index[0],
          *vn = &res->line.face.normals_index[0];
      int matched, len, i;
      const char *p = line + 2;
      for (i = 0; i < 4; i++) {
        len = 0; // in case it doesn't match
        matched = sscanf(p, "%*[ \t\n]%n", &len);
        p += len;
        matched = sscanf(p, "%i%n", v+i, &len);
        if (!matched) goto face_error;
        p += len;
        if (*p != '/') continue;
        p++;
        len = 0; // in case it doesn't match
        matched = sscanf(p, "%i%n", vt+i, &len);
        p += len;
        if (*p != '/') continue;
        p++;
        matched = sscanf(p, "%i%n", vn+i, &len);
        if (!matched) goto face_error;
        p += len;
      }
      res->line.face.num = i;
      for (i = 0; i < 4; i++) {
        struct obj_line *o;
        o = obj_lookup(ctx, OBJ_VERTEX, v[i]);
        if (o)
          memcpy(&res->line.face.vertices[i], &o->line.vertex, sizeof(obj_vertex));
        o = obj_lookup(ctx, OBJ_TEXCOORD, vt[i]);
        if (o)
          memcpy(&res->line.face.texcoords[i], &o->line.texcoord, sizeof(obj_texcoord));
        o = obj_lookup(ctx, OBJ_NORMAL, vn[i]);
        if (o)
          memcpy(&res->line.face.normals[i], &o->line.normal, sizeof(obj_normal));
      }
      break;
      face_error:
      res->type = OBJ_ERROR;
      res->line.error = "Invalid face definition";
      break;
    }
    case 'm':
      res->type = OBJ_MTLLIB;
      res->line.mtllib = line + 7;
      break;
    case 'u':
      res->type = OBJ_USEMTL;
      res->line.usemtl = line + 7;
      break;
    case 'o':
      res->type = OBJ_ONAME;
      res->line.oname = line + 2;
      break;
    case 'g':
      res->type = OBJ_GNAME;
      res->line.gname = line + 2;
      break;
    case 's':
      res->type = OBJ_SMOOTHING;
      res->line.smoothing = line + 2;
      break;
    default:
      res->type = OBJ_ERROR;
      res->line.error = "Invalid line";
  }
  return res;
}

static obj_mesh* create_mesh()
{
  obj_mesh* mesh = calloc(1, sizeof(obj_mesh));
  
  mesh->mtl = (obj_mtl){
    NULL,       // name
    {1, 1, 1},  // ambient
    {0, 0, 0},  // diffuse
    {0, 0, 0},  // specular
    0.0,        // specular coefficient
    1.0,        // transparency
    0           // illumination
  };
  
  return mesh;
}

enum mtl_linetype {
  MTL_ERROR,
  MTL_COMMENT,
  MTL_NEWMTL,
  MTL_AMBIENT,
  MTL_DIFFUSE,
  MTL_SPECULAR,
  MTL_SPECULAR_CO,
  MTL_TRANSPARENCY,
  MTL_ILLUM,
};

struct mtl_file {
  struct mtl_file *next;
  obj_mtl mtl;
};

struct mtl_line {
  enum mtl_linetype type;
  union {
    const char *error, *newmtl;
    obj_color color;
    float specular_co, transparency;
    int illum;
  } line;
};

static struct mtl_line mtl_readline(const char *line)
{
  struct mtl_line res;
  memset(&res, 0, sizeof(struct mtl_line));
  int line_len = strlen(line);
  {
    int len = 0;
    sscanf(line, "%*[ \t\n]%n", &len);
    if (len == line_len) {
      res.type = OBJ_COMMENT; // treat empty lines as comments
      return res;
    }
  }
  
  switch (line[0]) {
    case '#':
      res.type = MTL_COMMENT;
      break;
    case 'n':
      res.type = MTL_NEWMTL;
      res.line.newmtl = line + 7;
      break;
    case 'K': {
      float *c = &res.line.color[0];
      int matched, len;
      matched = sscanf(line + 3, "%f %f %f%n", c, c+1, c+2, &len);
      if (matched < 3) {
        res.type = MTL_ERROR;
        res.line.error = "Invalid color definition";
        break;
      }
      switch (line[1]) {
        case 'a': res.type = MTL_AMBIENT; break;
        case 'd': res.type = MTL_DIFFUSE; break;
        case 's': res.type = MTL_SPECULAR; break;
        default:
          res.type = MTL_ERROR;
          res.line.error = "Invalid line";
          break;
      }
      if (len + 3 < line_len) {
        res.type = MTL_ERROR;
        res.line.error = "Junk data after color definition";
      }
      break;
    }
    case 'N': {
      res.type = MTL_SPECULAR_CO;
      if (line[1] != 's') {
        res.type = MTL_ERROR;
        res.line.error = "Invalid line";
        break;
      }
      int matched, len;
      matched = sscanf(line+3, "%f%n", &res.line.specular_co, &len);
      if (matched < 1) {
        res.type = MTL_ERROR;
        res.line.error = "Invalid specular coefficient";
      }
      break;
    }
    case 'T':
      if (line[1] != 'r') {
        res.type = MTL_ERROR;
        res.line.error = "Invalid line";
        break;
      }
    case 'd': {
      res.type = MTL_TRANSPARENCY;
      int matched, len;
      matched = sscanf(line+(line[0]=='d'? 2 : 3), "%f%n", &res.line.transparency, &len);
      if (matched < 1) {
        res.type = MTL_ERROR;
        res.line.error = "Invalid transparency value";
      }
      break;
    }
    case 'i':
      res.type = MTL_ILLUM;
      res.line.illum = atoi(line + 6);
      break;
    default:
      res.type = MTL_ERROR;
      res.line.error = "Invalid line";
  }
  
  return res;
}

static struct mtl_file *mtl_readstring(const char * data, const char *filename)
{
  int             cur_line = 0, 
                  data_len, 
                  line_len;
  const char      *line_ptr = data;
  char            *line_str;
  struct mtl_file *first, 
                  *cur;
  
  first = cur = calloc(1, sizeof(struct mtl_file));
  data_len = strlen(data);
  
  while (line_ptr < data+data_len) {
    line_len = 0;
    while (line_ptr[line_len] != '\n' && line_ptr[line_len] != '\0') 
      line_len++;
    line_str = malloc(line_len + 1);
    memcpy(line_str, line_ptr, line_len);
    line_str[line_len] = 0;
    line_ptr += line_len + 1;
    
    struct mtl_line line = mtl_readline(line_str);
    cur_line++;
  
    switch (line.type) {
      case MTL_ERROR:
        fprintf(stderr, "[%s:%i] %s: %s\n", filename, cur_line, line.line.error, line_str);
        break;
      case MTL_NEWMTL:
        cur->next = calloc(1, sizeof(struct mtl_file));
        cur = cur->next;
        cur->mtl.name = line.line.newmtl;
        break;
      case MTL_AMBIENT:
        memcpy(cur->mtl.ambient, line.line.color, sizeof(obj_color));
        break;
      case MTL_DIFFUSE:
        memcpy(cur->mtl.diffuse, line.line.color, sizeof(obj_color));
        break;
      case MTL_SPECULAR:
        memcpy(cur->mtl.specular, line.line.color, sizeof(obj_color));
        break;
      case MTL_SPECULAR_CO:
        cur->mtl.specular_co = line.line.specular_co;
        break;
      case MTL_TRANSPARENCY:
        cur->mtl.transparency = line.line.transparency;
        break;
      case MTL_ILLUM:
        cur->mtl.illumination = line.line.illum;
        break;
      default:
        break;
    }
  }
  
  return first;
}

obj_file obj_readstring(const char * data, obj_reader inc, const char *filename)
{
  // declare a boatload of variables
  obj_file        file;
  struct obj_ctx  ctx;
  int             data_len, 
                  line_len;
  const char      *line_ptr = data, 
                  *cur_gname = NULL;
  char            *line_str;
  struct obj_line *last = NULL;
  struct mtl_file *cur_mtl_file = NULL;
  obj_mesh        *cur_mesh;
  
  // setup the variables
  ctx = (struct obj_ctx){NULL,0};
  data_len = strlen(data);
  memset(&file, 0, sizeof(obj_file));
  file.name = filename;
  cur_mesh = create_mesh();
  file.first_mesh = cur_mesh;
  
  while (line_ptr < data+data_len) {
    line_len = 0;
    while (line_ptr[line_len] != '\n' && line_ptr[line_len] != '\0') 
      line_len++;
    line_str = malloc(line_len + 1);
    memcpy(line_str, line_ptr, line_len);
    line_str[line_len] = 0;
    line_ptr += line_len + 1;
    
    struct obj_line *line = obj_readline(&ctx, line_str);
    if (last) last->next = line;
    else ctx.first = line;
    last = line;
    ctx.line++;
    
    switch(line->type) {
      case OBJ_ERROR:
        fprintf(stderr, "[%s:%i] %s: %s\n", filename, ctx.line, line->line.error, line_str);
        break;
      case OBJ_MTLLIB: {
        // load the mtl file and parse it
        free(cur_mtl_file);
        cur_mtl_file = mtl_readstring(inc(line->line.mtllib), line->line.mtllib);
        break;
      }
      case OBJ_FACE:
        if (cur_mesh->first_face) {
          obj_face *cur = cur_mesh->first_face;
          while (cur->next) cur = cur->next;
          cur->next = &line->line.face;
        } else cur_mesh->first_face = &line->line.face;
        break;
      case OBJ_ONAME:
        if (cur_mesh->name) {
          cur_mesh->next = create_mesh();
          cur_mesh = cur_mesh->next;
        }
        cur_mesh->name = line->line.oname;
        cur_mesh->group = cur_gname;
        break;
      case OBJ_GNAME:
        cur_gname = line->line.gname;
        break;
      case OBJ_SMOOTHING:
        cur_mesh->smoothing = atoi(line->line.smoothing);
        break;
      case OBJ_USEMTL: {
        struct mtl_file * cur = cur_mtl_file;
        do {
          cur = cur->next;
        } while (cur && cur->mtl.name && 
          strcmp(cur->mtl.name, line->line.usemtl) != 0);
        if (cur)
          memcpy(&cur_mesh->mtl, &cur->mtl, sizeof(obj_mtl));
        break;
      }
      default:
        break;
    }
  }
  return file;
}

const char* obj_stdio_reader(const char *filename)
{
  struct stat s;
  stat(filename, &s);
  char *buf = malloc(s.st_size + 1);
  FILE *f = fopen(filename, "r");
  if (!f)
    perror(filename);
  int len = fread(buf, 1, s.st_size, f);
  if (!len)
    perror(filename);
  buf[len] = 0;
  return buf;
}

obj_file obj_readfile(const char *filename)
{
  return obj_readstring(obj_stdio_reader(filename), obj_stdio_reader, filename);
}

GLfloat *obj_to_vbo(obj_mesh *mesh, enum obj_vertextype vertex, 
  enum obj_texcoordtype texcoord, enum obj_normaltype normal, 
  enum obj_facetype face, enum obj_vbolayout layout, size_t *size)
{
  int       num_points = 0;
  obj_face  *cur = mesh->first_face;
  size_t    vertex_size, 
            texcoord_size, 
            normal_size, 
            interleaved_size;
  GLfloat   *buf, 
            *ptr;
  int       per_point = face? 4:3;
  
  vertex_size = sizeof(GLfloat) * (
    ((vertex&OBJ_X) == OBJ_X) + 
    ((vertex&OBJ_Y) == OBJ_Y) + 
    ((vertex&OBJ_Z) == OBJ_Z) +
    ((vertex&OBJ_W) == OBJ_W));
  texcoord_size = sizeof(GLfloat) * (
    ((texcoord&OBJ_U) == OBJ_U) +
    ((texcoord&OBJ_V) == OBJ_V) +
    ((texcoord&OBJ_W) == OBJ_W));
  normal_size = sizeof(obj_normal) * normal;
  interleaved_size = vertex_size + texcoord_size + normal_size;
  
  while (cur) {
    num_points += per_point;
    cur = cur->next;
  }
  
  buf = malloc(num_points * interleaved_size);
  ptr = buf;
  
  if (layout) {
    cur = mesh->first_face;
    while (cur) {
      memcpy(ptr, cur->vertices, vertex_size * per_point);
      ptr += 4 * per_point;
      cur = cur->next;
    }
    cur = mesh->first_face;
    while (cur) {
      memcpy(ptr, cur->texcoords, texcoord_size * per_point);
      ptr += 3 * per_point;
      cur = cur->next;
    }
    cur = mesh->first_face;
    while (cur) {
      memcpy(ptr, cur->normals, normal_size * per_point);
      ptr += 3 * per_point;
      cur = cur->next;
    }
  } else {
    cur = mesh->first_face;
    while (cur) {
      int i;
      for (i = 0; i < per_point; i++) {
        memcpy(ptr, cur->vertices + i, vertex_size);
        ptr += 4;
        memcpy(ptr, cur->texcoords + i, texcoord_size);
        ptr += 3;
        memcpy(ptr, cur->normals + i, normal_size);
        ptr += 3;
      }
      cur = cur->next;
    }
  }
  *size = num_points * interleaved_size;
  return buf;
}

GLuint obj_to_gl(obj_mesh *mesh, GLint *count)
{
  GLuint buf;
  GLfloat *data;
  size_t size, stride;
  
  stride = sizeof(obj_vertex) + sizeof(obj_texcoord) + sizeof(obj_normal);
  glGenBuffers(1, &buf);
  glBindBuffer(GL_ARRAY_BUFFER, buf);
  data = obj_to_vbo(mesh, OBJ_XYZW, OBJ_UVW, OBJ_NORMALS, 
    OBJ_TRIANGLES, OBJ_INTERLEAVED, &size);
  *count = size/stride;
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  free(data);
  
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, NULL);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, 
    (void*)sizeof(obj_vertex));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
    (void*)(sizeof(obj_vertex)+sizeof(obj_texcoord)));
  
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  
  return buf;
}
