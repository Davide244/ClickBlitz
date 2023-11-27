// ClickBlitz.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ClickBlitz.h"
#include "AutoClicker.hpp"
#include <string>

#include <CommCtrl.h>
#include "Utils.hpp"

#define MAINWINDOWCLASSNAME L"MainWindow"

// UI Elements IDs
#define IDUI_STARTBTN   0x8e01
#define IDUI_STOPBTN    0x8e02
#define IDUI_FREQ_INT_SELECTOR 0x8e03
#define IDUI_CLICKSPEED_TEXT 0x8e04
#define IDUI_CLICKSPEEDFORMAT_STATIC 0x8e05
#define IDUI_STOPAT_TEXT 0x8e06
#define IDUI_STARTKEYBIND_BUTTON 0x8e07
#define IDUI_STARTKEYBINDTIMER 0x8e08
#define IDUI_START_TOGGLE_HOLD_SELECTOR 0x8e09
#define IDUI_STOPAUTOCLICKERKEYTIMER 0x8e0a
#define IDUI_CAPTUREMOUSEKEYBIND_TIMER 0x8e0b

#define IDUI_WHATTOCLICKRADIO 0x8e10

// Globals
HINSTANCE hInst;                                // current instance
void CreateUIElements(HWND hwnd);
void CheckForOtherInstances();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcGroupbox(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK WndProcGroupbox_WhatToClick(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
AutoClicker AutoClickerInstance;

HWND MainWindow;

HWND ClickOptionsGroupBox;
HWND WhatToClickGroupBox;

HWND RadioButtonFrequency;
HWND RadioButtonInterval;
HWND RadioButtonStartToggle;
HWND RadioButtonStartHold;

HWND RadioButtonMouseLeft;
HWND RadioButtonMouseMiddle;
HWND RadioButtonMouseRight;

// Global Logic Variables
bool IsCPSSelected = true;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CheckForOtherInstances();

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    hInst = hInstance;

    WNDCLASSEX WindowClass;
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = LoadIcon(WindowClass.hInstance, MAKEINTRESOURCE(IDI_CLICKBLITZ));
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = MAINWINDOWCLASSNAME;
    WindowClass.hIconSm = LoadIcon(WindowClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassEx(&WindowClass);

    MainWindow = CreateWindowEx(WS_EX_TOPMOST, MAINWINDOWCLASSNAME, APP_NAME, WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 225, 325, NULL, NULL, hInstance, NULL);
    if (!MainWindow)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Fatal Error", MB_ICONERROR | MB_OK);
		return 1;
	}

    CreateUIElements(MainWindow);

    ShowWindow(MainWindow, nCmdShow);
    UpdateWindow(MainWindow);

    MSG Message;
    while (GetMessage(&Message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

static HBRUSH StaticBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
static int ClicksPerSecond = 150;
static int ClickIntervall = 1;
static int StopAtClicks = 0;
static bool Mouse = true; // If true, the mouse will be clicked, if false, the keyboard will be clicked
static int KeyToPress = 0; // The key that will be pressed if Mouse is false
static int StartKey = 0; // The key that will start the autoclicker
static int PrevStartKey;

static bool IsStartToggle = true; // If true, the autoclicker will toggle on and off, if false, the autoclicker will hold down the activation key

MouseButton SelectedMouseButton = MouseButton::Left;

#define CAPTUREMOUSECOOLDOWN_MAX 50
char CaptureMouseCooldown = CAPTUREMOUSECOOLDOWN_MAX; // The amount of cycles of WM_TIMER that have to pass before the mouse can be captured again
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDUI_STARTBTN:
        {
            StopAtClicks = GetDlgItemInt(ClickOptionsGroupBox, IDUI_STOPAT_TEXT, NULL, false);

            switch (IsCPSSelected)
            {
            case true:
                ClicksPerSecond = GetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, NULL, false);
                AutoClickerInstance.AutoClick_Rate(ClicksPerSecond, StopAtClicks, SelectedMouseButton);
                break;
            case false:
                ClickIntervall = GetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, NULL, false);
                AutoClickerInstance.AutoClick_Intervall(ClickIntervall, StopAtClicks, SelectedMouseButton);
                break;
            }
            break;
        }
        case IDUI_STOPBTN:
        {
            AutoClickerInstance.AutoClick_Stop();
            EnableWindow(ClickOptionsGroupBox, true);
            break;
        }
        }
        break;
    }
    case WM_HOTKEY:
    {
        OutputDebugStringA("Confirming Hotkey...\n");
        if (wParam == 1000 && HIWORD(lParam) == StartKey && StartKey != 0) 
        {
            OutputDebugStringA("Hotkey pressed!\n");

            StopAtClicks = GetDlgItemInt(ClickOptionsGroupBox, IDUI_STOPAT_TEXT, NULL, false);

            if (AutoClicker::AutoClickerActive.load() == true && IsStartToggle == true)
            {
				AutoClickerInstance.AutoClick_Stop();
				break;
			}

            switch (IsCPSSelected)
            {
            case true:
                ClicksPerSecond = GetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, NULL, false);
                AutoClickerInstance.AutoClick_Rate(ClicksPerSecond, StopAtClicks, SelectedMouseButton);
                break;
            case false:
                ClickIntervall = GetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, NULL, false);
                AutoClickerInstance.AutoClick_Intervall(ClickIntervall, StopAtClicks, SelectedMouseButton);
                break;
            }

            if (IsStartToggle == false) 
                SetTimer(hWnd, IDUI_STOPAUTOCLICKERKEYTIMER, 1, NULL);

            break;
        }
    }
    case WM_TIMER:
    {
        switch (wParam) 
        {
        case IDUI_STOPAUTOCLICKERKEYTIMER:
        {
            if (GetAsyncKeyState(StartKey) & 0x8000)
            {
                AutoClickerInstance.AutoClick_Stop();
                KillTimer(hWnd, IDUI_STOPAUTOCLICKERKEYTIMER);
            }
            break;
        }
        case IDUI_CAPTUREMOUSEKEYBIND_TIMER:
        {
            CaptureMouseCooldown--;
            if (StartKey == VK_XBUTTON1 || StartKey == VK_XBUTTON2 && GetAsyncKeyState(StartKey) & 0x8000 && CaptureMouseCooldown <= 0)
            {
                CaptureMouseCooldown = CAPTUREMOUSECOOLDOWN_MAX;
                OutputDebugStringA("Mouse hotkey pressed!\n");
                SendMessage(hWnd, WM_HOTKEY, 1000, MAKELPARAM(0, StartKey));
			}
            break;
        }
        }
        break;
    }
    case WM_DESTROY:

        UnregisterHotKey(MainWindow, 1000);

        PostQuitMessage(0);
        break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkColor(hdcStatic, RGB(255, 255, 255));
        return (INT_PTR)StaticBackgroundBrush;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


bool WaitingForKey = false;
LRESULT CALLBACK WndProcGroupbox(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_COMMAND: 
    {
        switch (LOWORD(wParam))
        {
		case IDUI_FREQ_INT_SELECTOR:
        {
            switch (HIWORD(wParam))
            {
			case BN_CLICKED:
            {
                switch (IsCPSSelected)
                {
				case true:
                {
                    // check if the radio button is checked
                    if (SendMessage(RadioButtonFrequency, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        break;

					IsCPSSelected = false;
					SetWindowText(GetDlgItem(ClickOptionsGroupBox, IDUI_CLICKSPEEDFORMAT_STATIC), L"ms");
                    SetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, ClickIntervall, false);
					break;
				}
				case false:
                {
                    if (SendMessage(RadioButtonInterval, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        break;
					IsCPSSelected = true;
                    SetWindowText(GetDlgItem(ClickOptionsGroupBox, IDUI_CLICKSPEEDFORMAT_STATIC), L"Clicks/s");
                    SetDlgItemInt(ClickOptionsGroupBox, IDUI_CLICKSPEED_TEXT, ClicksPerSecond, false);
					break;
				}
				}
				break;
			}
			}
			break;
		}
        case IDUI_START_TOGGLE_HOLD_SELECTOR:
        {
            switch (HIWORD(wParam))
            {
			case BN_CLICKED:
            {
                switch (IsStartToggle)
                {
				case true:
                {
					// check if the radio button is checked
					if (SendMessage(RadioButtonStartToggle, BM_GETCHECK, 0, 0) == BST_CHECKED)
						break;

					IsStartToggle = false;
					break;
				}
				case false:
                {
					if (SendMessage(RadioButtonStartHold, BM_GETCHECK, 0, 0) == BST_CHECKED)
						break;
					IsStartToggle = true;
					break;
				}
				}
				break;
			}
			}
			break;
        }
        case IDUI_STARTKEYBIND_BUTTON:
            SetWindowText(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), L"Press any key...");
            EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTBTN), false);
            EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), false);

            PrevStartKey = StartKey;
            int i = 0;

            for (int i = 0; i < 255; i++)
            {
                if ((1 << 15) & GetAsyncKeyState(i)&0x8000)
                {
                    // ignore mouse buttons
                    if (i == VK_LBUTTON || i == VK_MBUTTON || i == VK_RBUTTON)
                        continue;
                    StartKey = i;
                    break;
                }
            }

            if (StartKey == 0 || StartKey == PrevStartKey) 
            {
                WaitingForKey = true;
                SetTimer(hWnd, IDUI_STARTKEYBINDTIMER, 1, NULL);
                break;
            }

            KillTimer(MainWindow, IDUI_CAPTUREMOUSEKEYBIND_TIMER);

            UnregisterHotKey(MainWindow, 1000);
            if (RegisterHotKey(MainWindow, 1000, MOD_NOREPEAT, StartKey) != ERROR_SUCCESS) {
                MessageBox(NULL, L"Failed to register hotkey!", L"Error", MB_ICONERROR | MB_OK);
				break;
            }

            if (StartKey == VK_XBUTTON1 || StartKey == VK_XBUTTON2)
                SetTimer(MainWindow, IDUI_CAPTUREMOUSEKEYBIND_TIMER, 1, NULL);

            std::string KeyString = Utils::GetKeyNameTextWrapper(StartKey);

            SetWindowTextA(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), KeyString.c_str());
            EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTBTN), true);
            EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), true);
            break;
		}
		break;  
    }
    case WM_TIMER:
    {
        if (WaitingForKey == true)
        {
            for (int i = 0; i < 255; i++)
            {
                if ((1 << 15) & GetAsyncKeyState(i) & 0x8000)
                {
                    // ignore mouse buttons
                    if (i == VK_LBUTTON || i == VK_MBUTTON || i == VK_RBUTTON)
                        continue;
                    StartKey = i;
                    break;
                }
            }

            if (StartKey != 0 && StartKey != PrevStartKey)
            {
                KillTimer(hWnd, IDUI_STARTKEYBINDTIMER);
                WaitingForKey = false;

                KillTimer(MainWindow, IDUI_CAPTUREMOUSEKEYBIND_TIMER);

                UnregisterHotKey(MainWindow, 1000);
                RegisterHotKey(MainWindow, 1000, MOD_NOREPEAT, StartKey);

                if (StartKey == VK_XBUTTON1 || StartKey == VK_XBUTTON2)
                    SetTimer(MainWindow, IDUI_CAPTUREMOUSEKEYBIND_TIMER, 1, NULL);

                std::string KeyString = Utils::GetKeyNameTextWrapper(StartKey);
                SetWindowTextA(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), KeyString.c_str());
                EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTBTN), true);
                EnableWindow(GetDlgItem(ClickOptionsGroupBox, IDUI_STARTKEYBIND_BUTTON), true);
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkColor(hdcStatic, RGB(255, 255, 255));
        return (INT_PTR)StaticBackgroundBrush;
    }
    default:
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProcGroupbox_WhatToClick(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            case IDUI_WHATTOCLICKRADIO:
            {
                if (SendMessage(RadioButtonMouseLeft, BM_GETCHECK, 0, 0) == BST_CHECKED)
                    SelectedMouseButton = MouseButton::Left;

                if (SendMessage(RadioButtonMouseMiddle, BM_GETCHECK, 0, 0) == BST_CHECKED)
                    SelectedMouseButton = MouseButton::Middle;

                if (SendMessage(RadioButtonMouseRight, BM_GETCHECK, 0, 0) == BST_CHECKED)
                    SelectedMouseButton = MouseButton::Right;

                break;
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkColor(hdcStatic, RGB(255, 255, 255));
        return (INT_PTR)StaticBackgroundBrush;
    }
    default:
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CreateUIElements(HWND hwnd) 
{
    ClickOptionsGroupBox = CreateWindowEx(NULL, L"BUTTON", L"Clicks && Triggers", WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_TEXT, 10, 10, 190, 140, hwnd, NULL, hInst, NULL);
    SetWindowSubclass(ClickOptionsGroupBox, WndProcGroupbox, 0, 0);

    #pragma region Click speed control
    // Up/Down click speed control with "Frequency" text nex to it in a static control
    /*HWND ClickSpeedUpDownBuddy = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"100", WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | ES_NUMBER, 80, 30, 70, 20, hwnd, NULL, hInst, NULL);
    HWND ClickSpeedUpDown = CreateWindowEx(NULL, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT | HDS_HOTTRACK, 0, 0, 0, 0, ClickOptionsGroupBox, NULL, hInst, NULL);*/
    HWND ClickSpeedUpDownBuddy = CreateWindowEx(
        WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP,
        WC_EDIT,
        L"100",
        WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER | 
        ES_NUMBER | ES_LEFT,
        70, 18,
        70, 20,
        ClickOptionsGroupBox,
        (HMENU)IDUI_CLICKSPEED_TEXT,
        hInst,
        NULL);
    HWND ClickSpeedUpDown = CreateWindowEx(
        WS_EX_LEFT | WS_EX_LTRREADING,
        UPDOWN_CLASS,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE | 
        UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_HOTTRACK,
        0, 0,
        0, 0,
        ClickOptionsGroupBox,
        NULL,
        hInst,
        NULL);
    HWND ClickSpeedText = CreateWindowEx(NULL, WC_STATIC, L"Frequency:", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 10, 18, 60, 20, ClickOptionsGroupBox, NULL, hInst, NULL);
    HWND ClickSpeedFormat = CreateWindowEx(NULL, WC_STATIC, L"Clicks/s", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 142, 18, 40, 20, ClickOptionsGroupBox, (HMENU)IDUI_CLICKSPEEDFORMAT_STATIC, hInst, NULL);
    SendMessage(ClickSpeedUpDown, UDM_SETRANGE, 0, MAKELPARAM(1500, 1));
    SendMessage(ClickSpeedUpDown, UDM_SETPOS, NULL, ClicksPerSecond);

    // Tooltip for click speed control
    HWND ClickSpeedTooltip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hInst, NULL);
    TOOLINFO ClickSpeedTooltipInfo = { 0 };
    ClickSpeedTooltipInfo.cbSize = sizeof(ClickSpeedTooltipInfo);
    ClickSpeedTooltipInfo.hwnd = hwnd;
    ClickSpeedTooltipInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ClickSpeedTooltipInfo.uId = (UINT_PTR)ClickSpeedUpDownBuddy;
    ClickSpeedTooltipInfo.lpszText = const_cast<wchar_t*>(L"Note: Most Computers cannot produce more than 750 - 850 clicks a second!");
    SendMessage(ClickSpeedTooltip, TTM_ADDTOOL, 0, (LPARAM)&ClickSpeedTooltipInfo);

    RadioButtonFrequency = CreateWindowEx(NULL, L"BUTTON", L"Frequency", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 25, 40, 80, 20, ClickOptionsGroupBox, (HMENU)IDUI_FREQ_INT_SELECTOR, hInst, NULL);
    RadioButtonInterval = CreateWindowEx(NULL, L"BUTTON", L"Interval", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 105, 40, 80, 20, ClickOptionsGroupBox, (HMENU)IDUI_FREQ_INT_SELECTOR, hInst, NULL);
    SendMessage(RadioButtonFrequency, BM_SETCHECK, BST_CHECKED, NULL);

    HWND StopAtClicksText = CreateWindowEx(NULL, WC_STATIC, L"Stop at:", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 10, 70, 60, 20, ClickOptionsGroupBox, NULL, hInst, NULL);
    HWND StopAtClicksFormat = CreateWindowEx(NULL, WC_STATIC, L"Clicks", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 142, 70, 40, 20, ClickOptionsGroupBox, NULL, hInst, NULL);
    HWND StopAtClicksTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | ES_NUMBER, 70, 70, 70, 20, ClickOptionsGroupBox, (HMENU)IDUI_STOPAT_TEXT, hInst, NULL);

    HWND StartKeybindText = CreateWindowEx(NULL, WC_STATIC, L"Start keybind:", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 10, 95, 80, 20, ClickOptionsGroupBox, NULL, hInst, NULL);
    HWND StartKeybindHotkey = CreateWindowEx(NULL, WC_BUTTON, L"None", WS_VISIBLE | WS_CHILD | WS_OVERLAPPED, 90, 95, 90, 20, ClickOptionsGroupBox, (HMENU)IDUI_STARTKEYBIND_BUTTON, hInst, NULL);

    RadioButtonStartToggle = CreateWindowEx(NULL, L"BUTTON", L"Toggle", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 25, 115, 80, 20, ClickOptionsGroupBox, (HMENU)IDUI_START_TOGGLE_HOLD_SELECTOR, hInst, NULL);
    RadioButtonStartHold = CreateWindowEx(NULL, L"BUTTON", L"Hold", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_DISABLED, 105, 115, 80, 20, ClickOptionsGroupBox, (HMENU)IDUI_START_TOGGLE_HOLD_SELECTOR, hInst, NULL);
    SendMessage(RadioButtonStartToggle, BM_SETCHECK, BST_CHECKED, NULL);

    // Tooltip for Hold radio option (disabled) because it's not implemented yet
    HWND StartHoldTooltip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hInst, NULL);
    TOOLINFO StartHoldTooltipInfo = { 0 };
    StartHoldTooltipInfo.cbSize = sizeof(StartHoldTooltipInfo);
    StartHoldTooltipInfo.hwnd = hwnd;
    StartHoldTooltipInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    StartHoldTooltipInfo.uId = (UINT_PTR)RadioButtonStartHold;
    StartHoldTooltipInfo.lpszText = const_cast<wchar_t*>(L"Note: This feature is currently broken and was disabled");
    SendMessage(StartHoldTooltip, TTM_ADDTOOL, 0, (LPARAM)&StartHoldTooltipInfo);
    #pragma endregion

    HWND StartButton = CreateWindowEx(NULL, L"BUTTON", L"Start", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 155, 90, 30, hwnd, (HMENU)IDUI_STARTBTN, hInst, NULL);
    HWND StopButton = CreateWindowEx(NULL, L"BUTTON", L"Stop", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 110, 155, 90, 30, hwnd, (HMENU)IDUI_STOPBTN, hInst, NULL);

#pragma region What to click groupbox
    WhatToClickGroupBox = CreateWindowEx(NULL, L"BUTTON", L"What to click", WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_TEXT, 10, 190, 190, 85, hwnd, NULL, hInst, NULL);
    SetWindowSubclass(WhatToClickGroupBox, WndProcGroupbox_WhatToClick, 0, 0);

    RadioButtonMouseLeft = CreateWindowEx(NULL, L"BUTTON", L"Left mouse button", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 30, 15, 140, 20, WhatToClickGroupBox, (HMENU)IDUI_WHATTOCLICKRADIO, hInst, NULL);
    RadioButtonMouseMiddle = CreateWindowEx(NULL, L"BUTTON", L"Middle mouse button", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 30, 35, 140, 20, WhatToClickGroupBox, (HMENU)IDUI_WHATTOCLICKRADIO, hInst, NULL);
    RadioButtonMouseRight = CreateWindowEx(NULL, L"BUTTON", L"Right mouse button", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 30, 55, 140, 20, WhatToClickGroupBox, (HMENU)IDUI_WHATTOCLICKRADIO, hInst, NULL);
    SendMessage(RadioButtonMouseLeft, BM_SETCHECK, BST_CHECKED, NULL);
#pragma endregion

    // Initialize Segoe UI font
    HFONT SegoeUIFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));
    HFONT SegoeUIBoldFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI Bold"));


    // Apply Segoe UI font to all UI elements
    SendMessage(ClickOptionsGroupBox, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(ClickSpeedUpDown, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(ClickSpeedUpDownBuddy, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(ClickSpeedFormat, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonFrequency, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonInterval, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(StopAtClicksFormat, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(StopAtClicksTextbox, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(StartKeybindHotkey, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonStartToggle, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonStartHold, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(WhatToClickGroupBox, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonMouseLeft, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonMouseMiddle, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);
    SendMessage(RadioButtonMouseRight, WM_SETFONT, (WPARAM)SegoeUIFont, TRUE);

    SendMessage(ClickSpeedText, WM_SETFONT, (WPARAM)SegoeUIBoldFont, TRUE);
    SendMessage(StopAtClicksText, WM_SETFONT, (WPARAM)SegoeUIBoldFont, TRUE);
    SendMessage(StartKeybindText, WM_SETFONT, (WPARAM)SegoeUIBoldFont, TRUE);
    SendMessage(StartButton, WM_SETFONT, (WPARAM)SegoeUIBoldFont, TRUE);
    SendMessage(StopButton, WM_SETFONT, (WPARAM)SegoeUIBoldFont, TRUE);
}

void CheckForOtherInstances()
{
    if (FindWindow(NULL, APP_NAME) != NULL && MessageBox(NULL, L"Another instance of ClickBlittz is already running. Do you want to open a new instance anyways?", L"ClickBlitz", MB_ICONEXCLAMATION | MB_YESNO | MB_TOPMOST) == 7)
		ExitProcess(0);
}

void EnableGUIInputs(bool isEnabled) 
{
    EnableWindow(ClickOptionsGroupBox, isEnabled);
    EnableWindow(WhatToClickGroupBox, isEnabled);
    EnableWindow(GetDlgItem(MainWindow, IDUI_STARTBTN), isEnabled);
}
