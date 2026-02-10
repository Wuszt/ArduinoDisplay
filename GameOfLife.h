#include "PixelShader.h"
#include "Board.h"

namespace AD
{
  class GameOfLife : public PixelShader
  {
  public:
    GameOfLife(int duration);

    virtual void Activate() override;

    virtual bool IsFinished() const override;

    virtual void Update() override;

  protected:
    virtual Color Update(uint8_t x, uint8_t y, float time) override;

  private:
    uint32_t CountNeighbours(int x, int y);

    Board<bool> m_board;

    int m_endTime = 0;
    int m_duration = 0;
  };
}