#pragma once

#include "DisplayDrawerInterface.h"
#include <memory>

class AnimatedGIF;

namespace AD
{
  class GifDrawer : public IDisplayDrawer
  {
  public:
    GifDrawer(const char* path, uint8_t loops);
    ~GifDrawer();
    
    virtual void Activate() override;
    virtual void Update() override;
    virtual void Deactivate() override;

    virtual bool IsFinished() const override;

  private:
    std::unique_ptr<AnimatedGIF> m_gif;
    const char* m_path = nullptr;
    int m_loopsCounter = 0;
    int m_loopsAmount = 0;
  };
}