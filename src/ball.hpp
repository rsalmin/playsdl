#pragma once

#include "context.hpp"
#include <iostream>

class vec2
{
public:
  explicit vec2(float x, float y): m_x{x}, m_y{y} {}

  float x() const noexcept {return m_x;}
  float y() const noexcept {return m_y;}

  float& x() noexcept {return m_x;}
  float& y() noexcept {return m_y;}

  vec2& operator+=(const vec2& v) noexcept
  {
     m_x += v.m_x;
     m_y += v.m_y;
     return *this;
  }
protected:
  float m_x{0};
  float m_y{0};
};

inline vec2 operator*(const vec2& v, float k)
{
   return vec2{v.x()*k, v.y()*k};
}

inline std::ostream& operator<<(std::ostream& os, const vec2& v)
{
  os << "{" << v.x() << ", " << v.y() << "}";
  return os;
}

class Ball
{
public:
  Ball(vec2 p, float r, vec2 v): m_p{std::move(p)}, m_r{r}, m_v{std::move(v)} {}

  vec2& p() noexcept {return m_p;}
  const vec2& p() const noexcept {return m_p;}

  vec2& v() noexcept {return m_v;}
  const vec2& v() const noexcept {return m_v;}

  void render(Context& ctx)
  {
      SDL_Rect fillRect = { static_cast<int>(m_p.x() - m_r), static_cast<int>(m_p.y() - m_r),
                                          static_cast<int>(2 * m_r), static_cast<int>(2 * m_r) };
      SDL_SetRenderDrawColor( ctx.renderer(), 0xFF, 0x00, 0x00, 0xFF );
      SDL_RenderFillRect( ctx.renderer(), &fillRect );
  }

protected:
  vec2 m_p{0.0, 0.0};
  float m_r {0.0};
  vec2 m_v{0.0, 0.0};
};
