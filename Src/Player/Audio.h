
#pragma once

// #include <Windows.h>
// #include <string>

// 'iDevice' - valid device index or -1 for system default
bool Audio_Create(unsigned int iDevice, HWND hWnd, const std::string &mp3Path, bool mute); 
void Audio_Destroy();
void Audio_Update();

void Audio_Start();
void Audio_Pause();
void Audio_Unpause();
bool Audio_IsPlaying();

void Audio_SetPosition(float secPos);
float Audio_GetPosition();

// for Bond intro
void Audio_Gunshot();
void Audio_Shelldrop();

// for end
void Audio_FlangerMP3(float wetDry, float freqMod); // freqMod 0.25 is normal, 6 to 8 is fucked up
