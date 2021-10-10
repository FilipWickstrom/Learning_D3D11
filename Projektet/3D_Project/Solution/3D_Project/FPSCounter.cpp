#include "FPSCounter.h"

FPSCounter::FPSCounter()
{
    m_start = std::chrono::high_resolution_clock::now();
    m_stop = m_start;
    m_running = false;
    m_FPS = 0;
    m_timeSinceUpdate = 0.0f;
}

FPSCounter::~FPSCounter()
{
}

bool FPSCounter::StartClock()
{
    if (m_running)
    {
        return false;
    }
    else
    {
        m_start = std::chrono::high_resolution_clock::now();
        m_running = true;
        return true;
    }
}

bool FPSCounter::StopClock()
{
    if (!m_running)
    {
        return false;
    }
    else
    {
        m_stop = std::chrono::high_resolution_clock::now();
        m_running = false;
        return true;
    }
}

void FPSCounter::RestartClock()
{
    m_running = true;
    m_start = std::chrono::high_resolution_clock::now();
}

float FPSCounter::GetDeltatime()
{
    if (m_running)
    {
        auto deltatime = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - m_start);
        return deltatime.count();
    }
    else
    {
        auto deltatime = std::chrono::duration<float, std::milli>(m_stop - m_start);
        return deltatime.count();
    }
}

void FPSCounter::DisplayInWindow(HWND& window)
{
    m_FPS++;
    m_timeSinceUpdate += GetDeltatime();

    //Update the fps and deltatime in window every 1 second
    if (m_timeSinceUpdate > 1000.0f)
    {
        std::string str = "3D project | FPS: " + std::to_string(m_FPS) + " | Frametime: " + std::to_string(GetDeltatime()) + "ms";
        std::wstring wstr = std::wstring(str.begin(), str.end());
        SetWindowText(window, wstr.c_str());
        m_FPS = 0;
        m_timeSinceUpdate = 0.0f;
    }
}

