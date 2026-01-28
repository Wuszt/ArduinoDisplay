#include "PixelShader.h"
#include "BitmapDrawer.h"
#include "DisplayManager.h"
#include <Arduino.h>

namespace AD
{
  Vector2 PixelShader::Normalize(uint8_t x, uint8_t y)
  {
    return {static_cast<float>(x) / 64.0f, static_cast<float>(y) / 64.0f};
  }

  void PixelShader::Update()
  {
    int time = millis();
    for (uint8_t y = 0u; y < 64u; ++y)
    {
      for (uint8_t x = 0u; x < 64u; ++x)
      {
        DisplayManager::Get().SetPixel(x, y, Update(x, y, static_cast<float>(time) / 500.0f));
      }
    }

    DisplayManager::Get().FlipScreen();
  }
}