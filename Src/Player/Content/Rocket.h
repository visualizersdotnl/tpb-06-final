
//
// Rocket stuff.
//
// Rocket can run in edit mode (requires the client (sync_player.exe or OSX app.) to be connected) or
// it can run from exported data in replay mode.
//
// You can specify a hostname/IP address on the command line in all non-Release Player builds,
// for fancy remote editing.
//
// Right now, Player has 3 different build configurations:
// -   Debug: debug build that runs Rocket in edit mode.
// -  Design: release build that runs Rocket in edit mode.
// - Release: release build that runs Rocket in replay mode (globally defines SYNC_PLAYER).
//

static sync_device *s_Rocket = nullptr;

double kRocketRowRate = (PIMPPLAYER_ROCKET_BPM/60.0) * PIMPPLAYER_ROCKET_RPB;

double Rocket_GetRow()
{
	return Audio_GetPosition()*kRocketRowRate;
}

#if !defined (SYNC_PLAYER)

// Rocket uses sockets.
#pragma comment(lib, "ws2_32.lib")

void Rocket_Pause(void *, int bPause)
{
	if (0 == bPause)
		Audio_Unpause();
	else
		Audio_Pause();
}

void Rocket_SetRow(void *, int row)
{
	const double secPos = row / kRocketRowRate;
	Audio_SetPosition((float) secPos);
}

int Rocket_IsPlaying(void *)
{
	return Audio_IsPlaying();
}

// Audio player hooks.
static sync_cb s_rocketHooks = 
{ 
	Rocket_Pause, 
	Rocket_SetRow, 
	Rocket_IsPlaying 
};

#endif

// Important to know: this container owns none of it's pointers.
class SyncTrack
{
public:
	SyncTrack(const std::string &name, bool uploadForShaders, const sync_track **ppRef = nullptr) :
		m_track(sync_get_track(s_Rocket, name.c_str())),
		m_name(name) 
	{
		ASSERT(nullptr != m_track);

		if (true == uploadForShaders)
			matParam = CreateDynShaderParam(name);
		else
			matParam = nullptr;

		if (nullptr != ppRef)
			*ppRef = m_track;
	}

	float Get(double row) const 
	{ 
		return (float) sync_get_val(m_track, row); 
	}

	// Call each frame!
	void Update(double row)
	{
		if (nullptr != matParam)
			matParam->SetValue(Get(row));
	}

	const sync_track *m_track;
	const std::string m_name;
	Pimp::MaterialParameter *matParam;
};

static std::vector<SyncTrack> s_syncTracks;
