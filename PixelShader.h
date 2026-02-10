#pragma once

#include "DisplayDrawerInterface.h"
#include "Color.h"
#include "Vector2.h"

namespace AD
{
  class PixelShader : public IDisplayDrawer
  {
  public:
    PixelShader(int targetFPS = 30, int prePassesAmount = 0);

    virtual void Update() override;

  protected:
    virtual void Prepass(uint8_t x, uint8_t y, float time, int pass) {}
    virtual Color Update(uint8_t x, uint8_t y, float time) = 0;
    static Vector2 Normalize(uint8_t x, uint8_t y);
  private:
    int m_targetFPS = 30;
    int m_prePassesAmount = 0;
  };
}