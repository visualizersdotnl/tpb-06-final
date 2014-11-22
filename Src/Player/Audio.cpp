
#include <Core/Platform.h>
#include <Shared/assert.h>
#include <bass24/c/bass.h>
#include "Audio.h"
#include "Content/Demo.h" // FIXME: for TPB-06 sample loads only

static HSTREAM s_hMP3[3] = { 0 };
static HFX s_hFX = 0;

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

	// hack: load MP3 as well as gun shot & shell drop
	std::vector<const std::string> MP3s;
	MP3s.push_back(mp3Path);
	MP3s.push_back(Demo::GetAssetsPath() + "shot.mp3");
	MP3s.push_back(Demo::GetAssetsPath() + "shelldrop.mp3");

	int iMP3 = 0;
	for (const std::string &path : MP3s)
	{
		DWORD streamFlags = BASS_SAMPLE_FX | BASS_MP3_SETPOS | BASS_STREAM_PRESCAN;
		if (0 != iMP3) streamFlags |= (0 == iDevice) ? BASS_STREAM_DECODE : 0; // Decode disables FX.
//		const DWORD streamFlags = BASS_SAMPLE_FX | BASS_MP3_SETPOS | BASS_STREAM_PRESCAN | ( (0 == iDevice) ? BASS_STREAM_DECODE : 0 );
		HSTREAM hMP3 = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, streamFlags /* BASS_UNICODE */);
		if (hMP3 == NULL)
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
				SetLastError("Can not load MP3: " + path);
				return false;
			}
		}

		s_hMP3[iMP3++] = hMP3;
	}

	s_hFX = BASS_ChannelSetFX(s_hMP3[0], BASS_FX_DX8_FLANGER, 1);
	if (0 == s_hFX)
	{
		SetLastError("Can't enable DirectX 8 sound effect on our MP3 through BASS.");
		return false;
	}

	Audio_FlangerMP3(0.25f, 6.f);

#if defined(_DEBUG) || defined(_DESIGN)
	// Looping soundtrack is convenient when editing.
	BASS_ChannelFlags(s_hMP3[0], BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
#endif

	if (true == mute)
	{
		for (int iMP3 = 0; iMP3 < 3; ++iMP3)
			BASS_ChannelSetAttribute(s_hMP3[iMP3], BASS_ATTRIB_VOL, 0.f);
	}

	return true;
}

void Audio_Destroy()
{
	BASS_Free();
}

void Audio_Update() 
{ 
	ASSERT(s_hMP3[0] != NULL);
	BASS_Update(0); 
}

void Audio_Start()
{
	ASSERT(s_hMP3[0] != NULL);
	BASS_ChannelPlay(s_hMP3[0], TRUE);
}

void Audio_Gunshot() { BASS_ChannelPlay(s_hMP3[1], TRUE); }
void Audio_Shelldrop() { BASS_ChannelPlay(s_hMP3[2], TRUE); }

void Audio_Pause()
{
	ASSERT(s_hMP3[0] != NULL);
	BASS_ChannelPause(s_hMP3[0]);
}

void Audio_Unpause()
{
	ASSERT(s_hMP3[0] != NULL);
 	BASS_ChannelPlay(s_hMP3[0], FALSE);
}

bool Audio_IsPlaying()
{
	ASSERT(s_hMP3[0] != NULL);
	return BASS_ChannelIsActive(s_hMP3[0]) == BASS_ACTIVE_PLAYING;
}

void Audio_SetPosition(float secPos)
{
	ASSERT(s_hMP3[0] != NULL);
	const QWORD newChanPos = BASS_ChannelSeconds2Bytes(s_hMP3[0], secPos);
	BASS_ChannelSetPosition(s_hMP3[0], newChanPos, BASS_POS_BYTE);
}

float Audio_GetPosition()
{
	ASSERT(s_hMP3[0] != NULL);
	const QWORD chanPos = BASS_ChannelGetPosition(s_hMP3[0], BASS_POS_BYTE);
	const double secPos = BASS_ChannelBytes2Seconds(s_hMP3[0], chanPos);
	return (float) secPos;
}

void Audio_FlangerMP3(float wetDry, float freqMod)
{
	BASS_DX8_FLANGER fxParams;
	fxParams.fWetDryMix = wetDry; // 0 = silent, 100 = wet
	fxParams.fDepth = 100.f;
	fxParams.fFeedback = -50.f;
	fxParams.fFrequency = freqMod; // upwards of 6 is cool
	fxParams.lWaveform = 1; // Sine
	fxParams.fDelay = 2.f;
	fxParams.lPhase = BASS_DX8_PHASE_ZERO;

	if (FALSE == BASS_FXSetParameters(s_hFX, &fxParams))
	{
		int errCode = BASS_ErrorGetCode();
		ASSERT(0);
	}
}