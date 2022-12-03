#include "scene.hpp"
#include <random>

Scene::Scene()
{
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> rndX(0, 1280);
  std::uniform_int_distribution<std::mt19937::result_type> rndY(0, 960);
  std::uniform_int_distribution<std::mt19937::result_type> rndVX(100, 500);
  std::uniform_int_distribution<std::mt19937::result_type> rndVY(100, 500);
  std::uniform_int_distribution<std::mt19937::result_type> rndR(5,50);
  std::uniform_int_distribution<std::mt19937::result_type> rndSign(0,1);

  for(size_t i = 0; i < 10; i++)
  {
     const int signX = 1 - rndSign(rng)*2;
     const int signY = 1 - rndSign(rng)*2;
     const vec2 rndVelocity (static_cast<int>(rndVX(rng))*signX, static_cast<int>(rndVY(rng))*signY);
     m_balls.emplace_back(vec2(rndX(rng), rndY(rng)), rndR(rng), rndVelocity);
   }
}

void Scene::update(const Context& ctx, const std::chrono::steady_clock::duration& dt)
{
  const float dtMilliseconds =  std::chrono::duration_cast<std::chrono::milliseconds>(dt).count() / 1000.0;

  for(Ball&b : m_balls)
  {
      b.p() += b.v() * dtMilliseconds;

      if ( b.p().x() > ctx.width() )
      {
        b.p().x() = ctx.width();
        b.v().x() = - b.v().x();
      }

      if ( b.p().x() < 0 )
      {
        b.p().x() = 0;
        b.v().x() = - b.v().x();
      }

      if ( b.p().y() > ctx.height() )
      {
        b.p().y() = ctx.height();
        b.v().y() = - b.v().y();
      }

      if (  b.p().y() < 0 )
      {
        b.p().y() = 0;
        b.v().y() = - b.v().y();
      }
  }
}

void Scene::render(Context& ctx)
{
  for(Ball& b: m_balls)
    b.render(ctx);
}