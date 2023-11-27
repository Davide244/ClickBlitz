#pragma once
#include <atomic>

static enum MouseButton
{
	Left,
	Right,
	Middle
};

class AutoClicker
{
private:
	// If "CPS" is true, the speed will be calculated in clicks per second, if false, the speed will be calculated in millisecond intervals
	static void AutoClickThread(int Speed, bool CPS, int StopAt, MouseButton SelectedMouseButton = Left);
public:
	void AutoClick_Intervall(int intervall_ms, int StopAt, MouseButton SelectedMouseButton);
	void AutoClick_Rate(int ClicksPerSecond, int StopAt, MouseButton SelectedMouseButton);
	void AutoClick_Stop();

	static std::atomic<bool> AutoClickerActive;
};

