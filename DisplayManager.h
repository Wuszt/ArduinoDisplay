#pragma once

#include <memory>
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

    void ClearScreen();
    void FlipScreen();
    void SetPixel(uint8_t x, uint8_t y, const Color& color);

  private:
    std::unique_ptr<MatrixPanel_I2S_DMA> m_display;
  };
}