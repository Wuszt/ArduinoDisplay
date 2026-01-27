#include "GifDrawer.h"
#include "DisplayManager.h"
#include "Color.h"
#include <LittleFS.h>
#include <AnimatedGIF.h>

namespace AD
{
  static Color ConvertToColor(const uint16_t color)
  {
    uint8_t r = 0u;
    uint8_t g = 0u;
    uint8_t b = 0u;

    r = (color >> 8) & 0xf8;
    g = (color >> 3) & 0xfc;
    b = (color << 3);
    r |= r >> 5;
    g |= g >> 6;
    b |= b >> 5;

    return Color(r, g, b);
  }

  static void* OpenFile(const char* path, int32_t* outSize)
  {
    File* file = new File(LittleFS.open(path, "r"));
    *outSize = file->size();
    return static_cast<void*>(file);
  }

  static void CloseFile(void* handle)
  {
    File* file = static_cast<File*>(handle);
    file->close();
    delete file;
  }

  static int32_t ReadFile(GIFFILE* gifFile, uint8_t* buffer, int32_t len)
  {
    int32_t bytesRead = len;
    File* file = static_cast<File*>(gifFile->fHandle);
    if ((gifFile->iSize - gifFile->iPos) < len)
    {
      bytesRead = gifFile->iSize - gifFile->iPos - 1;
    }
        
    if (bytesRead <= 0)
    {
      return 0;
    }

    bytesRead = static_cast<int32_t>(file->read(buffer, bytesRead));
    gifFile->iPos = file->position();
    return bytesRead;
  }

  static int32_t SeekFile(GIFFILE* gifFile, int32_t position)
  {
    int time = micros();
    File* file = static_cast<File*>(gifFile->fHandle);
    file->seek(position);
    gifFile->iPos = static_cast<int32_t>(file->position());
    time = micros() - time;
    return gifFile->iPos;
  }

  void Draw(GIFDRAW *pDraw)
  {
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;
    iWidth = pDraw->iWidth;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x = 0; x < iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
          s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + pDraw->iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while (x < pDraw->iWidth)
      {
        c = ucTransparent - 1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
            *d++ = usPalette[c];
            iCount++;
          }
        }           // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          for (int xOffset = 0; xOffset < iCount; xOffset++)
          {
            DisplayManager::Get().SetPixel(x + xOffset + pDraw->iX, y, ConvertToColor(usTemp[xOffset]));
          }
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
            iCount++;
          else
            s--;
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x = 0; x < pDraw->iWidth; x++)
      {
        DisplayManager::Get().SetPixel(x + pDraw->iX, y, ConvertToColor(usPalette[*s++]));
      }
    }
  } /* GIFDraw() */

  GifDrawer::GifDrawer(const char* path, uint8_t loops)
    : m_path(path)
    , m_loopsAmount(loops)
  {}

  GifDrawer::~GifDrawer()
  {}

  bool GifDrawer::IsFinished() const
  {
    return m_loopsCounter <= 0;
  }

  void GifDrawer::Activate()
  {
    m_loopsCounter = m_loopsAmount;
    m_gif = std::make_unique<AnimatedGIF>();
    m_gif->begin(LITTLE_ENDIAN_PIXELS);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "/%s.gif", m_path);

    if (!m_gif->open(buffer, OpenFile, CloseFile, ReadFile, SeekFile, Draw))
    {
      Serial.printf("[%hs] Failed to open gif: %s", __FUNCTION__, buffer);
    }

    DisplayManager::Get().ClearScreen();
  }

  void GifDrawer::Update()
  {
    if (m_gif->playFrame(true, nullptr) == 0)
    {
      --m_loopsCounter;
    }
    DisplayManager::Get().FlipScreen();
  }

  void GifDrawer::Deactivate()
  {
    m_gif->close();
    m_gif = nullptr;
  }
}
