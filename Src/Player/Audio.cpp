
// #include <Windows.h>
#include <string>
#include <Shared/assert.h>
#include "../Libs/bass24/c/bass.h"
#include "Audio.h"
#include "SetLastError.h"

static HSTREAM s_hMP3 = 0;

bool Audio_Create(unsigned int iDevice, HWND hWnd, const std::string &mp3Path, bool mute)
{
	ASSERT(iDevice == -1); // || iDevice < Audio_GetDeviceCount());
	ASSERT(hWnd != NULL);

	// Bass device IDs:
	//  0 = No sound (causes functionality to be limited, so -1 is the better pick).
	// -1 = Default.
	// >0 = As enumerated.

//	DWORD devCount = 0; 
//	BASS_DEVICEINFO devInfo;
//	for (DWORD iDev = 0; BASS_GetDeviceInfo(iDev, &devInfo); ++iDev)
//		if (devInfo.flags & BASS_DEVICE_ENABLED)
//			++devCount;
//

	if (!BASS_Init(iDevice, 44100, BASS_DEVICE_LATENCY, hWnd, NULL))
	{ 
		switch (BASS_ErrorGetCode())
		{
		case BASS_ERROR_DEVICE:
		case BASS_ERROR_ALREADY:
		case BASS_ERROR_NO3D:
		case BASS_ERROR_UNKNOWN:
		case BASS_ERROR_MEM:
			ASSERT(0);

		case BASS_ERROR_DRIVER:
		case BASS_ERROR_FORMAT:
			SetLastError("Can not initialize BASS audio library @ 44.1 kHz.");
			return false;
		}
	}

	const DWORD streamFlags = BASS_MP3_SETPOS | BASS_STREAM_PRESCAN | ( (0 == iDevice) ? BASS_STREAM_DECODE : 0 );
	s_hMP3 = BASS_StreamCreateFile(FALSE, mp3Path.c_str(), 0, 0, streamFlags /* BASS_UNICODE */);
	if (s_hMP3 == NULL)
	{
		switch (BASS_ErrorGetCode())
		{
		case BASS_ERROR_INIT:
		case BASS_ERROR_NOTAVAIL:
		case BASS_ERROR_ILLPARAM:
		case BASS_ERROR_NO3D:
		case BASS_ERROR_FILEFORM:
		case BASS_ERROR_CODEC:
		case BASS_ERROR_FORMAT:
		case BASS_ERROR_SPEAKER:
		case BASS_ERROR_MEM:
			ASSERT(0);

		case BASS_ERROR_FILEOPEN:
		case BASS_ERROR_UNKNOWN:			
			SetLastError("Can not load MP3: " + mp3Path);
			return false;
		}
	}

	BASS_ChannelFlags(s_hMP3, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);

	if (true == mute)
		BASS_ChannelSetAttribute(s_hMP3, BASS_ATTRIB_VOL, 0.f);

	return true;
}

void Audio_Destroy()
{
	BASS_Free();
}

void Audio_Update() 
{ 
	ASSERT(s_hMP3 != NULL);
	BASS_Update(0); 
}

void Audio_Start()
{
	ASSERT(s_hMP3 != NULL);
	BASS_ChannelPlay(s_hMP3, TRUE);
}

void Audio_Pause()
{
	ASSERT(s_hMP3 != NULL);
	BASS_ChannelPause(s_hMP3);
}

void Audio_Unpause()
{
	ASSERT(s_hMP3 != NULL);
 	BASS_ChannelPlay(s_hMP3, FALSE);
}

bool Audio_IsPlaying()
{
	ASSERT(s_hMP3 != NULL);
	return BASS_ChannelIsActive(s_hMP3) == BASS_ACTIVE_PLAYING;
}

void Audio_SetPosition(float secPos)
{
	ASSERT(s_hMP3 != NULL);
	const QWORD newChanPos = BASS_ChannelSeconds2Bytes(s_hMP3, secPos);
	BASS_ChannelSetPosition(s_hMP3, newChanPos, BASS_POS_BYTE);
}

float Audio_GetPosition()
{
	ASSERT(s_hMP3 != NULL);
	const QWORD chanPos = BASS_ChannelGetPosition(s_hMP3, BASS_POS_BYTE);
	const double secPos = BASS_ChannelBytes2Seconds(s_hMP3, chanPos);
	return (float) secPos;
}
