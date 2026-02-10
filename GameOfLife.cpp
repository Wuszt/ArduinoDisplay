#include "GameOfLife.h"
#include <Arduino.h>
#include "BitmapParser.h"
#include <memory>

namespace AD
{
  uint32_t GameOfLife::CountNeighbours(int x, int y)
  {
    int counter = 0;
    for(int yy = max(y - 1, 0); yy <= min(y + 1, 63); ++yy)
    {
      for(int xx = max(x - 1, 0); xx <= min(x + 1, 63); ++xx)
      {
        counter += (m_board.GetFromCurrent(xx, yy) ? 1 : 0);
      }
    }

    return counter - (m_board.GetFromCurrent(x, y) ? 1 : 0);
  }

  GameOfLife::GameOfLife(int duration)
    : PixelShader(10)
    , m_duration(duration)
  {}

  bool GameOfLife::IsFinished() const
  {
    return millis() >= m_endTime;
  }

  void GameOfLife::Activate()
  {
    m_endTime = millis() + m_duration;
    for (int index = 0; index < random(30u * 30u, 64u * 64u); ++index)
    {
      m_board.SetInCurrent(random(0, 64), random(0, 64), true);
    }
  }

  void GameOfLife::Update()
  {
    PixelShader::Update();
    m_board.Swap();
  }

  Color GameOfLife::Update(uint8_t x, uint8_t y, float time)
  {
    const uint32_t neighboursAmount = CountNeighbours(x, y);

    m_board.SetInNext(x, y, m_board.GetFromCurrent(x, y));

    if (m_board.GetFromCurrent(x, y))
    {
      if (neighboursAmount < 2 || neighboursAmount > 3)
      {
        m_board.SetInNext(x, y, false);
      }
    }
    else
    {
      if (neighboursAmount == 3)
      {
        m_board.SetInNext(x, y, true);
      }
    }

    randomSeed((x << 8) | y);
    return m_board.GetFromCurrent(x, y) ? Color::GetRandom() : Color();
  }
}
