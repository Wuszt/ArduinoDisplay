#pragma once

namespace AD
{
  class Color;
  bool LoadBMP24_ToArrayYX(const char* path, Color out[64][64]);
}