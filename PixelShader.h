#pragma once

#include "DisplayDrawerInterface.h"
#include "Color.h"
#include "Vector2.h"

namespace AD
{
  class PixelShader : public IDisplayDrawer
  {
  protected:
    virtual Color Update(uint8_t x, uint8_t y, float time) = 0;
    static Vector2 Normalize(uint8_t x, uint8_t y);

  private:
    virtual void Update() override;
  };
}