
#pragma once

namespace Demo
{
	// @plek: Hacky boolean to indicate 1 extra step (disk I/O) for the loading bar.
	extern bool g_diskResourcesLoaded;

	bool GenerateWorld(Pimp::World** outWorld);
	void ReleaseWorld();
	void Tick();
}
