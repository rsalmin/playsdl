#pragma once

#include <chrono>

class FPSCounter
{
public:
    FPSCounter();
    FPSCounter& operator++();
    std::uint32_t fps10() const noexcept {return m_fps10;}
private:
    std::uint32_t m_iFrame{0};
    std::chrono::steady_clock::time_point m_startPoint;
    std::uint32_t m_fps10{0};
};
