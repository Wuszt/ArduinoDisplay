#include "PixelShader.h"
#include "BitmapDrawer.h"
#include "DisplayManager.h"
#include <Arduino.h>

namespace AD
{
  PixelShader::PixelShader(int targetFPS, int prePassesAmount)
    : m_targetFPS(targetFPS)
    , m_prePassesAmount(prePassesAmount)
  {}

  Vector2 PixelShader::Normalize(uint8_t x, uint8_t y)
  {
    return {static_cast<float>(x) / 64.0f, static_cast<float>(y) / 64.0f};
  }

  void PixelShader::Update()
  {
    const int time = millis();

    for (int prepass = 0; prepass < m_prePassesAmount; ++prepass)
    {
      for (uint8_t y = 0u; y < 64u; ++y)
      {
        for (uint8_t x = 0u; x < 64u; ++x)
        {
          Prepass(x, 63 - y, static_cast<float>(time) / 500.0f, prepass);
        }
      }
    }

    for (uint8_t y = 0u; y < 64u; ++y)
    {
      for (uint8_t x = 0u; x < 64u; ++x)
      {
        DisplayManager::Get().SetPixel(x, 63 - y, Update(x, 63 - y, static_cast<float>(time) / 500.0f));
      }
    }

    DisplayManager::Get().FlipScreen();

    const int targetTime = 1000 / m_targetFPS;
    const int elapsedTime = millis() - time;
    if (elapsedTime < targetTime)
    {
      delay(targetTime - elapsedTime);
    }
  }
}