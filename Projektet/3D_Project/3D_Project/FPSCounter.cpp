#include "FPSCounter.h"

FPSCounter::FPSCounter()
{
    m_lastUpdate = std::chrono::steady_clock::now();
    m_lastDeltatime = 0.0f;
}

FPSCounter::~FPSCounter()
{
}

float FPSCounter::GetDeltatime()
{
    std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
    std::chrono::duration<float>deltatime = timeNow - m_lastUpdate;
    m_lastUpdate = timeNow;
    m_lastDeltatime = deltatime.count();
    return m_lastDeltatime;
}

void FPSCounter::DisplayInWindow(HWND& window)
{
    std::string str = "3D project | FPS: " + std::to_string((int)(1 / m_lastDeltatime)) + " | Frametime: " + std::to_string(m_lastDeltatime*1000) + "ms";
    std::wstring wstr = std::wstring(str.begin(), str.end());
    SetWindowText(window, wstr.c_str());
}
