#include "DisplayManager.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "Color.h"

namespace AD
{
  DisplayManager& DisplayManager::Get()
  {
    static DisplayManager instance{64, 64};
    return instance;
  }

  DisplayManager::DisplayManager(uint16_t width, uint16_t height)
  {
    HUB75_I2S_CFG config(width, height, 1);
    config.gpio.r1 = 25;
    config.gpio.g1 = 26;
    config.gpio.b1 = 27;
    config.gpio.r2 = 14;
    config.gpio.g2 = 12;
    config.gpio.b2 = 13;
    
    config.gpio.a = 23;
    config.gpio.b = 22;
    config.gpio.c = 5;
    config.gpio.d = 17;
    config.gpio.e = 18;
    
    config.gpio.clk = 16;
    config.gpio.lat = 4;
    config.gpio.oe = 15;
    //config.double_buff = true;

    m_display = std::make_unique<MatrixPanel_I2S_DMA>(config);
    m_display->begin();
    m_display->setBrightness8(128);
    m_display->clearScreen();
  }

  void DisplayManager::ClearScreen()
  {
    m_display->clearScreen();
  }

  void DisplayManager::FlipScreen()
  {
    m_display->flipDMABuffer();
  }

  void DisplayManager::SetPixel(uint8_t x, uint8_t y, const Color& color)
  {
    m_display->drawPixelRGB888(x, y, color.GetR(), color.GetG(), color.GetB());
  }

  DisplayManager::~DisplayManager() = default;
}