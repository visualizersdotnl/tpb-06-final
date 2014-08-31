#include "configuration.h"
#include <dxgi.h>
#include "d3d.h"
#include "Settings.h"

namespace Pimp
{
	Configuration::Configuration()
		: selectedAdapter(0), selectedDisplayMode(0), 
		fullscreen(false),
		adapters(32),
		displayModes(1024),
		selectedDisplayAspectRatio(3) // 16:9
#if !PIMPPLAYER_USECONFIGDIALOG
		, forceResolutionAndAspect(false)
#endif
	{
		IDXGIFactory1* factory = NULL;
		D3D_VERIFY( CreateDXGIFactory1(__uuidof(IDXGIFactory1) ,(void**)&factory), "Configuration::CreateDXGIFactory" );
		ASSERT(factory != NULL);

		int adapterIndex = 0;
		bool hasModes = false;

		IDXGIAdapter1* adapter;
		while (factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 adapterDesc;
			D3D_VERIFY(adapter->GetDesc1(&adapterDesc), "Configuration::GetDesc1");			

			Adapter newAdapter;
			newAdapter.adapterPtr = adapter;
			int result = WideCharToMultiByte(CP_ACP, 0, adapterDesc.Description, -1, newAdapter.name, sizeof(newAdapter.name), NULL, NULL);
			ASSERT(result > 0);	

			adapters.Add(newAdapter);

			if (!hasModes)
			{
				IDXGIOutput* output = NULL;
				HRESULT hr = adapter->EnumOutputs(0, &output);

				if (hr != DXGI_ERROR_NOT_FOUND)
				{
					D3D_VERIFY(hr, "Configuration::EnumOutputs(0)");
					ASSERT(output != NULL);

					UINT numModes = 0;
					DXGI_FORMAT format = PIMP_BACKBUFFER_FORMAT;

					D3D_VERIFY(output->GetDisplayModeList(format, 0, &numModes, NULL), "Configuration::GetDisplayModeList");

					if (numModes > 0)
					{
						DXGI_MODE_DESC* modes = NULL;
						modes = new DXGI_MODE_DESC[numModes];

						D3D_VERIFY(output->GetDisplayModeList(format, 0, &numModes, modes), "Configuration::GetDisplayModeList");

						for (int modeIndex=0; modeIndex<(int)numModes; ++modeIndex)
						{
							DisplayMode newMode;
							newMode.width = (int)modes[modeIndex].Width;
							newMode.height = (int)modes[modeIndex].Height;

							bool skipMode = false;

							for (int i=0; i<displayModes.Size(); ++i)
								if (displayModes[i].width == newMode.width &&
									displayModes[i].height == newMode.height)
								{
									skipMode = true;
									break;
								}

								if (!skipMode)
								{
									// Set 1280x720 as our default mode.
									if (adapterIndex == selectedAdapter && newMode.width == 1280 && newMode.height == 720)
										selectedDisplayMode = displayModes.Size();

									displayModes.Add(newMode);
								}
						}

						hasModes = true;

						delete [] modes;
					}
				}
			}
			
			++adapterIndex;
		}
		
		factory->Release();

		selectedDisplayAspectRatio = 3; // force 16:9

//
//#if !PIMPPLAYER_USECONFIGDIALOG
//
//		// under Remote Desktop we sometimes have no modes at all...
//
//		if (displayModes.Size() == 0)
//		{
//			DisplayMode m;
//			m.width = 1280;
//			m.height = 720;
//			m.refreshRate = 60;
//			displayModes.Add(m);
//
//			selectedDisplayMode = 0;
//		}
//
//#endif

	}

	// should stay sorted by aspect ratio
	static Configuration::DisplayAspectRatio configAspectRatios[] = {
		{ 5.0f/4.0f, "5:4" },
		{ 4.0f/3.0f, "4:3" },
		{ 16.0f/10.0f, "16:10" },
		{ 16.0f/9.0f, "16:9" },
	};


	const Configuration::DisplayAspectRatio* Configuration::GetDisplayAspectRatios() const
	{
		return configAspectRatios;
	}

	float Configuration::GetSelectedDisplayAspectRatio() const
	{
#if !PIMPPLAYER_USECONFIGDIALOG
		if (forceResolutionAndAspect)
			return forcedAspectRatio.aspect;
		else
#endif
			return configAspectRatios[selectedDisplayAspectRatio].aspect;
	}

	int Configuration::GetNumDisplayAspectRatios() const
	{
		return sizeof(configAspectRatios) / sizeof(Configuration::DisplayAspectRatio);
	}

	const Configuration::DisplayAspectRatio& Configuration::GetMaxDisplayAspectRatio() const
	{
#if !PIMPPLAYER_USECONFIGDIALOG
		if (forceResolutionAndAspect)
			return forcedAspectRatio;
		else
#endif
			return configAspectRatios[GetNumDisplayAspectRatios()-1];
	}
}

