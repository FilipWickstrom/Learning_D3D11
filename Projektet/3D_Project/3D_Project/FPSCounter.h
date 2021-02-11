#pragma once
#include <Windows.h>
#include <chrono>
#include <string>

class FPSCounter
{
private:
	std::chrono::steady_clock::time_point m_lastUpdate;
	float m_lastDeltatime;

public:
	FPSCounter();
	~FPSCounter();
	float GetDeltatime();
	void DisplayInWindow(HWND& window);
};