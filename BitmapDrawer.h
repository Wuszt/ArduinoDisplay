#pragma once

#include "DisplayDrawerInterface.h"

namespace AD
{
  class BitmapDrawer : public IDisplayDrawer
  {
  public:
    BitmapDrawer(const char* path, int duration);
    virtual void Activate() override;

    virtual bool IsFinished() const override;

  private:
    const char* m_path = nullptr;
    int m_endTime = 0;
    int m_duration = 0;
  };
}