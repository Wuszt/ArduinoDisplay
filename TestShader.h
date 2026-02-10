#include "PixelShader.h"

namespace AD
{
  class TestShader : public PixelShader
  {
  public:
    virtual bool IsFinished() const override
    {
      return false;
    }

  protected:
    virtual Color Update(uint8_t x, uint8_t y, float time) override;
  };
}