#pragma once
#include <Windows.h>
#include <chrono>
#include <string>

class FPSCounter
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start;
	std::chrono::time_point<std::chrono::steady_clock> m_stop;
	bool m_running;
	int m_FPS;
	float m_timeSinceUpdate;

public:
	FPSCounter();
	~FPSCounter();
	
	bool StartClock();
	bool StopClock();
	void RestartClock();
	
	float GetDeltatime();
	void DisplayInWindow(HWND& window);
};