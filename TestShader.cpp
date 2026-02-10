#include "TestShader.h"
#include <Arduino.h>
#include <math.h>
#include <stdint.h>
#include <string.h> // memcpy

namespace AD
{
  // ------------------------- Config -------------------------
  static constexpr int W = 64;
  static constexpr int H = 64;

  // ------------------------- Fast SIN/COS LUT -------------------------
  // 1024 samples over [0, 2π)
  static constexpr int SIN_LUT_SIZE = 1024; // power of 2
  static float SIN_LUT[SIN_LUT_SIZE];
  static bool  SIN_LUT_READY = false;

  static void InitSinLUT()
  {
    if (SIN_LUT_READY) return;
    for (int i = 0; i < SIN_LUT_SIZE; ++i)
    {
      float a = (2.0f * PI * (float)i) / (float)SIN_LUT_SIZE;
      SIN_LUT[i] = sinf(a);
    }
    SIN_LUT_READY = true;
  }

  static inline float sin_lut(float x)
  {
    // x rad -> idx in [0..SIZE)
    // inv(2π) ≈ 0.15915494
    int idx = (int)(x * (SIN_LUT_SIZE * 0.15915494f));
    return SIN_LUT[idx & (SIN_LUT_SIZE - 1)];
  }

  static inline float cos_lut(float x)
  {
    return sin_lut(x + 0.5f * PI);
  }

  // ------------------------- Fast sqrt (approx) -------------------------
  // Quake-style rsqrt + 1 Newton step.
  // Great speed, visually fine; if you see artifacts, you can add 2nd Newton step.
  static inline float fast_rsqrt(float number)
  {
    if (number <= 0.0f) return 0.0f;

    float y = number;
    float x2 = number * 0.5f;

    uint32_t i;
    memcpy(&i, &y, sizeof(i));
    i = 0x5f3759dfu - (i >> 1);
    memcpy(&y, &i, sizeof(y));

    // 1 Newton iteration
    y = y * (1.5f - (x2 * y * y));
    return y;
  }

  static inline float fast_sqrt(float x)
  {
    if (x <= 0.0f) return 0.0f;
    return x * fast_rsqrt(x);
  }

  // ------------------------- Small math types -------------------------
  struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}
  };

  struct Vec3 {
    float r, g, b;
    Vec3() : r(0), g(0), b(0) {}
    Vec3(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
  };

  static inline Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2(a.x + b.x, a.y + b.y); }
  static inline Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2(a.x - b.x, a.y - b.y); }
  static inline Vec2 operator*(const Vec2& a, float s)       { return Vec2(a.x * s, a.y * s); }

  static inline Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.r + b.r, a.g + b.g, a.b + b.b); }
  static inline Vec3 operator*(const Vec3& a, float s)       { return Vec3(a.r * s, a.g * s, a.b * s); }

  // ------------------------- Helpers (GLSL-ish) -------------------------
  static inline float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
  static inline float mixf(float a, float b, float t)   { return a + (b - a) * t; }
  static inline Vec3  mix3(const Vec3& a, const Vec3& b, float t) { return Vec3(mixf(a.r,b.r,t), mixf(a.g,b.g,t), mixf(a.b,b.b,t)); }
  static inline float stepf(float edge, float x) { return x < edge ? 0.0f : 1.0f; }

  static inline float smoothstep(float edge0, float edge1, float x)
  {
    float t = clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
  }

  static inline float dot2(const Vec2& v) { return v.x * v.x + v.y * v.y; }
  static inline float dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
  static inline float length2(const Vec2& v) { return fast_sqrt(dot2(v)); }

  static inline float fractf(float x) { return x - floorf(x); }
  static inline float modf_glsl(float x, float y) { return x - y * floorf(x / y); }
  static inline Vec2  abs2(const Vec2& v) { return Vec2(fabsf(v.x), fabsf(v.y)); }
  static inline Vec2  max2(const Vec2& a, const Vec2& b) { return Vec2(fmaxf(a.x,b.x), fmaxf(a.y,b.y)); }
  static inline Vec2  min2(const Vec2& a, const Vec2& b) { return Vec2(fminf(a.x,b.x), fminf(a.y,b.y)); }

  // ------------------------- powf approximation (for this shader use-case) -------------------------
  // Original: powf(uv.y*uv.y, 2.1)
  // Let a = uv.y^2 (>=0). Approx a^2.1 with a^2.125 = a^2 * a^(1/8)
  // a^(1/8) = sqrt(sqrt(sqrt(a))) using fast_sqrt.
  static inline float pow_a_2p1_approx(float a)
  {
    a = fmaxf(a, 0.0f);
    float a2 = a * a;          // a^2
    float r = fast_sqrt(a);    // a^(1/2)
    r = fast_sqrt(r);          // a^(1/4)
    r = fast_sqrt(r);          // a^(1/8)
    return a2 * r;             // ~ a^(2.125)
  }

  // ------------------------- Shader functions -------------------------
  static float sun(Vec2 uv, float battery, float iTime)
  {
    float len = length2(uv);
    float val   = smoothstep(0.3f, 0.29f, len);
    float bloom = smoothstep(0.7f, 0.0f,  len);

    float cut = 3.0f * sin_lut((uv.y + iTime * 0.2f * (battery + 0.02f)) * 100.0f)
              + clampf(uv.y * 14.0f + 1.0f, -6.0f, 6.0f);
    cut = clampf(cut, 0.0f, 1.0f);

    return clampf(val * cut, 0.0f, 1.0f) + bloom * 0.6f;
  }

  static float grid(Vec2 uv, float battery, float iTime)
  {
    Vec2 size = Vec2(uv.y, uv.y * uv.y * 0.2f) * 0.01f;
    uv = uv + Vec2(0.0f, iTime * 4.0f * (battery + 0.05f));

    uv = abs2(Vec2(fractf(uv.x), fractf(uv.y)) - Vec2(0.5f, 0.5f));

    Vec2 lines;
    lines.x = smoothstep(size.x, 0.0f, uv.x);
    lines.y = smoothstep(size.y, 0.0f, uv.y);

    Vec2 size5 = size * 5.0f;
    lines.x += smoothstep(size5.x, 0.0f, uv.x) * 0.4f * battery;
    lines.y += smoothstep(size5.y, 0.0f, uv.y) * 0.4f * battery;

    return clampf(lines.x + lines.y, 0.0f, 3.0f);
  }

  static float sdTrapezoid(Vec2 p, float r1, float r2, float he)
  {
    Vec2 k1(r2, he);
    Vec2 k2(r2 - r1, 2.0f * he);
    p.x = fabsf(p.x);

    Vec2 ca(p.x - fminf(p.x, (p.y < 0.0f) ? r1 : r2), fabsf(p.y) - he);

    float t = clampf(dot(k1 - p, k2) / dot2(k2), 0.0f, 1.0f);
    Vec2 cb = p - k1 + k2 * t;

    float s = (cb.x < 0.0f && ca.y < 0.0f) ? -1.0f : 1.0f;
    return s * fast_sqrt(fminf(dot2(ca), dot2(cb)));
  }

  static float sdLine(Vec2 p, Vec2 a, Vec2 b)
  {
    Vec2 pa = p - a;
    Vec2 ba = b - a;
    float h = clampf(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    Vec2 q  = pa - ba * h;
    return length2(q);
  }

  static float sdBox(Vec2 p, Vec2 b)
  {
    Vec2 d = abs2(p) - b;
    Vec2 d0(fmaxf(d.x, 0.0f), fmaxf(d.y, 0.0f));
    return length2(d0) + fminf(fmaxf(d.x, d.y), 0.0f);
  }

  static float opSmoothUnion(float d1, float d2, float k)
  {
    float h = clampf(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return mixf(d2, d1, h) - k * h * (1.0f - h);
  }

  static float sdCloud(Vec2 p, Vec2 a1, Vec2 b1, Vec2 a2, Vec2 b2, float w)
  {
    float lineVal1 = sdLine(p, a1, b1);
    float lineVal2 = sdLine(p, a2, b2);

    Vec2 ww(w * 1.5f, 0.0f);
    Vec2 left  = max2(a1 + ww, a2 + ww);
    Vec2 right = min2(b1 - ww, b2 - ww);
    Vec2 boxCenter = (left + right) * 0.5f;
    float boxH = fabsf(a2.y - a1.y) * 0.5f;

    float boxVal = sdBox(p - boxCenter, Vec2(0.04f, boxH)) + w;

    float uniVal1 = opSmoothUnion(lineVal1, boxVal, 0.05f);
    float uniVal2 = opSmoothUnion(lineVal2, boxVal, 0.05f);

    return fminf(uniVal1, uniVal2);
  }

  // ------------------------- Main -------------------------
  Color TestShader::Update(uint8_t px, uint8_t py, float iTime)
  {
    InitSinLUT();

    Vec2 iResolution((float)W, (float)H);
    Vec2 fragCoord((float)px + 0.5f, (float)py + 0.5f);

    Vec2 uv = (fragCoord * 2.0f - iResolution) * (1.0f / iResolution.y);
    uv.y = -uv.y;

    float battery = 1.0f;

    float fog = smoothstep(0.1f, -0.02f, fabsf(uv.y + 0.2f));
    Vec3 col(0.0f, 0.1f, 0.2f);

    if (uv.y < -0.2f)
    {
      uv.y = 3.0f / (fabsf(uv.y + 0.2f) + 0.05f);
      uv.x *= uv.y;

      float gridVal = grid(uv, battery, iTime);
      col = mix3(col, Vec3(1.0f, 0.5f, 1.0f), gridVal);
    }
    else
    {
      float fujiD = fminf(uv.y * 4.5f - 0.5f, 1.0f);
      uv.y -= battery * 1.1f - 0.51f;

      Vec2 sunUV = uv + Vec2(0.75f, 0.2f);

      col = Vec3(1.0f, 0.2f, 1.0f);
      float sunVal = sun(sunUV, battery, iTime);

      col = mix3(col, Vec3(1.0f, 0.4f, 0.1f), sunUV.y * 2.0f + 0.2f);
      col = mix3(Vec3(0.0f, 0.0f, 0.0f), col, sunVal);

      // Fuji: powf optimized
      float a = uv.y * uv.y;
      float fujiVal = sdTrapezoid(
        uv + Vec2(-0.75f, 0.5f),
        1.75f + pow_a_2p1_approx(a),
        0.2f,
        0.5f
      );

      // Waves: sin optimized
      float waveVal = uv.y + sin_lut(uv.x * 20.0f + iTime * 2.0f) * 0.05f + 0.2f;
      float wave_width = smoothstep(0.0f, 0.01f, waveVal);

      // Fuji color
      col = mix3(
        col,
        mix3(Vec3(0.0f, 0.0f, 0.25f), Vec3(1.0f, 0.0f, 0.5f), fujiD),
        stepf(fujiVal, 0.0f)
      );

      // Fuji top snow
      col = mix3(col, Vec3(1.0f, 0.5f, 1.0f), wave_width * stepf(fujiVal, 0.0f));

      // Fuji outline
      col = mix3(col, Vec3(1.0f, 0.5f, 1.0f), 1.0f - smoothstep(0.0f, 0.01f, fabsf(fujiVal)));

      // Horizon color (faithful to original, but can be “hot”)
      float t = clampf(uv.y * 3.5f + 3.0f, 0.0f, 1.0f);
      Vec3 hor = mix3(Vec3(1.0f, 0.12f, 0.8f), Vec3(0.0f, 0.0f, 0.2f), t);
      col = col + mix3(col, hor, stepf(0.0f, fujiVal));

      // Clouds: sin/cos optimized
      Vec2 cloudUV = uv;
      cloudUV.x = modf_glsl(cloudUV.x + iTime * 0.1f, 4.0f) - 2.0f;

      float cloudTime = iTime * 0.5f;

      float cloudY = -0.5f;
      float cloudVal1 = sdCloud(
        cloudUV,
        Vec2(0.1f  + sin_lut(cloudTime + 140.5f) * 0.1f, cloudY),
        Vec2(1.05f + cos_lut(cloudTime * 0.9f - 36.56f) * 0.1f, cloudY),
        Vec2(0.2f  + cos_lut(cloudTime * 0.867f + 387.165f) * 0.1f, 0.25f + cloudY),
        Vec2(0.5f  + cos_lut(cloudTime * 0.9675f - 15.162f) * 0.09f, 0.25f + cloudY),
        0.075f
      );

      cloudY = -0.6f;
      float cloudVal2 = sdCloud(
        cloudUV,
        Vec2(-0.9f + cos_lut(cloudTime * 1.02f + 541.75f) * 0.1f, cloudY),
        Vec2(-0.5f + sin_lut(cloudTime * 0.9f - 316.56f) * 0.1f, cloudY),
        Vec2(-1.5f + cos_lut(cloudTime * 0.867f + 37.165f) * 0.1f, 0.25f + cloudY),
        Vec2(-0.6f + sin_lut(cloudTime * 0.9675f + 665.162f) * 0.09f, 0.25f + cloudY),
        0.075f
      );

      float cloudVal = fminf(cloudVal1, cloudVal2);

      col = mix3(col, Vec3(0.0f, 0.0f, 0.2f), 1.0f - smoothstep(0.075f - 0.0001f, 0.075f, cloudVal));
      float glow = 1.0f - smoothstep(0.0f, 0.01f, fabsf(cloudVal - 0.075f));
      col = col + Vec3(1.0f, 1.0f, 1.0f) * glow;
    }

    col = col + Vec3(fog*fog*fog, fog*fog*fog, fog*fog*fog);

    Vec3 gray(col.r, col.r, col.r);
    col = mix3(gray * 0.5f, col, battery * 0.7f);

    col.r = clampf(col.r, 0.0f, 1.0f);
    col.g = clampf(col.g, 0.0f, 1.0f);
    col.b = clampf(col.b, 0.0f, 1.0f);

    return Color(col.r, col.g, col.b);
  }
}
