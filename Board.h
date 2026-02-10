#pragma once

#include <bitset>
#include <array>
#include <type_traits>

namespace AD
{
  namespace Internal
  {
    template<class T>
    struct BoardStorage
    {
      std::array<T, 64u * 64u> m_storage;
    };

    template<>
    struct BoardStorage<bool>
    {
      std::bitset<64u * 64u> m_storage;
    };

    inline uint32_t GetIndex(int x, int y)
    {
      return y * 64 + x;
    }
  }

  template<class T>
  class Board
  {
  public:
    std::conditional_t<std::is_fundamental_v<T>, T, T&> GetFromCurrent(int x, int y)
    {
      return m_storage[m_currentIndex].m_storage[Internal::GetIndex(x, y)];
    }

    std::conditional_t<std::is_fundamental_v<T>, T, T&> GetFromNext(int x, int y)
    {
      return m_storage[(m_currentIndex + 1) % 2].m_storage[Internal::GetIndex(x, y)];
    }

    void SetInCurrent(int x, int y, T value)
    {
      m_storage[m_currentIndex].m_storage[Internal::GetIndex(x, y)] = std::move(value);
    }

    void SetInNext(int x, int y, T value)
    {
      m_storage[(m_currentIndex + 1) % 2].m_storage[Internal::GetIndex(x, y)] = std::move(value);
    }

    void Swap()
    {
      m_currentIndex = (m_currentIndex + 1) % 2;
    }

  protected:
    Internal::BoardStorage<T> m_storage[2];
    uint8_t m_currentIndex = 0u;
  };
}