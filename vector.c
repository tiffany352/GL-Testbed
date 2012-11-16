#include "vector.h"

#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Vector2

#define simple_op(name, op) \
sg_Vector2 sg_Vector2_##name(sg_Vector2 a, sg_Vector2 b) { \
  return (sg_Vector2) {a.x op b.x, a.y op b.y}; \
}\
sg_Vector2 sg_Vector2_##name##_f(sg_Vector2 a, float b) { \
  return (sg_Vector2) {a.x op b, a.y op b}; \
}

simple_op(add, +)
simple_op(sub, -)
simple_op(mul, *)
simple_op(div, /)

#undef simple_op

float sg_Vector2_len(sg_Vector2 v) {
  return sqrt(v.x*v.x + v.y*v.y);
}

sg_Vector2 sg_Vector2_normalise(sg_Vector2 v) {
  float l = sg_Vector2_len(v);
  return (sg_Vector2) {v.x/l, v.y/l};
}

float sg_Vector2_dot(sg_Vector2 a, sg_Vector2 b) {
  return (a.x*b.x) + (a.y*b.y);
}

////////////////////////////////////////////////////////////////////////////////
// Vector3

#define simple_op(name, op) \
sg_Vector3 sg_Vector3_##name(sg_Vector3 a, sg_Vector3 b) { \
  return (sg_Vector3) {a.x op b.x, a.y op b.y, a.z op b.z}; \
}\
sg_Vector3 sg_Vector3_##name##_f(sg_Vector3 a, float b) { \
  return (sg_Vector3) {a.x op b, a.y op b, a.z op b}; \
}

simple_op(add, +)
simple_op(sub, -)
simple_op(mul, *)
simple_op(div, /)

#undef simple_op

float sg_Vector3_len(sg_Vector3 v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

sg_Vector3 sg_Vector3_normalise(sg_Vector3 v) {
  float l = sg_Vector3_len(v);
  return (sg_Vector3) {v.x/l, v.y/l, v.z/l};
}

float sg_Vector3_dot(sg_Vector3 a, sg_Vector3 b) {
  return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}

// u = (a,b,c) and v = (p,r,q)
// u x v = (br-cq,cp-ar,aq-bp)
sg_Vector3 sg_Vector3_cross(sg_Vector3 a, sg_Vector3 b) {
  return (sg_Vector3) {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

/*
stolen from quaternion.inl in glm source
  template <typename T>
  GLM_FUNC_QUALIFIER detail::tvec3<T> operator* 
  (detail::tquat<T> const & q, detail::tvec3<T> const & v) {
    typename detail::tquat<T>::value_type Two(2);

    detail::tvec3<T> uv, uuv;
    detail::tvec3<T> QuatVector(q.x, q.y, q.z);
    uv = glm::cross(QuatVector, v);
    uuv = glm::cross(QuatVector, uv);
    uv *= (Two * q.w); 
    uuv *= Two; 

    return v + uv + uuv;
  }
  
  // in non-alien:
  vec3 rotate(quat q, vec3 v) {
    uv = cross(q.xyz, v)
    uuv = cross(q.xyz, uv)
    uv = uv * (2 * q.w)
    uuv = uuv * 2
    return v + uv + uuv;
  }
*/
sg_Vector3 sg_Vector3_rotate_q(sg_Vector3 v, sg_Quaternion q) {

  sg_Vector3 uv, uuv, q_vec;
  q_vec = (sg_Vector3){q.x, q.y, q.z};
  uv = sg_Vector3_cross(q_vec, v);
  uuv = sg_Vector3_cross(q_vec, uv);
  uv = sg_Vector3_mul_f(uv, 2 * q.w);
  uuv = sg_Vector3_mul_f(uuv, 2);
  
  return sg_Vector3_add(
    v,
    sg_Vector3_add(
    uv,
    uuv
  ));
}

////////////////////////////////////////////////////////////////////////////////
// Vector4

#define simple_op(name, op) \
sg_Vector4 sg_Vector4_##name(sg_Vector4 a, sg_Vector4 b) { \
  return (sg_Vector4) {a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w}; \
}\
sg_Vector4 sg_Vector4_##name##_f(sg_Vector4 a, float b) { \
  return (sg_Vector4) {a.x op b, a.y op b, a.z op b, a.w op b}; \
}

simple_op(add, +)
simple_op(sub, -)
simple_op(mul, *)
simple_op(div, /)

#undef simple_op

float sg_Vector4_len(sg_Vector4 v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

sg_Vector4 sg_Vector4_normalise(sg_Vector4 v) {
  float l = sg_Vector4_len(v);
  return (sg_Vector4) {v.x/l, v.y/l, v.z/l, v.w/l};
}

float sg_Vector4_dot(sg_Vector4 a, sg_Vector4 b) {
  return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}

// u = (a,b,c) and v = (p,r,q)
// u x v = (br-cq,cp-ar,aq-bp)
sg_Vector4 sg_Vector4_cross(sg_Vector4 a, sg_Vector4 b) {
  return (sg_Vector4) {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x, 1};
}

sg_Vector4 sg_Vector4_rotate_q(sg_Vector4 v, sg_Quaternion q) {

  sg_Vector4 uv, uuv, q_vec;
  q_vec = (sg_Vector4){q.x, q.y, q.z, 1};
  uv = sg_Vector4_cross(q_vec, v);
  uuv = sg_Vector4_cross(q_vec, uv);
  uv = sg_Vector4_mul_f(uv, 2 * q.w);
  uuv = sg_Vector4_mul_f(uuv, 2);
  
  return sg_Vector4_add(
    v,
    sg_Vector4_add(
    uv,
    uuv
  ));
}

