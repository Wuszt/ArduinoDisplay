#pragma once

namespace AD
{
  class IDisplayDrawer
  {
  public:
    virtual ~IDisplayDrawer() = default;
    
    virtual void Activate() {}
    virtual void Update() {}
    virtual void Deactivate() {}

    virtual bool IsFinished() const = 0;
  };
}