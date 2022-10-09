#include "fps_counter.hpp"

FPSCounter::FPSCounter()
{
    m_startPoint = std::chrono::steady_clock::now();
}

FPSCounter& FPSCounter::operator++()
{
    const std::uint32_t framesToCount = 100;
    m_iFrame++;
    if (m_iFrame % framesToCount == 0)
    {
        auto now = std::chrono::steady_clock::now();
        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startPoint).count();
        m_fps10= static_cast<float>(10 * 1000 * framesToCount) / delta;
        m_startPoint = now;
    }
     return *this;
}
