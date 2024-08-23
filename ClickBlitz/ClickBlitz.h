#pragma once

#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
                                              name                  = 'Microsoft.Windows.Common-Controls' \
                                              version               = '6.0.0.0' \
                                              processorArchitecture = '*' \
                                              publicKeyToken        = '6595b64144ccf1df' \
                                              language              = '*'\"")

#include "resource.h"

// Public string constants
#define APP_NAME L"ClickBlitz"
#define APP_VERSION L"1.1.0"
#define APP_AUTHOR L"David Jonjic"

void EnableGUIInputs(bool isEnabled);
