
#pragma once

namespace Pimp 
{
	class Configuration
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
		// The actual aspect ratio (for letterboxing) will be derived from displayMode in D3D.cpp.
		float renderAspectRatio; 

	public:
		Configuration();

		void SetFullscreen(bool full) { fullscreen = full; }
		bool GetFullscreen() const    { return fullscreen; }

		void SetDisplayMode(const DisplayMode &mode) { displayMode = mode; }
		const DisplayMode& GetDisplayMode()          { return displayMode; }

		void SetRenderAspectRatio(float ratio) { renderAspectRatio = ratio; }
		float GetRenderAspectRatio() const     { return renderAspectRatio;  }
	};

	extern Configuration gCoreCfg;
}
