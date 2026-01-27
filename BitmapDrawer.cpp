#include <stdio.h>
#include "BitmapDrawer.h"
#include "BitmapParser.h"
#include "DisplayManager.h"
#include "Color.h"
#include <Arduino.h>

namespace AD
{
  BitmapDrawer::BitmapDrawer(const char* path, int duration)
    : m_path(path)
    , m_duration(duration)
  {}

  bool BitmapDrawer::IsFinished() const
  {
    return millis() >= m_endTime;
  }

  void BitmapDrawer::Activate()
  {
    m_endTime = millis() + m_duration;
    std::unique_ptr<Color[][64]> colors = std::make_unique<Color[][64]>(64);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "/%s.bmp", m_path);
    if (!AD::LoadBMP24_ToArrayYX(buffer, colors.get()))
    {
      Serial.printf("Failed to load: %s \n", buffer);
      return;
    }

    for (uint8_t y = 0u; y < 64u; ++y)
    {
      for (uint8_t x = 0u; x < 64u; ++x)
      {
        DisplayManager::Get().SetPixel(x, y, colors[y][x]);
      }
    }

    DisplayManager::Get().FlipScreen();
  }
}