#include "BitmapParser.h"
#include <Arduino.h>
#include <LittleFS.h>
#include "Color.h"

static uint16_t readLE16(File &f) 
{
  uint8_t b0 = f.read();
  uint8_t b1 = f.read();
  return (uint16_t)b0 | ((uint16_t)b1 << 8);
}

static uint32_t readLE32(File &f) 
{
  uint32_t b0 = f.read();
  uint32_t b1 = f.read();
  uint32_t b2 = f.read();
  uint32_t b3 = f.read();
  return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

bool AD::LoadBMP24_ToArrayYX(const char* path, Color out[64][64]) 
{
  File f = LittleFS.open(path, "r");
  if (!f)
  {
    Serial.printf("[%hs] Wrong file\n", __FUNCTION__);
    return false;
  }

  // --- BITMAPFILEHEADER ---
  if (f.read() != 'B' || f.read() != 'M') {
    f.close(); return false;
  }

  readLE32(f); // file size
  readLE16(f); // reserved
  readLE16(f);
  uint32_t pixelOffset = readLE32(f);

  // --- DIB HEADER ---
  uint32_t dibSize = readLE32(f);
  if (dibSize < 40) { f.close(); return false; }

  int32_t width  = (int32_t)readLE32(f);
  int32_t height = (int32_t)readLE32(f);
  uint16_t planes = readLE16(f);
  uint16_t bpp    = readLE16(f);
  uint32_t compression = readLE32(f);

  // skip rest of DIB
  f.seek(14 + dibSize);

  if (planes != 1 || bpp != 24 || compression != 0) 
  {
    f.close(); 
    return false;
  }

  bool topDown = false;
  if (height < 0) {
    topDown = true;
    height = -height;
  }

  if (width != 64 || height != 64) 
  {
    f.close(); 
    return false;
  }

  const uint32_t rowBytes = width * 3;
  const uint32_t paddedRowBytes = (rowBytes + 3) & ~3;

  static uint8_t row[192 + 4]; // 64*3 + padding

  for (int y = 0; y < 64; ++y) 
  {
    int srcY = topDown ? y : (63 - y);
    uint32_t rowPos = pixelOffset + srcY * paddedRowBytes;

    f.seek(rowPos);
    if (f.read(row, paddedRowBytes) != (int)paddedRowBytes) 
    {
      f.close();
      return false;
    }

    for (int x = 0; x < 64; ++x) 
    {
      Color& color = out[y][x];
      color = Color(row[x * 3 + 2], row[x * 3 + 1], row[x * 3]);
    }
  }

  f.close();
  return true;
}