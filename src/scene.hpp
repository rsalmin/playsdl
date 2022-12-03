#pragma once

#include <chrono>
#include <vector>
#include "ball.hpp"
#include "context.hpp"

class Scene
{
public:
  Scene();

  void update(const Context&, const std::chrono::steady_clock::duration&);

  void render(Context&);

private:
  std::vector<Ball> m_balls;
};