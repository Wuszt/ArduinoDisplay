#pragma once

#include <memory>
#include "Color.h"
class MatrixPanel_I2S_DMA;

namespace AD
{
  class Color;
  class DisplayManager
  {
  public:
    DisplayManager(uint16_t width, uint16_t height);
    ~DisplayManager();
    static DisplayManager& Get();

    void Tick();

    void ClearScreen(const Color& color = Color(0.0f, 0.0f, 0.0f));
    void FlipScreen();
    void SetPixel(uint8_t x, uint8_t y, const Color& color);

  private:
    std::unique_ptr<MatrixPanel_I2S_DMA> m_display;
    float m_currentBrightness = 0.5f;
    int m_lastUpdateTimestamp;
  };
}