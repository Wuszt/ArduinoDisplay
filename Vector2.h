#pragma once
#include <cmath>

namespace AD
{
  struct Vector2
  {
    float X = 0.0f;
    float Y = 0.0f;

    Vector2() {}

    Vector2( float x, float y )
      : X( x )
      , Y( y )
    {}

    explicit Vector2( float xy )
      : X( xy )
      , Y( xy )
    {}

    ~Vector2() {}

    static Vector2 ZEROS() { return Vector2( 0.0f ); }
    static Vector2 ONES() { return Vector2( 1.0f ); }
    static Vector2 PLUS_MAX();
    static Vector2 PLUS_INF();
    static Vector2 MINUS_MAX();
    static Vector2 MINUS_INF();
    static Vector2 EX() { return Vector2( 1.0f, 0.0f ); }
    static Vector2 EY() { return Vector2( 0.0f, 1.0f ); }

    float SquareMag() const { return X * X + Y * Y; }

    float Mag() const
    {
      return std::sqrt( X * X + Y * Y );
    }

    float Normalize()
    {
      float mag = Mag();
      X /= mag;
      Y /= mag;
      return mag;
    }

    float DistTo( const Vector2& to ) const
    {
      return ( *this - to ).Mag();
    }

    float SquareDistTo( const Vector2& to ) const
    {
      return ( *this - to ).SquareMag();
    }

    Vector2 Normalized() const
    {
      Vector2 result = *this;
      result.Normalize();
      return result;
    }

    float Dot( const Vector2& vec ) const { return X * vec.X + Y * vec.Y; }

    Vector2 operator-() const { return Vector2( -X, -Y ); }

    Vector2 operator*( float value ) const { return Vector2( value * X, value * Y ); }
    Vector2 operator+( const Vector2& vec ) const { return Vector2( X + vec.X, Y + vec.Y ); }
    Vector2 operator-( const Vector2& vec ) const { return Vector2( X - vec.X, Y - vec.Y ); }

    void operator+=( const Vector2& vec )
    {
      X += vec.X;
      Y += vec.Y;
    }

    void operator-=( const Vector2& vec )
    {
      X -= vec.X;
      Y -= vec.Y;
    }

    void operator*=( float val )
    {
      X *= val;
      Y *= val;
    }

    void operator/=( float val )
    {
      X /= val;
      Y /= val;
    }

    const float* AsArray() const
    {
      return &X;
    }

    float* AsArray()
    {
      return &X;
    }

    bool operator==( const Vector2& vec ) const { return X == vec.X && Y == vec.Y; }

    bool operator!=( const Vector2& vec ) const { return X != vec.X || Y != vec.Y; }

    bool IsZero() const { return X == 0.0f && Y == 0.0f; }
    bool IsAlmostZero() const
    {
      return abs( X ) <= std::numeric_limits< decltype( X ) >::epsilon() && abs( Y ) <= std::numeric_limits< decltype( Y ) >::epsilon();
    }
  };
}
