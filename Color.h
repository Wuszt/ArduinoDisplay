#pragma once
#include <stdint.h>

namespace AD
{
  class Color
  {
  public:
    Color() = default;

    Color(uint8_t r, uint8_t g, uint8_t b)
      : m_r(r)
      , m_g(g)
      , m_b(b)
    {}

    Color(unsigned r, unsigned g, unsigned b)
      : m_r(r)
      , m_g(g)
      , m_b(b)
    {}

    Color(float r, float g, float b);

    Color(const uint8_t arr[3])
      : Color(arr[0], arr[1], arr[2])
    {}

    Color(const unsigned arr[3])
      : Color(arr[0], arr[1], arr[2])
    {}

    uint8_t GetR() const
    {
      return m_r;
    }

    uint8_t GetG() const
    {
      return m_g;
    }

    uint8_t GetB() const
    {
      return m_b;
    }

    float GetR_AsFloat() const
    {
      return static_cast<float>(m_r) / 255.0f;
    }

    float GetG_AsFloat() const
    {
      return static_cast<float>(m_g) / 255.0f;
    }

    float GetB_AsFloat() const
    {
      return static_cast<float>(m_b) / 255.0f;
    }

    static Color GetRandom();

  private:
    uint8_t m_r = 0u;
    uint8_t m_g = 0u;
    uint8_t m_b = 0u;
  };
}