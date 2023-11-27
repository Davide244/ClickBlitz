#include "Utils.hpp"
#include <chrono>
#include <unordered_map>

BOOLEAN Utils::nanosleep(LONGLONG ns)
{
    HANDLE timer;
    LARGE_INTEGER li;   /* Time defintion */

    if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return FALSE;

    /* Set timer properties */
    li.QuadPart = -ns;
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
        CloseHandle(timer);
        return FALSE;
    }
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
    return TRUE;
}

void Utils::Sleep(double ms)
{
	// Get current time
	auto start = std::chrono::high_resolution_clock::now();

	// Loop until the time has passed
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < ms)
    {
		// Do nothing
	}
}

// Key name UNORDERED_MAP structure:
// { VK_KEY, "Key name" }
std::unordered_map<int, std::string> keyNames = 
{
    { VK_LBUTTON, "Left Mouse" },
	{ VK_RBUTTON, "Right Mouse" },
	{ VK_MBUTTON, "Middle Mouse" },
	{ VK_XBUTTON1, "Mouse X1" },
	{ VK_XBUTTON2, "Mouse X2" },
	{ VK_BACK, "Backspace" },
	{ VK_TAB, "Tab" },
	{ VK_CLEAR, "Clear" },
	{ VK_RETURN, "Enter" },
	{ VK_SHIFT, "Shift" },
	{ VK_CONTROL, "Ctrl" },
	{ VK_MENU, "Alt" },
	{ VK_PAUSE, "Pause" },
	{ VK_CAPITAL, "Caps Lock" },
	{ VK_ESCAPE, "Esc" },
	{ VK_SPACE, "Space" },
	{ VK_PRIOR, "Page Up" },
	{ VK_NEXT, "Page Down" },
	{ VK_END, "End" },
	{ VK_HOME, "Home" },
	{ VK_LEFT, "Left arrow" },
	{ VK_UP, "Up arrow" },
	{ VK_RIGHT, "Right arrow" },
	{ VK_DOWN, "Down arrow" },
	{ VK_SELECT, "Select" },
	{ VK_PRINT, "Print" },
	{ VK_EXECUTE, "Execute" },
	{ VK_SNAPSHOT, "Print Screen" },
	{ VK_INSERT, "Insert" },
	{ VK_DELETE, "Delete" },
	{ VK_HELP, "Help" },
	{ VK_LWIN, "Left Windows" },
	{ VK_RWIN, "Right Windows" },
	{ VK_APPS, "Applications" },
	{ VK_SLEEP, "Sleep" },
	{ VK_NUMPAD0, "Numpad 0" },
	{ VK_NUMPAD1, "Numpad 1" },
	{ VK_NUMPAD2, "Numpad 2" },
	{ VK_NUMPAD3, "Numpad 3" },
	{ VK_NUMPAD4, "Numpad 4" },
	{ VK_NUMPAD5, "Numpad 5" },
	{ VK_NUMPAD6, "Numpad 6" },
	{ VK_NUMPAD7, "Numpad 7" },
	{ VK_NUMPAD8, "Numpad 8" },
	{ VK_NUMPAD9, "Numpad 9"},
	{ VK_MULTIPLY, "Numpad *" },
	{ VK_ADD, "Numpad +" },
	{ VK_SEPARATOR, "Numpad ," },
	{ VK_SUBTRACT, "Numpad -" },
	{ VK_DECIMAL, "Numpad ." },
	{ VK_DIVIDE, "Numpad /" },
	{ VK_F1, "F1" },
	{ VK_F2, "F2" },
	{ VK_F3, "F3" },
	{ VK_F4, "F4" },
	{ VK_F5, "F5" },
	{ VK_F6, "F6" },
	{ VK_F7, "F7" },
	{ VK_F8, "F8" },
	{ VK_F9, "F9" },
	{ VK_F10, "F10" },
	{ VK_F11, "F11" },
	{ VK_F12, "F12" },
	{ VK_F13, "F13" },
	{ VK_F14, "F14" },
	{ VK_F15, "F15" },
	{ VK_F16, "F16" },
	{ VK_F17, "F17" },
	{ VK_F18, "F18" },
	{ VK_F19, "F19" },
	{ VK_F20, "F20" },
	{ VK_F21, "F21" },
	{ VK_F22, "F22" },
	{ VK_F23, "F23" },
	{ VK_F24, "F24" },
	{ VK_NUMLOCK, "Num Lock" },
	{ VK_SCROLL, "Scroll Lock" },
	{ VK_LSHIFT, "Left Shift" },
	{ VK_RSHIFT, "Right Shift" },
	{ VK_LCONTROL, "Left Ctrl" },
	{ VK_RCONTROL, "Right Ctrl" },
	{ VK_LMENU, "Left Alt" },
	{ VK_RMENU, "Right Alt" },
	{ VK_BROWSER_BACK, "Browser Back" },
	{ VK_BROWSER_FORWARD, "Browser Forward" },
	{ VK_BROWSER_REFRESH, "Browser Refresh" },
	{ VK_BROWSER_STOP, "Browser Stop" },
	{ VK_BROWSER_SEARCH, "Browser Search" },
	{ VK_BROWSER_FAVORITES, "Browser Favorites" },
	{ VK_BROWSER_HOME, "Browser Home"},
	{ VK_VOLUME_MUTE, "Volume Mute" },
	{ VK_VOLUME_DOWN, "Volume Down" },
	{ VK_VOLUME_UP, "Volume Up" },
	{ VK_MEDIA_NEXT_TRACK, "Next Track" },
	{ VK_MEDIA_PREV_TRACK, "Previous Track" },
	{ VK_MEDIA_STOP, "Media Stop" },
	{ VK_MEDIA_PLAY_PAUSE, "Play/Pause" },
	{ VK_LAUNCH_MAIL, "Launch Mail" },
	{ VK_LAUNCH_MEDIA_SELECT, "Launch Media Select" },
	{ VK_LAUNCH_APP1, "Launch App 1" },
	{ VK_LAUNCH_APP2, "Launch App 2" },
	{ VK_OEM_1, ";" },
	{ VK_OEM_PLUS, "+" },
	{ VK_OEM_COMMA, "," },
	{ VK_OEM_MINUS, "-" },
	{ VK_OEM_PERIOD, "." },
	{ VK_OEM_2, "/" },
	{ VK_OEM_3, "~" },
	{ VK_OEM_4, "[" },
	{ VK_OEM_5, "\\" },
	{ VK_OEM_6, "]" },
	{ VK_OEM_7, "'" },
	{ VK_OEM_8, "OEM 8" },
	{ VK_OEM_102, "OEM 102" },
	{ VK_PROCESSKEY, "Process" },
	{ VK_PACKET, "Packet" },
	{ VK_ATTN, "Attn" },
	{ VK_CRSEL, "CrSel" },
	{ VK_EXSEL, "ExSel" },
	{ VK_EREOF, "Erase EOF" },
	{ VK_PLAY, "Play" },
	{ VK_ZOOM, "Zoom" },
	{ VK_NONAME, "NoName" },
	{ VK_PA1, "PA1" },
	{ VK_OEM_CLEAR, "OEM Clear" },
	{ VK_OEM_AX, "OEM AX" },
	{ VK_OEM_ENLW, "OEM ENLW" },
	{ VK_OEM_RESET, "OEM Reset" },
	{ VK_OEM_JUMP, "OEM Jump" },
	{ VK_OEM_PA1, "OEM PA1" },
	{ VK_OEM_PA2, "OEM PA2" },
	{ VK_OEM_PA3, "OEM PA3" },
	{ VK_OEM_WSCTRL, "OEM WSCTRL" },
	{ VK_OEM_CUSEL, "OEM CUSEL" },
	{ VK_OEM_ATTN, "OEM ATTN" },
	{ VK_OEM_FINISH, "OEM Finish" },
	{ VK_OEM_COPY, "OEM Copy" },
	{ VK_OEM_AUTO, "OEM Auto" },
	{ VK_OEM_BACKTAB, "OEM Back Tab" },
	{ VK_PROCESSKEY, "Process" },
	{ VK_ATTN, "Attn" },
	{ VK_CRSEL, "CrSel" },
	{ VK_EXSEL, "ExSel" },
	{ VK_EREOF, "Erase EOF" },
	{ VK_PLAY, "Play" },
	{ VK_ZOOM, "Zoom" },
	{ VK_NONAME, "NoName" },
	{ VK_PA1, "PA1" },
	{ VK_OEM_CLEAR, "OEM Clear" },
	{ VK_OEM_AX, "OEM AX" },
	{ VK_OEM_ENLW, "OEM ENLW" },
	{ VK_OEM_RESET, "OEM Reset" },
	{ VK_OEM_JUMP, "OEM Jump" },
	{ VK_OEM_PA1, "OEM PA1" },
	{ VK_OEM_PA2, "OEM PA2" },
	{ VK_OEM_PA3, "OEM PA3" },
	{ VK_OEM_WSCTRL, "OEM WSCTRL" },
	{ VK_OEM_CUSEL, "OEM CUSEL" },
	{ VK_OEM_ATTN, "OEM ATTN" },
	{ VK_OEM_FINISH, "OEM Finish" },
	{ VK_OEM_COPY, "OEM Copy"},

	// Non VK defined keys (eg. number keys)
	{ 0x30, "0" },
	{ 0x31, "1" },
	{ 0x32, "2" },
	{ 0x33, "3" },
	{ 0x34, "4" },
	{ 0x35, "5" },
	{ 0x36, "6" },
	{ 0x37, "7" },
	{ 0x38, "8" },
	{ 0x39, "9" },
	{ 0x41, "A" },
	{ 0x42, "B" },
	{ 0x43, "C" },
	{ 0x44, "D" },
	{ 0x45, "E" },
	{ 0x46, "F" },
	{ 0x47, "G" },
	{ 0x48, "H" },
	{ 0x49, "I" },
	{ 0x4A, "J" },
	{ 0x4B, "K" },
	{ 0x4C, "L" },
	{ 0x4D, "M" },
	{ 0x4E, "N" },
	{ 0x4F, "O" },
	{ 0x50, "P" },
	{ 0x51, "Q" },
	{ 0x52, "R" },
	{ 0x53, "S" },
	{ 0x54, "T" },
	{ 0x55, "U" },
	{ 0x56, "V" },
	{ 0x57, "W" },
	{ 0x58, "X" },
	{ 0x59, "Y" },
	{ 0x5A, "Z" }
};

std::string Utils::GetKeyNameTextWrapper(uint16_t scanCode)
{
	std::string keyName = keyNames[scanCode];

	if (keyName.empty())
		return "Unknown";

	return keyName;
}
