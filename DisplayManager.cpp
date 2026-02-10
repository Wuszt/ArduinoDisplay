#include "DisplayManager.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Arduino.h>

namespace AD
{
  DisplayManager& DisplayManager::Get()
  {
    static DisplayManager instance{64, 64};
    return instance;
  }

  float GetTargetBrightness()
  {
    const int value = analogRead(33);
    return max(0.0f, min(1.0f, 1.0f - (static_cast<float>(value) / 4095.0f)));
  }

  DisplayManager::DisplayManager(uint16_t width, uint16_t height)
  {
    m_lastUpdateTimestamp -std::numeric_limits<int>::min();

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
    config.clkphase = false;
    //config.double_buff = true;

    m_display = std::make_unique<MatrixPanel_I2S_DMA>(config);
    m_display->begin();
    m_display->clearScreen();
  }

  void DisplayManager::Tick()
  {
    const int currentTime = millis();
    const float delta = static_cast<float>(currentTime - m_lastUpdateTimestamp) / 10000.0f;
    const float normalizedValue = GetTargetBrightness();
    m_currentBrightness = max(0.0f, min(1.0f, m_currentBrightness + (normalizedValue - m_currentBrightness) * delta));
    const uint8_t minBrightness = 4u;
    const uint8_t maxBrithness = 50u; 
    m_display->setBrightness8(minBrightness + static_cast<uint8_t>((maxBrithness - minBrightness) * m_currentBrightness));
    m_lastUpdateTimestamp = currentTime;
  }

  void DisplayManager::ClearScreen(const Color& color)
  {
    m_display->fillScreenRGB888(color.GetR(), color.GetG(), color.GetB());
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