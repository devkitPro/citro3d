#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

extern "C" {
#include <c3d/maths.h>
}

typedef std::default_random_engine            generator_t;
typedef std::uniform_real_distribution<float> distribution_t;

static inline void
randomMatrix(C3D_Mtx &m, generator_t &g, distribution_t &d)
{
  for(size_t i = 0; i < 16; ++i)
    m.m[i] = d(g);
}

static inline glm::vec3
randomVector3(generator_t &g, distribution_t &d)
{
  return glm::vec3(d(g), d(g), d(g));
}

static inline glm::vec4
randomVector4(generator_t &g, distribution_t &d)
{
  return glm::vec4(d(g), d(g), d(g), d(g));
}

static inline float
randomAngle(generator_t &g, distribution_t &d)
{
  return d(g);
}

static inline C3D_FQuat
randomQuat(generator_t &g, distribution_t &d)
{
  return Quat_New(d(g), d(g), d(g), d(g));
}

static inline glm::mat4
loadMatrix(const C3D_Mtx &m)
{
  return glm::mat4(m.m[ 3], m.m[ 7], m.m[11], m.m[15],
                   m.m[ 2], m.m[ 6], m.m[10], m.m[14],
                   m.m[ 1], m.m[ 5], m.m[ 9], m.m[13],
                   m.m[ 0], m.m[ 4], m.m[ 8], m.m[12]);
}

static inline glm::quat
loadQuat(const C3D_FQuat &q)
{
  return glm::quat(q.r, q.i, q.j, q.k);
}

static inline bool
operator==(const glm::vec3 &lhs, const C3D_FVec &rhs)
{
  return std::abs(lhs.x - rhs.x) < 0.001f
      && std::abs(lhs.y - rhs.y) < 0.001f
      && std::abs(lhs.z - rhs.z) < 0.001f;
}

static inline bool
operator==(const C3D_FVec &lhs, const glm::vec3 &rhs)
{
  return rhs == lhs;
}

static inline bool
operator==(const glm::vec4 &lhs, const C3D_FVec &rhs)
{
  return std::abs(lhs.x - rhs.x) < 0.001f
      && std::abs(lhs.y - rhs.y) < 0.001f
      && std::abs(lhs.z - rhs.z) < 0.001f
      && std::abs(lhs.w - rhs.w) < 0.001f;
}

static inline bool
operator==(const C3D_FVec &lhs, const glm::vec4 &rhs)
{
  return rhs == lhs;
}

static inline bool
operator==(const glm::mat4 &lhs, const C3D_Mtx &rhs)
{
  for(size_t i = 0; i < 4; ++i)
  {
    for(size_t j = 0; j < 4; ++j)
    {
      if(std::abs(lhs[i][j] - rhs.m[j*4+3-i]) > 0.001f)
        return false; // LCOV_EXCL_LINE This would cause an assertion failure
    }
  }

  return true;
}

static inline bool
operator==(const C3D_Mtx &lhs, const glm::mat4 &rhs)
{
  return rhs == lhs;
}

static inline bool
operator==(const glm::quat &lhs, const C3D_FQuat &rhs)
{
  return std::abs(lhs.w - rhs.r) < 0.01f
      && std::abs(lhs.x - rhs.i) < 0.01f
      && std::abs(lhs.y - rhs.j) < 0.01f
      && std::abs(lhs.z - rhs.k) < 0.01f;
}

static inline bool
operator==(const C3D_FQuat &lhs, const glm::quat &rhs)
{
  return rhs == lhs;
}

static inline bool
operator==(const C3D_FQuat &lhs, const C3D_FQuat &rhs)
{
  return std::abs(lhs.r - rhs.r) < 0.01f
      && std::abs(lhs.i - rhs.i) < 0.01f
      && std::abs(lhs.j - rhs.j) < 0.01f
      && std::abs(lhs.k - rhs.k) < 0.01f;
}

static inline void
print(const C3D_FVec &v)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  std::printf("% 6.4f % 6.4f % 6.4f % 6.4f\n", v.w, v.x, v.y, v.z);
}

static inline void
print(const glm::vec3 &v)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  std::printf("% 6.4f % 6.4f % 6.4f\n", v.x, v.y, v.z);
}

static inline void
print(const glm::vec4 &v)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  std::printf("%6.4f % 6.4f % 6.4f % 6.4f\n", v.w, v.x, v.y, v.z);
}

static inline void
print(const C3D_Mtx &m)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  for(size_t j = 0; j < 4; ++j)
  {
    std::printf("% 6.4f % 6.4f % 6.4f % 6.4f\n",
                m.m[j*4+3],
                m.m[j*4+2],
                m.m[j*4+1],
                m.m[j*4+0]);
  }
}

static inline void
print(const glm::mat4 &m)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  for(size_t j = 0; j < 4; ++j)
  {
    std::printf("% 6.4f % 6.4f % 6.4f % 6.4f\n",
                m[0][j],
                m[1][j],
                m[2][j],
                m[3][j]);
  }
}

static inline void
print(const glm::quat &q)
{
  std::printf("%s:\n", __PRETTY_FUNCTION__);
  std::printf("% 6.4f % 6.4f % 6.4f % 6.4f\n", q.w, q.x, q.y, q.z);
}

static const glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
static const glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
static const glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
static const glm::vec3 z_flip(1.0f, 1.0f, -1.0f);

static void
check_matrix(generator_t &gen, distribution_t &dist)
{

  glm::mat4 fix_depth(1.0f, 0.0f,  0.0f, 0.0f,
                      0.0f, 1.0f,  0.0f, 0.0f,
                      0.0f, 0.0f,  0.5f, 0.0f,
                      0.0f, 0.0f, -0.5f, 1.0f);
  glm::mat4 tilt = glm::rotate(glm::mat4(), -static_cast<float>(M_TAU)/4.0f, z_axis);

  // check identity
  {
    C3D_Mtx m;
    Mtx_Identity(&m);
    assert(m == glm::mat4());
  }

  // ortho nominal cases
  {
    C3D_Mtx  m;
    C3D_FVec v;
    float    l = 0.0f,
             r = 400.0f,
             b = 0.0f,
             t = 320.0f,
             n = 0.0f,
             f = 100.0f;

    Mtx_Ortho(&m, l, r, b, t, n, f, false);

    // check near clip plane
    v = Mtx_MultiplyFVecH(&m, FVec3_New((r-l)/2.0f, (t-b)/2.0f, -n));
    v = FVec4_PerspDivide(v);
    assert(v == FVec4_New(0.0f, 0.0f, -1.0f, 1.0f));

    // check far clip plane
    v = Mtx_MultiplyFVecH(&m, FVec3_New((r-l)/2.0f, (t-b)/2.0f, -f));
    v = FVec4_PerspDivide(v);
    assert(v == FVec4_New(0.0f, 0.0f, 0.0f, 1.0f));
  }

  // perspective nominal cases
  {
    C3D_Mtx  m;
    C3D_FVec v;
    float   fovy   = C3D_Angle(60.0f/360.0f),
            aspect = C3D_AspectRatioTop,
            near   = 0.1f,
            far    = 10.0f;

    Mtx_Persp(&m, fovy, aspect, near, far, false);

    // check near clip plane
    v = Mtx_MultiplyFVecH(&m, FVec3_New(0.0f, 0.0f, -near));
    v = FVec4_PerspDivide(v);
    assert(v == FVec4_New(0.0f, 0.0f, -1.0f, 1.0f));

    // check far clip plane
    v = Mtx_MultiplyFVecH(&m, FVec3_New(0.0f, 0.0f, -far));
    v = FVec4_PerspDivide(v);
    assert(v == FVec4_New(0.0f, 0.0f, 0.0f, 1.0f));
  }

  for(size_t x = 0; x < 10000; ++x)
  {
    // check inverse
    {
      C3D_Mtx m, inv, id;

      randomMatrix(m, gen, dist);

      // cast to int to try to avoid assertion failure due to rounding error
      for(size_t i = 0; i < 16; ++i)
        m.m[i] = static_cast<int>(m.m[i]);

      Mtx_Copy(&inv, &m);
      if(Mtx_Inverse(&inv))
      {
        Mtx_Multiply(&id, &m, &inv);
        assert(id == glm::mat4()); // could still fail due to rounding errors
        Mtx_Multiply(&id, &inv, &m);
        assert(id == glm::mat4()); // could still fail due to rounding errors
      }
    }

    // check perspective
    {
      C3D_Mtx m;
      float   fovy   = dist(gen),
              aspect = dist(gen),
              near   = dist(gen),
              far    = dist(gen),
              fovx;

      while(aspect < 0.25f || aspect > 4.0f)
        aspect = dist(gen);

      while(fovy < M_TAU / 36.0f
         || fovy >= M_TAU / 2.0f
         || (fovx = 2.0f * atanf(tanf(fovy/2.0f) * aspect)) < M_TAU / 36.0f
         || fovx >= M_TAU / 2.0f)
      {
        fovy = dist(gen);
      }

      while(std::abs(far - near) < 0.1f)
        far = dist(gen);

      // RH
      Mtx_Persp(&m, fovy, aspect, near, far, false);
      glm::mat4 g = glm::perspective(fovy, aspect, near, far);
      assert(m == fix_depth*g);

      // LH
      Mtx_Persp(&m, fovy, aspect, near, far, true);
      g = glm::perspective(fovy, aspect, near, far);
      assert(m == fix_depth*glm::scale(g, z_flip));
    }

    // check perspective tilt
    {
      C3D_Mtx m;
      float   fovy   = dist(gen),
              aspect = dist(gen),
              near   = dist(gen),
              far    = dist(gen),
              fovx;

      while(aspect < 0.25f || aspect > 4.0f)
        aspect = dist(gen);

      while(fovy < M_TAU / 36.0f
         || fovy >= M_TAU / 2.0f
         || (fovx = 2.0f * atanf(tanf(fovy/2.0f) * aspect)) < M_TAU / 36.0f
         || fovx >= M_TAU / 2.0f)
      {
        fovy = dist(gen);
      }

      while(std::abs(far - near) < 0.1f)
        far = dist(gen);

      // RH
      Mtx_PerspTilt(&m, fovy, aspect, near, far, false);
      glm::mat4 g = glm::perspective(fovx, 1.0f / aspect, near, far);
      assert(m == fix_depth*g*tilt);

      // LH
      Mtx_PerspTilt(&m, fovy, aspect, near, far, true);
      g = glm::perspective(fovx, 1.0f / aspect, near, far);
      assert(m == fix_depth*glm::scale(g, z_flip)*tilt);
    }

    // check perspective stereo
    {
      C3D_Mtx left, right;
      float   fovy   = dist(gen),
              aspect = dist(gen),
              near   = dist(gen),
              far    = dist(gen),
              iod    = dist(gen),
              focLen = dist(gen),
              fovy_tan,
              fovx;

      while(aspect < 0.25f || aspect > 4.0f)
        aspect = dist(gen);

      while(fovy < M_TAU / 36.0f
         || fovy >= M_TAU / 2.0f
         || (fovx = 2.0f * atanf(tanf(fovy/2.0f) * aspect)) < M_TAU / 36.0f
         || fovx >= M_TAU / 2.0f)
      {
        fovy = dist(gen);
      }

      while(std::abs(far - near) < 0.1f)
        far = dist(gen);

      while(focLen < 0.25f)
        focLen = dist(gen);

      glm::mat4 g = glm::perspective(fovy, aspect, near, far);

      fovy_tan = tanf(fovy/2.0f);
      glm::mat4 left_eye (1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          iod/(focLen*2.0f), 0.0f, 1.0f, 0.0f,
                          iod*fovy_tan*aspect/2.0f, 0.0f, 0.0f, 1.0f);
      glm::mat4 right_eye(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          -iod/(focLen*2.0f), 0.0f, 1.0f, 0.0f,
                          -iod*fovy_tan*aspect/2.0f, 0.0f, 0.0f, 1.0f);

      // RH
      Mtx_PerspStereo(&left, fovy, aspect, near, far, -iod, focLen, false);
      Mtx_PerspStereo(&right, fovy, aspect, near, far, iod, focLen, false);
      assert(left == fix_depth*g*left_eye);
      assert(right == fix_depth*g*right_eye);

      // LH
      Mtx_PerspStereo(&left, fovy, aspect, near, far, -iod, focLen, true);
      Mtx_PerspStereo(&right, fovy, aspect, near, far, iod, focLen, true);
      assert(left == fix_depth*glm::scale(g*left_eye, z_flip));
      assert(right == fix_depth*glm::scale(g*right_eye, z_flip));
    }

    // check perspective stereo tilt
    {
      C3D_Mtx left, right;
      float   fovy   = dist(gen),
              aspect = dist(gen),
              near   = dist(gen),
              far    = dist(gen),
              iod    = dist(gen),
              focLen = dist(gen),
              fovx,
              fovx_tan;

      while(aspect < 0.25f || aspect > 4.0f)
        aspect = dist(gen);

      while(fovy < M_TAU / 36.0f
         || fovy >= M_TAU / 2.0f
         || (fovx = 2.0f * atanf(tanf(fovy/2.0f) * aspect)) < M_TAU / 36.0f
         || fovx >= M_TAU / 2.0f)
      {
        fovy = dist(gen);
      }

      while(std::abs(far - near) < 0.1f)
        far = dist(gen);

      while(focLen < 0.25f)
        focLen = dist(gen);

      glm::mat4 g = glm::perspective(fovx, 1.0f / aspect, near, far);

      fovx_tan = tanf(fovx/2.0f);
      glm::mat4 left_eye (1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, -iod/(focLen*2.0f), 1.0f, 0.0f,
                          0.0f, -iod*fovx_tan/2.0f, 0.0f, 1.0f);
      glm::mat4 right_eye(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, iod/(focLen*2.0f), 1.0f, 0.0f,
                          0.0f, iod*fovx_tan/2.0f, 0.0f, 1.0f);

      // RH
      Mtx_PerspStereoTilt(&left, fovy, aspect, near, far, -iod, focLen, false);
      Mtx_PerspStereoTilt(&right, fovy, aspect, near, far, iod, focLen, false);
      assert(left == fix_depth*g*left_eye*tilt);
      assert(right == fix_depth*g*right_eye*tilt);

      // LH
      Mtx_PerspStereoTilt(&left, fovy, aspect, near, far, -iod, focLen, true);
      Mtx_PerspStereoTilt(&right, fovy, aspect, near, far, iod, focLen, true);
      assert(left == fix_depth*glm::scale(g*left_eye, z_flip)*tilt);
      assert(right == fix_depth*glm::scale(g*right_eye, z_flip)*tilt);
    }

    // check ortho
    {
      C3D_Mtx m;
      float   l = dist(gen),
              r = dist(gen),
              b = dist(gen),
              t = dist(gen),
              n = dist(gen),
              f = dist(gen);

      while(std::abs(r-l) < 0.1f)
        r = dist(gen);

      while(std::abs(t-b) < 0.1f)
        t = dist(gen);

      while(std::abs(f-n) < 0.1f)
        f = dist(gen);

      // RH
      Mtx_Ortho(&m, l, r, b, t, n, f, false);
      glm::mat4 g = glm::ortho(l, r, b, t, n, f);
      assert(m == fix_depth*g);

      // LH
      Mtx_Ortho(&m, l, r, b, t, n, f, true);
      g = glm::ortho(l, r, b, t, n, f);
      assert(m == fix_depth*glm::scale(g, z_flip));
    }

    // check ortho tilt
    {
      C3D_Mtx m;
      float   l = dist(gen),
              r = dist(gen),
              b = dist(gen),
              t = dist(gen),
              n = dist(gen),
              f = dist(gen);

      while(std::abs(r-l) < 0.1f)
        r = dist(gen);

      while(std::abs(t-b) < 0.1f)
        t = dist(gen);

      while(std::abs(f-n) < 0.1f)
        f = dist(gen);

      // RH
      Mtx_OrthoTilt(&m, l, r, b, t, n, f, false);
      glm::mat4 g = glm::ortho(l, r, b, t, n, f);
      assert(m == tilt*fix_depth*g);

      // LH
      Mtx_OrthoTilt(&m, l, r, b, t, n, f, true);
      g = glm::ortho(l, r, b, t, n, f);
      assert(m == tilt*fix_depth*glm::scale(g, z_flip));
    }

    // check lookAt
    {
      C3D_Mtx m;
      C3D_FVec camera, target, diff, up;

      // avoid very small distances and 'up' pointing near the target
      do
      {
        camera = FVec3_New(dist(gen), dist(gen), dist(gen));
        target = FVec3_New(dist(gen), dist(gen), dist(gen));
        up     = FVec3_New(dist(gen), dist(gen), dist(gen));
        diff   = FVec3_Subtract(target, camera);
      } while(FVec3_Magnitude(diff) < 0.25f
           || FVec3_Magnitude(up) < 0.25f
           || FVec3_Dot(up, diff) / FVec3_Magnitude(up) / FVec3_Magnitude(diff) < cosf(30.0f*M_TAU/360.0f));

      glm::mat4 g = glm::lookAt(glm::vec3(camera.x, camera.y, camera.z),
                                glm::vec3(target.x, target.y, target.z),
                                glm::vec3(up.x,     up.y,     up.z));

      // RH
      Mtx_LookAt(&m, camera, target, up, false);
      assert(m == g);

      // LH
      Mtx_LookAt(&m, camera, target, up, true);
      // I can't say for certain that this is the correct test
      assert(m == glm::scale(glm::mat4(), glm::vec3(-1.0f, 1.0f, -1.0f))*g);
    }

    // check multiply
    {
      C3D_Mtx m1, m2;
      randomMatrix(m1, gen, dist);
      randomMatrix(m2, gen, dist);

      glm::mat4 g1 = loadMatrix(m1);
      glm::mat4 g2 = loadMatrix(m2);

      C3D_Mtx result;
      Mtx_Multiply(&result, &m1, &m2);
      assert(result == g1*g2);
    }

    // check translate
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      Mtx_Translate(&m, v.x, v.y, v.z, true);
      assert(m == glm::translate(g, v));
    }

    // check translate (reversed)
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      Mtx_Translate(&m, v.x, v.y, v.z, false);
      assert(m == glm::translate(glm::mat4(), v)*g);
    }

    // check scale
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      Mtx_Scale(&m, v.x, v.y, v.z);
      assert(m == glm::scale(g, v));
    }

    // check rotate
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      Mtx_Rotate(&m, FVec3_New(v.x, v.y, v.z), r, true);
      assert(m == glm::rotate(g, r, v));
    }

    // check rotate (reversed)
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      Mtx_Rotate(&m, FVec3_New(v.x, v.y, v.z), r, false);
      assert(m == glm::rotate(glm::mat4(), r, v)*g);
    }

    // check rotate X
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateX(&m, r, true);
      assert(m == glm::rotate(g, r, x_axis));
    }

    // check rotate X (reversed)
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateX(&m, r, false);
      assert(m == glm::rotate(glm::mat4(), r, x_axis)*g);
    }

    // check rotate Y
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateY(&m, r, true);
      assert(m == glm::rotate(g, r, y_axis));
    }

    // check rotate Y (reversed)
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateY(&m, r, false);
      assert(m == glm::rotate(glm::mat4(), r, y_axis)*g);
    }

    // check rotate Z
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateZ(&m, r, true);
      assert(m == glm::rotate(g, r, z_axis));
    }

    // check rotate Z (reversed)
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      float r = randomAngle(gen, dist);

      glm::mat4 g = loadMatrix(m);

      Mtx_RotateZ(&m, r, false);
      assert(m == glm::rotate(glm::mat4(), r, z_axis)*g);
    }

    // check vec3 multiply
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec3 v = randomVector3(gen, dist);

      assert(Mtx_MultiplyFVec3(&m, FVec3_New(v.x, v.y, v.z)) == glm::mat3x3(g)*v);
    }

    // check vec4 multiply
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec4 v = randomVector4(gen, dist);

      assert(Mtx_MultiplyFVec4(&m, FVec4_New(v.x, v.y, v.z, v.w)) == g*v);
    }

    // check vecH multiply
    {
      C3D_Mtx m;
      randomMatrix(m, gen, dist);

      glm::mat4 g = loadMatrix(m);
      glm::vec4 v = randomVector4(gen, dist);
      v.w = 1.0f;

      assert(Mtx_MultiplyFVecH(&m, FVec3_New(v.x, v.y, v.z)) == glm::mat4x3(g)*v);
    }

    // check matrix transpose
    {
      C3D_Mtx m;
      glm::mat4 check;

      randomMatrix(m, gen, dist);

      //Reducing rounding errors, and copying the values over to the check matrix.
      for(size_t i = 0; i < 16; ++i)
      {
        m.m[i] = static_cast<int>(m.m[i]);
      }

      check = loadMatrix(m);

      Mtx_Transpose(&m);
      assert(m == glm::transpose(check));
      Mtx_Transpose(&m);
      assert(m == check);

      //Comparing inverse(transpose(m)) == transpose(inverse(m))
      C3D_Mtx m2;
      Mtx_Copy(&m2, &m);
      Mtx_Transpose(&m2);
      if(Mtx_Inverse(&m2))
      {
        assert(m2 == glm::inverse(glm::transpose(check)));
        assert(m2 == glm::transpose(glm::inverse(check)));
      }
      Mtx_Copy(&m2, &m);
      if(Mtx_Inverse(&m2))
      {
        Mtx_Transpose(&m2);
        assert(m2 == glm::inverse(glm::transpose(check)));
        assert(m2 == glm::transpose(glm::inverse(check)));
      }
    }
  }
}

static void
check_quaternion(generator_t &gen, distribution_t &dist)
{
  // check identity
  {
    C3D_FQuat q = Quat_Identity();
    glm::quat g;

    assert(q == g);
  }

  for(size_t x = 0; x < 10000; ++x)
  {
    // check negation
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      assert(Quat_Negate(q) == -g);
    }

    // check addition
    {
      C3D_FQuat q1 = randomQuat(gen, dist);
      C3D_FQuat q2 = randomQuat(gen, dist);

      glm::quat g1 = loadQuat(q1);
      glm::quat g2 = loadQuat(q2);

      assert(Quat_Add(q1, q2) == g1+g2);
    }

    // check subtraction
    {
      C3D_FQuat q1 = randomQuat(gen, dist);
      C3D_FQuat q2 = randomQuat(gen, dist);

      glm::quat g1 = loadQuat(q1);
      glm::quat g2 = loadQuat(q2);

      assert(Quat_Subtract(q1, q2) == g1 + (-g2));
    }

    // check scale
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      float f = dist(gen);

      assert(Quat_Scale(q, f) == g*f);
    }

    // check normalize
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      assert(Quat_Normalize(q) == glm::normalize(g));
    }

    // check dot
    {
      C3D_FQuat q1 = randomQuat(gen, dist);
      C3D_FQuat q2 = randomQuat(gen, dist);
      glm::quat g1 = loadQuat(q1);
      glm::quat g2 = loadQuat(q2);

      assert(std::abs(Quat_Dot(q1, q2) - glm::dot(g1, g2)) < 0.0001f);
    }

    // check conjugate
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      assert(Quat_Conjugate(q) == glm::conjugate(g));
    }

    // check inverse
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      assert(Quat_Inverse(q) == glm::inverse(g));
    }

    // check quaternion multiplication
    {
      C3D_FQuat q1 = randomQuat(gen, dist);
      C3D_FQuat q2 = randomQuat(gen, dist);
      glm::quat g1 = loadQuat(q1);
      glm::quat g2 = loadQuat(q2);

      assert(Quat_Multiply(q1, q2) == g1*g2);
    }

    // check quat pow()
    // Note: older versions of glm have broken pow() for quats
    {
      C3D_FQuat q = randomQuat(gen, dist);
      //glm::quat g = loadQuat(q);
      float     r = dist(gen);

      //assert(Quat_Pow(q, r) == glm::pow(g, r));

      q = Quat_Normalize(q);

      // check trivial cases
      assert(Quat_Pow(q, 1.0f) == q);
      assert(Quat_Pow(q, 0.0f) == Quat_Identity());
      assert(Quat_Pow(Quat_Identity(), r) == Quat_Identity());

      // validate semantics
      assert(Quat_Pow(q, r) == Quat_Multiply(Quat_Pow(q, r/2), Quat_Pow(q, r/2)));
    }

    // check vector multiplication (cross)
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      glm::vec3 v = randomVector3(gen, dist);

      assert(Quat_CrossFVec3(q, FVec3_New(v.x, v.y, v.z)) == glm::cross(g, v));
      assert(FVec3_CrossQuat(FVec3_New(v.x, v.y, v.z), q) == glm::cross(v, g));
    }

    // check rotation
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      glm::vec3 v = randomVector3(gen, dist);
      float     r = randomAngle(gen, dist);

      assert(Quat_Rotate(q, FVec3_New(v.x, v.y, v.z), r, false) == glm::rotate(g, r, v));
      assert(Quat_Rotate(q, FVec3_New(v.x, v.y, v.z), r, true) == glm::rotate(glm::quat(), r, v)*g);
    }

    // check rotate X
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      float     r = randomAngle(gen, dist);

      assert(Quat_RotateX(q, r, false) == glm::rotate(g, r, x_axis));
      assert(Quat_RotateX(q, r, true) == glm::rotate(glm::quat(), r, x_axis)*g);
    }

    // check rotate Y
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      float     r = randomAngle(gen, dist);

      assert(Quat_RotateY(q, r, false) == glm::rotate(g, r, y_axis));
      assert(Quat_RotateY(q, r, true) == glm::rotate(glm::quat(), r, y_axis)*g);
    }

    // check rotate Z
    {
      C3D_FQuat q = randomQuat(gen, dist);
      glm::quat g = loadQuat(q);

      float     r = randomAngle(gen, dist);

      assert(Quat_RotateZ(q, r, false) == glm::rotate(g, r, z_axis));
      assert(Quat_RotateZ(q, r, true) == glm::rotate(glm::quat(), r, z_axis)*g);
    }

    // check conversion to matrix
    {
      C3D_FQuat q = Quat_Normalize(randomQuat(gen, dist));
      glm::quat g = loadQuat(q);

      C3D_Mtx m;
      Mtx_FromQuat(&m, q);
      assert(m == glm::mat4_cast(g));

      C3D_FQuat q2 = Quat_FromMtx(&m);
      assert(q2 == q || q2 == FVec4_Negate(q));
    }
  }
}

int main(int argc, char *argv[])
{
  std::random_device rd;
  generator_t        gen(rd());
  distribution_t     dist(-10.0f, 10.0f);

  check_matrix(gen, dist);
  check_quaternion(gen, dist);

  return EXIT_SUCCESS;
}
