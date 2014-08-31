#pragma once

#include <shared/shared.h>
#include <dxgi.h>

namespace Pimp 
{

	class Configuration : 
		public Singleton<Configuration>
	{
	public:

		struct DisplayMode
		{
			int width;
			int height;
		};

		struct Adapter
		{
			Adapter() { name[0]=0; }

			char name[256];
			IDXGIAdapter1* adapterPtr;
		};

		struct DisplayAspectRatio
		{
			float aspect; // w/h
			char name[256];
		};


	private:
		FixedSizeList<Adapter> adapters;
		FixedSizeList<DisplayMode> displayModes;
		int selectedAdapter;
		int selectedDisplayMode;
		int selectedDisplayAspectRatio;

		bool fullscreen;

#if !PIMPPLAYER_USECONFIGDIALOG
		bool forceResolutionAndAspect;
		DisplayMode forcedDisplayMode;
		DisplayAspectRatio forcedAspectRatio; // w/h
#endif

		Configuration();

		friend class Singleton<Configuration>;

	public:
		void SetSelectedAdapter(int index)
		{
			selectedAdapter = index;
		}

		void SetSelectedGraphicsMode(int mode)
		{
			selectedDisplayMode = mode;
		}

		void SetFullscreen(bool full)
		{
			fullscreen = full;
		}

		bool GetFullscreen() const
		{
			return fullscreen;
		}

		int GetSelectedAdapterIndex() const
		{
			return selectedAdapter;
		}

		IDXGIAdapter1* GetSelectedAdapter() const
		{
			return adapters[selectedAdapter].adapterPtr;
		}

		int GetSelectedDisplayModeIndex() const
		{
			return selectedDisplayMode;
		}

		const FixedSizeList<Adapter>& GetAdapters() const 
		{
			return adapters;
		}

		const FixedSizeList<DisplayMode>& GetDisplayModes() const 
		{
			return displayModes;
		}

		const DisplayMode& GetSelectedDisplayMode() 
		{
#if !PIMPPLAYER_USECONFIGDIALOG
			if (forceResolutionAndAspect)
				return forcedDisplayMode;
			else
#endif
				return displayModes[selectedDisplayMode];
		}

		const DisplayAspectRatio* GetDisplayAspectRatios() const;
		int GetNumDisplayAspectRatios() const;

		int GetSelectedDisplayAspectRatioIndex() const 
		{
			return selectedDisplayAspectRatio;
		}

		float GetSelectedDisplayAspectRatio() const;


		void SetSelectedDisplayAspectRatio(int index)
		{
			selectedDisplayAspectRatio = index;
		}

#if !PIMPPLAYER_USECONFIGDIALOG
		void SetForcedResolutionAndAspect(int width, int height, float aspect)
		{
			forceResolutionAndAspect = true;
			forcedAspectRatio.aspect = aspect;
			forcedDisplayMode.width = width;
			forcedDisplayMode.height = height;
		}
#endif

		const DisplayAspectRatio& GetMaxDisplayAspectRatio() const;
	};
}
