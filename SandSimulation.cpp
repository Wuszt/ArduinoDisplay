#include "SandSimulation.h"
#include <Arduino.h>

namespace AD
{
   SandSimulation::SandSimulation(int duration)
    : PixelShader(60)
    , m_duration(duration)
  {}

  bool SandSimulation::IsFinished() const
  {
    return millis() >= m_endTime;
  }

  void SandSimulation::Activate()
  {
    m_endTime = millis() + m_duration;

    for (uint8_t y = 0; y < 63; ++y)
    {
      for (uint8_t x = 0; x < 64; ++x)
      {
        m_board.SetInCurrent(x, y, Type::None);
      }
    }

    for (uint8_t x = 0; x < 64; ++x)
    {
      m_board.SetInCurrent(x, 63, Type::Floor);
    }

    int remainingBlocks = 600;
    while(remainingBlocks > 0)
    {
      Vector2 currPoint(random(2, 62), random(10, 63));
      const int currentSize = random(min(25, remainingBlocks), min(50, remainingBlocks));
      remainingBlocks -= currentSize;
      for (int i = 0; i < currentSize;)
      {
        m_board.SetInCurrent(currPoint.X, currPoint.Y, Type::Floor);

        Vector2 tmpVec;
        do
        {
          tmpVec = currPoint + Vector2(random(-1, 2), random(-1, 2));
        }
        while (tmpVec.X < 0 || tmpVec.X > 63 || tmpVec.Y < 5 || tmpVec.Y > 63);

        currPoint = tmpVec;
        if (m_board.GetFromCurrent(currPoint.X, currPoint.Y) != Type::Floor)
        {
          ++i;
        }
      }
    }
  }

  void SandSimulation::Update()
  {
    PixelShader::Update();
    m_board.Swap();
  }

  Color SandSimulation::Update(uint8_t x, uint8_t y, float time)
  {
    m_board.SetInNext(x, y, m_board.GetFromCurrent(x, y));

    if (m_board.GetFromCurrent(x, y) == Type::Floor)
    {
      return Color(0.4f, 0.4f, 0.4f);
    }

    if (y == 0)
    {
      if (x > 16 && x < 50 && 5 > random(0, 100))
      {
        m_board.SetInNext(x, y, Type::Sand);
      }
      else
      {
        m_board.SetInNext(x, y, Type::None);
      }
    }
    else
    {
      if (m_board.GetFromNext(x, y) == Type::None)
      {
        if (m_board.GetFromCurrent(x, y - 1) == Type::Sand)
        {
          m_board.SetInNext(x, y, Type::Sand);
          m_board.SetInCurrent(x, y - 1, Type::None);
        }
        else if (x > 0 && m_board.GetFromCurrent(x - 1, y - 1) == Type::Sand 
              && m_board.GetFromCurrent(x - 1, y) != Type::None 
              && m_board.GetFromCurrent(x, y - 1) != Type::Sand
              && (m_board.GetFromCurrent(x - 1, y) != Type::Floor || m_board.GetFromCurrent(x, y - 1) != Type::Floor))
        {
          m_board.SetInNext(x, y, Type::Sand);
          m_board.SetInCurrent(x - 1, y - 1, Type::None);
        }
        else if (x < 63 && m_board.GetFromCurrent(x + 1, y - 1) == Type::Sand 
              && m_board.GetFromCurrent(x + 1, y) != Type::None
              && m_board.GetFromCurrent(x, y - 1) != Type::Sand
              && (m_board.GetFromCurrent(x + 1, y) != Type::Floor || m_board.GetFromCurrent(x, y - 1) != Type::Floor)
              && (random(0,100) > 50 || (x < 62 && (m_board.GetFromCurrent(x + 2, y) != Type::None || m_board.GetFromCurrent(x + 2, y - 1) == Type::Sand))))
        {
          m_board.SetInNext(x, y, Type::Sand);
          m_board.SetInCurrent(x + 1, y - 1, Type::None);
        }
      }
    }

    return m_board.GetFromNext(x, y) == Type::Sand ? Color(0.75f, 0.75f, 0.0f) : Color(16u, 16u, 16u);
  }
}