#pragma once

#include "PixelShader.h"
#include "Board.h"

namespace AD
{
  class SandSimulation : public PixelShader
  {
  public:
    SandSimulation(int duration);

    virtual void Activate() override;
    virtual void Update() override;
    virtual bool IsFinished() const override;

  protected:
    virtual Color Update(uint8_t x, uint8_t y, float time) override;

  private:
    enum class Type
    {
      None,
      Sand,
      Floor
    };

    Board<Type> m_board;
    int m_endTime = 0;
    int m_duration = 0;
  };
}