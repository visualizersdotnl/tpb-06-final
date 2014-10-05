
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
	if (!BASS_Init((iDevice == -1) ? -1 : iDevice + 1, 44100, BASS_DEVICE_LATENCY, hWnd, NULL))
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

	s_hMP3 = BASS_StreamCreateFile(FALSE, mp3Path.c_str(), 0, 0, 0 /* BASS_UNICODE */);
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

	if (true == mute)
		BASS_ChannelSetAttribute(s_hMP3, BASS_ATTRIB_VOL, 0.f);

	return true;
}

void Audio_Destroy()
{
	BASS_Free();
}

void Audio_Start()
{
	ASSERT(s_hMP3 != NULL);
	BASS_ChannelPlay(s_hMP3, FALSE);
}

static float Audio_GetPos()
{
	ASSERT(s_hMP3 != NULL);
	const QWORD chanPos = BASS_ChannelGetPosition(s_hMP3, BASS_POS_BYTE);
	const float secPos = (float) BASS_ChannelBytes2Seconds(s_hMP3, chanPos);
	return secPos;
}
