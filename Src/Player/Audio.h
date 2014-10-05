
#pragma once

// #include <Windows.h>
// #include <string>

// 'iDevice' - valid device index or -1 for system default
bool Audio_Create(unsigned int iDevice, HWND hWnd, const std::string &mp3Path, bool mute); 
void Audio_Destroy();
void Audio_Start();
