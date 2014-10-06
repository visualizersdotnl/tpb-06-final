
#pragma once

// #include <Windows.h>
// #include <string>

// Library required for Rocket.
#pragma comment(lib, "ws2_32.lib")

// 'iDevice' - valid device index or -1 for system default
bool Audio_Create(unsigned int iDevice, HWND hWnd, const std::string &mp3Path, bool mute); 
void Audio_Destroy();
void Audio_Start();
void Audio_Update(); // Call it!, CALL IT!

// Hooks for Rocket.
// Bind them yourself using sync_set_callbacks() (done in Demo.cpp).
void Audio_Rocket_Pause(void *, int bPause);
void Audio_Rocket_SetRow(void *, int row);
int Audio_Rocket_IsPlaying(void *);

// For Rocket update.
double Audio_Rocket_GetRow();
