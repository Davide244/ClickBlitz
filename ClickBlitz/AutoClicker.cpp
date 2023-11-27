#include "AutoClicker.hpp"
#include "Utils.hpp"
#include "ClickBlitz.h"
#include <WinUser.h>
#include <thread>

std::atomic<bool> AutoClicker::AutoClickerActive;

void AutoClicker::AutoClickThread(int Speed, bool CPS, int StopAt, MouseButton SelectedMouseButton) // Speed is either the intervall in ms or the clicks per second, depending on the CPS bool
{
	long long SleepTime_ms;
	if (CPS == true)
		SleepTime_ms = (1.0 / Speed) * 1000;
	else
		SleepTime_ms = Speed;

	unsigned char SelectedInput = 0;
	unsigned char SelectedInputUp = 0;
	switch (SelectedMouseButton)
	{
		case Left:
			SelectedInput = MOUSEEVENTF_LEFTDOWN;
			SelectedInputUp = MOUSEEVENTF_LEFTUP;
			break;
		case Right:
			SelectedInput = MOUSEEVENTF_RIGHTDOWN;
			SelectedInputUp = MOUSEEVENTF_RIGHTUP;
			break;
		case Middle:
			SelectedInput = MOUSEEVENTF_MIDDLEDOWN;
			SelectedInputUp = MOUSEEVENTF_MIDDLEUP;
			break;
	}

	INPUT Inputs[2];
	ZeroMemory(&Inputs, sizeof(INPUT));
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = SelectedInput;
	Inputs[0].mi.time = 0;
	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = SelectedInputUp;
	Inputs[1].mi.time = 0;

	if (StopAt > 0) 
	{
		int i = 0;
		while (AutoClickerActive.load() == true && i < StopAt)
		{
			i++;
			SendInput(2, Inputs, sizeof(INPUT));
			Utils::Sleep(SleepTime_ms);
		}
	}
	else
	{
		while (AutoClickerActive.load() == true)
		{
			SendInput(2, Inputs, sizeof(INPUT));
			Utils::Sleep(SleepTime_ms);
		}
	}

	AutoClickerActive.store(bool(false));
}

void AutoClicker::AutoClick_Intervall(int intervall_ms, int StopAt, MouseButton SelectedMouseButton)
{
	if (AutoClickerActive.load() == true)
		return;

	EnableGUIInputs(false);

	AutoClickerActive.store(bool(true));
	std::thread autoClickThread(&AutoClickThread, intervall_ms, false, StopAt, SelectedMouseButton);
	autoClickThread.detach();
}

void AutoClicker::AutoClick_Rate(int ClicksPerSecond, int StopAt, MouseButton SelectedMouseButton)
{
	if (AutoClickerActive.load() == true)
		return;

	EnableGUIInputs(false);

	AutoClickerActive.store(bool(true));
	std::thread autoClickThread(&AutoClickThread, ClicksPerSecond, true, StopAt, SelectedMouseButton);
	autoClickThread.detach();
}

void AutoClicker::AutoClick_Stop()
{
	if (AutoClickerActive.load() == false) // If the AutoClicker is not active, return
		return;
	AutoClickerActive.store(bool(false));

	EnableGUIInputs(true);
}
