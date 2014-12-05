	
/*
	Player stub.

	Look for 'FIXME' & '@plek' -> stuff to either, fix, remove or formalize.

	The idea here is to:
	- Create and manage a simple render window.
	- Initialize and maintain DXGI/D3D and create the device exactly like Core wants it.
	- Kick off audio.
	- Provide a stable main loop.
	- Take care of proper shutdown and error message display.

	Oh, and try some leak detection using an external tool or debug heap.
*/

#include <Core/Platform.h>
#include <intrin.h> // for SIMD check
#include <Shared/stopwatch.h>
#include <Shared/misc.h>
#include <Core/Core.h>
#include "Settings.h"
#include "SceneTools.h"
#include "DebugCamera.h"
#include "AutoShaderReload.h"
#include "Audio.h"

#include "Content/Demo.h"

// audio settings
const std::string kMP3Path = PIMPPLAYER_MP3_PATH;
const bool kMuteAudio = PIMPPLAYER_MUTE_AUDIO;

// configuration: windowed (dev. only) / full screen
const bool kWindowed = PIMPPLAYER_WINDOWED_DEV;
const unsigned int kWindowedResX = PIMPPLAYER_WINDOWED_RES_X;
const unsigned int kWindowedResY = PIMPPLAYER_WINDOWED_RES_Y;

// In full screen mode the desktop resolution is adapted.
// Adapting the desktop resolution makes good sense: it's usually the viewer's optimal resolution
// without monitor distortion. And a beam team can very well be instructed to select an appropriate one
// for performance reasons.

// global error message
static std::string s_lastError;
void SetLastError(const std::string &message) { s_lastError = message; }

// DXGI objects
static IDXGIFactory1  *s_pDXGIFactory = nullptr;
static IDXGIAdapter1  *s_pAdapter     = nullptr;
static IDXGIOutput    *s_pDisplay     = nullptr;
static DXGI_MODE_DESC  s_displayMode;

// app. window
static bool s_classRegged = false;
static HWND s_hWnd = NULL;
static bool s_wndIsActive; // set by WindowProc()

// Direct3D objects
static ID3D11Device        *s_pD3D        = nullptr;
static ID3D11DeviceContext *s_pD3DContext = nullptr;
static IDXGISwapChain      *s_pSwapChain  = nullptr;

// Debug camera and it's state.
#if defined(_DEBUG) || defined(_DESIGN)
static AutoShaderReload* s_pAutoShaderReloader = nullptr;
static DebugCamera* s_pDebugCamera = nullptr;

static bool		s_isPaused			= false;
static bool		s_isMouseTracking	= false;
static int		s_mouseTrackInitialX;
static int		s_mouseTrackInitialY;
#endif

static bool CreateDXGI(HINSTANCE hInstance)
{
	if FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&s_pDXGIFactory)))
	{
		SetLastError("Can not create DXGI 1.1 factory.");
		return false;
	}

	// get primary adapter
	s_pDXGIFactory->EnumAdapters1(0, &s_pAdapter);
	if (nullptr == s_pAdapter)
	{
		SetLastError("No primary display adapter found.");
		return false;
	}

//	DXGI_ADAPTER_DESC1 adDesc;
//	s_pAdapter->GetDesc1(&adDesc);

	// and it's display
	s_pAdapter->EnumOutputs(0, &s_pDisplay);
	if (nullptr == s_pDisplay)
	{
		SetLastError("No physical display attached to primary adapter.");
		return false;
	}

	// get current display mode
	// FindClosestMatchingMode() has failed when used on some systems' external display (hello Toshiba!)
	// as a solution the primary display's (desktop) resolution is used to get an exact match
	DXGI_MODE_DESC modeToMatch;
	modeToMatch.Width = GetSystemMetrics(SM_CXSCREEN);
	modeToMatch.Height = GetSystemMetrics(SM_CYSCREEN);
	modeToMatch.RefreshRate.Numerator = 0;
	modeToMatch.RefreshRate.Denominator = 0;
	modeToMatch.Format = PIMP_BACKBUFFER_FORMAT_LIN;
	modeToMatch.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	modeToMatch.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if FAILED(s_pDisplay->FindClosestMatchingMode(&modeToMatch, &s_displayMode, NULL))
	{
		SetLastError("Can not retrieve primary monitor's display mode.");
		return false;
	}

	// Now that we've found a valid backbuffer, force its format to our gamma-corrected backbuffer.
	// We couldn't search for the gamma-corrected format right away, since FindClosestMatchingMode doesn't 
	// appear to take sRGB formats into account...
	s_displayMode.Format = PIMP_BACKBUFFER_FORMAT_GAMMA;

	if (kWindowed)
	{
		// override resolution
		s_displayMode.Width  = kWindowedResX;
		s_displayMode.Height = kWindowedResY;
	}

	return true;
}

static void DestroyDXGI()
{
	SAFE_RELEASE(s_pDisplay);
	SAFE_RELEASE(s_pAdapter);
	SAFE_RELEASE(s_pDXGIFactory);
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// debug camera mouse input
#if defined(_DEBUG) || defined(_DESIGN)
	case WM_LBUTTONDOWN:
		s_isMouseTracking = true;
		s_mouseTrackInitialX = LOWORD(lParam);
		s_mouseTrackInitialY = HIWORD(lParam);
		s_pDebugCamera->StartLookAt(); 
		break;

	case WM_LBUTTONUP:
		s_isMouseTracking = false;
		s_pDebugCamera->EndLookAt();
		break;

	case WM_MOUSEMOVE:
		if (s_isMouseTracking) {
			int posX = LOWORD(lParam);
			int posY = HIWORD(lParam);
			s_pDebugCamera->LookAt(posX - s_mouseTrackInitialX, posY - s_mouseTrackInitialY); }
		break;
#endif

	case WM_CLOSE:
		PostQuitMessage(0); // terminate message loop
		s_hWnd = NULL;      // DefWindowProc() will call DestroyWindow()
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		
		// debug camera (un)pause 
#if defined(_DEBUG) || defined(_DESIGN)
		case VK_SPACE:
			{
				s_isPaused = !s_isPaused;
				DEBUG_LOG(s_isPaused ? "Entering debug camera mode." : "Leaving debug camera mode.")

				s_pDebugCamera->SetEnabled(s_isPaused);

				if (false == kWindowed)
					ShowCursor(true == s_isPaused);
			}
			break;
#endif
		}

		// debug camera keyboard input
#if defined(_DEBUG) || defined(_DESIGN)
		if (s_isPaused)
		{
			if (wParam == 'A')
				s_pDebugCamera->Move(Vector3(-1.0f, 0.0f, 0.0f));
			else if (wParam == 'D')
				s_pDebugCamera->Move(Vector3(+1.0f, 0.0f, 0.0f));
			else if (wParam == 'W')
				s_pDebugCamera->Move(Vector3( 0.0f, 0.0f,-1.0f));
			else if (wParam == 'S')
				s_pDebugCamera->Move(Vector3( 0.0f, 0.0f,+1.0f));
			else if (wParam == 'Q' && !s_isMouseTracking)
				s_pDebugCamera->Roll(false);
			else if (wParam == 'E' && !s_isMouseTracking)
				s_pDebugCamera->Roll(true);			
			else if (wParam == VK_RETURN)
				s_pDebugCamera->DumpCurrentTransformToOutputWindow();
		}
#endif
		break;

	case WM_ACTIVATE:
		switch (LOWORD(wParam))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if (false == kWindowed) 
			{
				// (re)assign WS_EX_TOPMOST style
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			
			s_wndIsActive = true;
			break;
		
		case WA_INACTIVE:
			if (false == kWindowed) 
			{
				if (NULL != s_pSwapChain)
				{
					// push window to bottom of the Z order
					SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}

			s_wndIsActive = false;
			break;
		};

	case WM_SIZE:
		break; // ALT+ENTER is blocked, all else is ignored or scaled if the window type permits it.
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


#include "resource.h"

static bool CreateAppWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = (kWindowed) ? (HBRUSH) GetStockObject(BLACK_BRUSH) : NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = PIMPPLAYER_RELEASE_ID;
	wndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	
	if (0 == RegisterClassEx(&wndClass))
	{
		SetLastError("Can not create application window (RegisterClassEx() failed).");
		return false;
	}

	s_classRegged = true;
	
	DWORD windowStyle, exWindowStyle;
	if (kWindowed)
	{
		// windowed style
		windowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU;
		exWindowStyle = 0;
	}
	else
	{
		// full screen style (WS_EX_TOPMOST assigned by WM_ACTIVATE)
		windowStyle = WS_POPUP;
		exWindowStyle = 0;
	}

	// calculate full window size
	RECT wndRect = { 0, 0, s_displayMode.Width, s_displayMode.Height };
	AdjustWindowRectEx(&wndRect, windowStyle, FALSE, exWindowStyle);
	const int wndWidth = wndRect.right - wndRect.left;
	const int wndHeight = wndRect.bottom - wndRect.top;

	s_hWnd = CreateWindowEx(
		exWindowStyle,
		PIMPPLAYER_RELEASE_ID,
		PIMPPLAYER_RELEASE_TITLE,
		windowStyle,
		0, 0, // always pop up on primary display's desktop area!
		wndWidth, wndHeight,
		NULL,
		NULL,
		hInstance,
		nullptr);	

	if (NULL == s_hWnd)
	{
		SetLastError("Can not create application window (CreateWindowEx() failed).");
		return false;
	}

	ShowWindow(s_hWnd, (kWindowed) ? nCmdShow : SW_SHOW);

	return true;
}

static bool UpdateAppWindow(bool &renderFrame)
{
	// skip frame unless otherwise specified
	renderFrame = false;

	// got a message to process?
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			// quit!
			return false;
		}
		
		// dispatch message
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
	{
		// no message: window alive?
		if (NULL != s_hWnd)
		{
			if (false == kWindowed && s_wndIsActive)
			{
				// kill cursor for active full screen window
				SetCursor(NULL);
			}

			// render frame if windowed or full screen window has focus
			if (kWindowed || s_wndIsActive)
			{
				renderFrame = true;
			}
			else 
			{
				// full screen window out of focus: relinquish rest of time slice
				Sleep(0); 
			}
		}
	}
	
	// continue!
	return true;
}

void DestroyAppWindow(HINSTANCE hInstance)
{
	if (NULL != s_hWnd)
	{
		DestroyWindow(s_hWnd);
		s_hWnd = NULL;
	}	
	
	if (s_classRegged)
	{
		UnregisterClass("RenderWindow", hInstance);
	}
}

static bool CreateDirect3D()
{
	// create device
#if _DEBUG
	const UINT Flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
	const UINT Flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1
//		D3D_FEATURE_LEVEL_10_0,
//		D3D_FEATURE_LEVEL_9_3,
//		D3D_FEATURE_LEVEL_9_2,
//		D3D_FEATURE_LEVEL_9_1
	};

	// FIXME: decide what we'll do with this information.
	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	memset(&swapDesc, 0, sizeof(swapDesc));
	swapDesc.BufferDesc = s_displayMode;
	swapDesc.SampleDesc.Count = D3D_ANTIALIAS_NUM_SAMPLES;
	swapDesc.SampleDesc.Quality = D3D_ANTIALIAS_QUALITY;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = s_hWnd;
	swapDesc.Windowed = kWindowed;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hRes = D3D11CreateDeviceAndSwapChain(
		s_pAdapter,
		D3D_DRIVER_TYPE_UNKNOWN, // This is documented in the "Remarks" section of this call.
		NULL,
		Flags,
		featureLevels, ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapDesc,
		&s_pSwapChain,
		&s_pD3D,
		&featureLevel,
		&s_pD3DContext);
	if (S_OK == hRes)
	{
		hRes = s_pDXGIFactory->MakeWindowAssociation(s_hWnd, DXGI_MWA_NO_WINDOW_CHANGES);
		ASSERT(hRes == S_OK);
		return true;
	}

	// Failed :(
	std::stringstream message;
	message << "Can't create Direct3D 11.0 device.\n\n";
	message << ((true == kWindowed) ? "Type: windowed.\n" : "Type: full screen.\n");
	message << "Resolution: " << s_displayMode.Width << "*" << s_displayMode.Height << ".\n\n";
	message << DXGetErrorString(hRes) << " - " << DXGetErrorDescription(hRes) << ".\n";
	SetLastError(message.str());
	return false;
}

static void DestroyDirect3D()
{
	if (false == kWindowed && nullptr != s_pSwapChain)
		s_pSwapChain->SetFullscreenState(FALSE, nullptr);
	
	SAFE_RELEASE(s_pSwapChain);
	SAFE_RELEASE(s_pD3DContext);
	SAFE_RELEASE(s_pD3D);
}

#include "CPUID.h"

int __stdcall Main(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	// change path to target root
	SetCurrentDirectory("..\\");

	// check for AVX
	int cpuInfo[4];
	__cpuid(cpuInfo, 1);
	if (0 == (cpuInfo[2] & CPUID_FEAT_ECX_AVX))
	{
		MessageBox(NULL, "Processor does not support AVX instructions.", "Error!", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	if (false == kWindowed)
	{
		// This mode will prevent a crash in full screen mode from popping up a system error dialog that
		// may not receive focus resulting in a "hangup". If complemented with a proper general exception
		// handler it might be an ideal solution for that particular frustration.
		SetErrorMode(SEM_NOGPFAULTERRORBOX);

		// FIXME: it doesn't have that effect on the debugger, I'd like to see that fixed too (focus issue I think).
	}

	// initialize Matrix4 class
	Matrix4::Init();

	// initialize DXGI
	if (CreateDXGI(hInstance))
	{
		// now set up Core configuration
		Pimp::Configuration::DisplayMode displayMode;
		displayMode.width = s_displayMode.Width;
		displayMode.height = s_displayMode.Height;
		Pimp::gCoreCfg.SetDisplayMode(displayMode);
		Pimp::gCoreCfg.SetFullscreen(false == kWindowed);
		Pimp::gCoreCfg.SetRenderAspectRatio(PIMPPLAYER_RENDER_ASPECT_RATIO);

		// create app. window
		if (CreateAppWindow(hInstance, nCmdShow))
		{
			// initialize audio
			if (Audio_Create(-1, s_hWnd, Demo::GetAssetsPath() + kMP3Path, kMuteAudio))
			{
				// initialize Direct3D
				if (CreateDirect3D())
				{
					try
					{
						// Initialize Core D3D.
						Pimp::gD3D = new Pimp::D3D(s_pD3D, s_pD3DContext, s_pSwapChain);

						// Prepare demo resources.
						const char *rocketClient = (0 == strlen(lpCmdLine)) ? "localhost" : lpCmdLine;
						if (true == Demo::GenerateWorld(rocketClient))
						{

#if defined(_DEBUG) || defined(_DESIGN)
							Pimp::World *pWorld = Demo::GetWorld();

							s_pAutoShaderReloader = new AutoShaderReload(pWorld, 0.5f /* checkInterval */);
							s_pDebugCamera = new DebugCamera(pWorld);

							DEBUG_LOG("============================================================================");
							DEBUG_LOG("Pimp is up and running!");
							DEBUG_LOG("");
							DEBUG_LOG("> SPACE: Toggle debug camera.");
							DEBUG_LOG("");
							DEBUG_LOG("Controls:");
							DEBUG_LOG("> W,S,A,D:   Translate current view forward, back, left or right.");
							DEBUG_LOG("> Q,E:       Roll current view, in either positive or negative direction.");
							DEBUG_LOG("> Drag LMB:  Adjust yaw and pitch of current view.");
							DEBUG_LOG("> ENTER:     Dump current debug camera transform to output window.");
							DEBUG_LOG("============================================================================");
#endif	

							Stopwatch stopwatch;

							// in windowed mode FPS is refreshed every 60 frames
							float timeElapsedFPS = 0.f;
							unsigned int numFramesFPS = 0;
							
							// enter (render) loop
							bool renderFrame;
							while (true == UpdateAppWindow(renderFrame))
							{
								// render frame
								float timeElapsed = stopwatch.GetSecondsElapsedAndReset();

#if defined(_DEBUG) || defined(_DESIGN)
								s_pAutoShaderReloader->Update();

								if (true == s_isPaused)
									Demo::Tick(timeElapsed, s_pDebugCamera->Get());
								else
								{
									if (false == Demo::Tick(timeElapsed))
										break;
								}
#else
								if (false == Demo::Tick(timeElapsed))
									break;
#endif
								
								Demo::WorldRender();
//								Pimp::gD3D->Flip((true == kWindowed) ? 0 : 1);
								Pimp::gD3D->Flip(0);

								if (true == kWindowed)
								{
									// handle FPS counter
									timeElapsedFPS += timeElapsed;

									if (++numFramesFPS == 60)
									{
										const float FPS = 60.f/timeElapsedFPS;
										
										char fpsStr[256];
										sprintf_s(fpsStr, 256, "%s (%.2f FPS)", PIMPPLAYER_RELEASE_TITLE, FPS);
										SetWindowText(s_hWnd, fpsStr);

										timeElapsedFPS = 0.f;
										numFramesFPS = 0;
									}
								}

								Audio_Update();
							}

#if defined(_DEBUG) || defined(_DESIGN)
							delete s_pDebugCamera;
							delete s_pAutoShaderReloader;
#endif	

						}

						Demo::ReleaseWorld();
						delete Pimp::gD3D;
					}

					// Catch anything Core/D3D throws.
					catch(const CoreD3DException &exception)
					{
						SetLastError(exception.what());

						// Disregard any "secondary" resources and just proceed killing D3D, DXGI, BASS & the window.
					}
				}

				DestroyDirect3D();
			}
			
			Audio_Destroy();
		}
		
		DestroyAppWindow(hInstance);
	}

	DestroyDXGI();

	if (!s_lastError.empty())
	{
		MessageBox(NULL, s_lastError.c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow)
{
#if !defined(_DEBUG) && !defined(_DESIGN)
	__try 
	{
#endif

	return Main(hInstance, hPrevInstance, cmdLine, nCmdShow);

#if !defined(_DEBUG) && !defined(_DESIGN)
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// Try a few things to restore the desktop.
		SAFE_RELEASE(s_pSwapChain);
		if (NULL != s_hWnd) DestroyWindow(s_hWnd);

		// Sound the alarm bell.
		MessageBox(NULL, "Demo crashed (unhandled exception). Now quickly: http://www.pouet.net!", PIMPPLAYER_RELEASE_ID, MB_OK | MB_ICONEXCLAMATION);

		// Better do as little as possible past this point.
		_exit(1); 
	}
#endif

	return 0;
}
