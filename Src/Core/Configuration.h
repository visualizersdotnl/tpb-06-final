
#pragma once

#include <Shared/assert.h> // Used by <Shared/singleton.h>
#include <Shared/singleton.h>

namespace Pimp 
{
	// This class used to contain objects to aid a flexible configuration dialog.
	// The new Player stub has a rather different (and static) philosophy to setting up a display mode,
	// so all of this has been taken out. If in the future a dialog will be added, the code to maintain
	// a list of properties and selections will likely be maintained by the host or live alongside it.

	class Configuration : 
		public Singleton<Configuration>
	{
	public:
		struct DisplayMode
		{
			int width;
			int height;
		};

	private:
		bool fullscreen;
		DisplayMode displayMode;

		// This means: the aspect ratio the demo requires.
		// The actual aspect ratio (for letterboxing) will be derived from displayMode.
		float renderAspectRatio; 

		Configuration();

		friend class Singleton<Configuration>;

	public:
		void SetFullscreen(bool full) { fullscreen = full; }
		bool GetFullscreen() const    { return fullscreen; }

		void SetDisplayMode(const DisplayMode &mode) { displayMode = mode; }
		const DisplayMode& GetDisplayMode()          { return displayMode; }

		void SetRenderAspectRatio(float ratio) { renderAspectRatio = ratio; }
		float GetRenderAspectRatio() const     { return renderAspectRatio;  }
	};
}
