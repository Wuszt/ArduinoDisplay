#include "Color.h"
#include <algorithm>
#include <Arduino.h>

namespace AD
{
  Color::Color(float r, float g, float b)
  {
    m_r = static_cast<uint8_t>(std::min(1.0f, std::max(0.0f, r)) * 255.0f + 0.5f);
    m_g = static_cast<uint8_t>(std::min(1.0f, std::max(0.0f, g)) * 255.0f + 0.5f);   
    m_b = static_cast<uint8_t>(std::min(1.0f, std::max(0.0f, b)) * 255.0f + 0.5f);
  }

  Color Color::GetRandom()
  {
    Color clr;
    clr.m_r = random(0, 255);
    clr.m_g = random(0, 255);
    clr.m_b = random(0, 255);
    return clr;
  }
}