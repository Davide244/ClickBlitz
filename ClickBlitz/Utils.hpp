#pragma once
#include "framework.h"
#include <string>


namespace Utils
{
	BOOLEAN nanosleep(LONGLONG ns);
	void Sleep(double ms);
	std::string GetKeyNameTextWrapper(uint16_t scanCode);
};

