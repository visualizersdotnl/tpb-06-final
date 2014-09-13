
// New Player stub, taken from a visualizers.nl project, stripped & adapted to serve in this Inque->Pimp contraption.
// Idea is to provide a stable stub without 64KB-cutbacks (e.g. proper error handling and resource destruction on exit).

// As in the rest of the codebase, I'll comment my own peculiarities with '@plek' (red tape, mostly).
// Over time (and if we continue to use this codebase), these comments should probably vanish.
// Stuff that really needs fixing, here it comes, can be found by searching for 'FIXME'.

// The idea here is to:
// - Create and manage a simple render window.
// - Initialize and maintain DXGI/D3D and create the device exactly like Core wants it.
// - Kick off audio and talk to Rocket.
// - Provide a stable main loop.
// - Take care of proper shutdown and error message display.

// To do (@plek):
// - Finish up taking over all Main.cpp functionality (e.g. FPS counter).
// - Take a close look at current D3D warnings and leaks on exit (World probably isn't releasing anything).
// - Set up Core D3D and World with proper error checking (SetLastError()).
// - Attempt to eliminate most app. C++ exceptions.
// - Carry on with whats in README.md!
//
// Secondary (for those lazy moments):
// - Go look what's redundant in Shared (ask Glow) & remove unused (commented) code.
// - Check FIXMEs (esp. the ALT+ENTER block).
// - Icon. (Shifter!)
// - Keep a close eye on those D3D warnings.

#include <Windows.h>
#include <intrin.h> // for SSE2 check

#include <stdint.h>
// #include <string>
#include <sstream>
#include <vector>

#include <Core/Core.h>

#include "Stub.h"
#include "Settings.h"
#include "SceneTools.h"
#include "DebugCamera.h"

#ifdef _DEBUG
static DebugCamera* s_pDebugCamera;

static bool		s_isPaused			= false;
static bool		s_isMouseTracking	= false;
static int		s_mouseTrackInitialX;
static int		s_mouseTrackInitialY;
#endif


// configuration: windowed (dev. only) / full screen
const bool kWindowed = PIMPPLAYER_FORCE_WINDOWED;
const unsigned int kWindowedResX = PIMPPLAYER_DEV_RES_X;
const unsigned int kWindowedResY = PIMPPLAYER_DEV_RES_Y;

// global error message
// FIXME: Move to Shared.
static std::string s_lastError;
void SetLastError(const std::string &message) { s_lastError = message; }

// DXGI objects
static IDXGIFactory    *s_pDXGIFactory = NULL;
static IDXGIAdapter    *s_pAdapter = NULL;
static IDXGIOutput     *s_pDisplay = NULL;
static DXGI_MODE_DESC   s_displayMode;

// app. window
static bool s_classRegged = false;
HWND        g_hWnd = NULL;
static bool s_wndIsActive; // set by WindowProc()

// Direct3D objects
static ID3D10Device1  *s_pD3D = NULL;
static IDXGISwapChain *s_pSwapChain = NULL;

// World, our Core container for the entire demo.
Pimp::World *gWorld = nullptr;

// World generator (basically our complete content initalization).
// Lives in Scene.cpp.
extern void GenerateWorld(Pimp::World** outWorld);

static bool CreateDXGI(HINSTANCE hInstance)
{
	if FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void **>(&s_pDXGIFactory)))
	{
		SetLastError("Can not create DXGI factory.");
		return false;
	}

	// get primary display
	s_pDXGIFactory->EnumAdapters(0, &s_pAdapter);
	s_pAdapter->EnumOutputs(0, &s_pDisplay);
	ASSERT(NULL != s_pAdapter && NULL != s_pDisplay);

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
#ifdef _DEBUG
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
		if (s_isMouseTracking)
		{
			int posX = LOWORD(lParam);
			int posY = HIWORD(lParam);

			s_pDebugCamera->LookAt(posX - s_mouseTrackInitialX, posY - s_mouseTrackInitialY);
		}
		break;
#endif

	case WM_CLOSE:
		PostQuitMessage(0); // terminate message loop
		g_hWnd = NULL;      // DefWindowProc() will call DestroyWindow()
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
#ifdef _DEBUG
		case VK_SPACE:
			{
				s_isPaused = !s_isPaused;
				DEBUG_LOG(s_isPaused ? "Updating has been paused." : "Updating has been resumed!")

				s_pDebugCamera->SetEnabled(s_isPaused);

				ShowCursor(s_isPaused);
			}
			break;
#endif
		}

#ifdef _DEBUG
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

				// switch to full screen
				if (NULL != s_pSwapChain)
					s_pSwapChain->SetFullscreenState(TRUE, s_pDisplay);
			}
			
			s_wndIsActive = true;
			break;
		
		case WA_INACTIVE:
			if (false == kWindowed) 
			{
				// exit full screen
				if (NULL != s_pSwapChain)
					s_pSwapChain->SetFullscreenState(FALSE, NULL);

				// push window to bottom of the Z order
				SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			
			s_wndIsActive = false;
			break;
		};

	case WM_SIZE:
		break; // FIXME: ALT+ENTER issue.
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static bool CreateAppWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INDIGO)); // FIXME!
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = (kWindowed) ? (HBRUSH) GetStockObject(BLACK_BRUSH) : NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = PIMPPLAYER_RELEASE_ID;
	wndClass.hIconSm = NULL;
	
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

	g_hWnd = CreateWindowEx(
		exWindowStyle,
		PIMPPLAYER_RELEASE_ID,
		PIMPPLAYER_RELEASE_TITLE,
		windowStyle,
		0, 0, // always pop up on primary display's desktop area!
		wndWidth, wndHeight,
		NULL,
		NULL,
		hInstance,
		NULL);	

	if (NULL == g_hWnd)
	{
		SetLastError("Can not create application window (CreateWindowEx() failed).");
		return false;
	}

	ShowWindow(g_hWnd, (kWindowed) ? nCmdShow : SW_SHOW);

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
		if (NULL != g_hWnd)
		{
			if (!kWindowed && s_wndIsActive)
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
	if (NULL != g_hWnd)
	{
		DestroyWindow(g_hWnd);
		g_hWnd = NULL;
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
	const UINT Flags = D3D10_CREATE_DEVICE_SINGLETHREADED | D3D10_CREATE_DEVICE_DEBUG;
#else
	const UINT Flags = D3D10_CREATE_DEVICE_SINGLETHREADED;
#endif

	if SUCCEEDED(D3D10CreateDevice1(
		s_pAdapter,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		Flags,
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_1_SDK_VERSION,
		&s_pD3D))
	{
		// create swap chain
		DXGI_SWAP_CHAIN_DESC swapDesc;
		swapDesc.BufferDesc = s_displayMode;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 3;
		swapDesc.OutputWindow = g_hWnd;
		swapDesc.Windowed = kWindowed;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if SUCCEEDED(s_pDXGIFactory->CreateSwapChain(s_pD3D, &swapDesc, &s_pSwapChain))
		{
			// FIXME: block automatic ALT+ENTER (doesn't quite work?)
			s_pDXGIFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

			return true;
		}
	}

	// either one failed, passing it off as one error
	std::string message;
	message  = "Can not create Direct3D 10 device.\r\n\r\n";
	message += (kWindowed) ? "Type: windowed.\r\n" : "Type: full screen.\r\n";
	message += "Resolution: " + ToString(s_displayMode.Width) + "*" + ToString(s_displayMode.Height) + ".";
	SetLastError(message);
	return false;
}

static void DestroyDirect3D()
{
	if (NULL != s_pSwapChain)
	{
		s_pSwapChain->SetFullscreenState(FALSE, NULL);
		s_pSwapChain->Release();
	}

	SAFE_RELEASE(s_pD3D);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	// change path to target root
	SetCurrentDirectory("..\\");

	// check for SSE2
	int cpuInfo[4];
	__cpuid(cpuInfo, 1);
	if (0 == (cpuInfo[3] & 1<<26))
	{
		MessageBox(NULL, "Processor does not support SSE2 instructions.", "Error!", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	if (false == kWindowed)
	{
		// This mode will prevent a crash in full screen mode from popping up a system error dialog that
		// may not receive focus resulting in a "hangup". If complemented with a proper general exception
		// handler it might be an ideal solution for that particular frustration.
		SetErrorMode(SEM_NOGPFAULTERRORBOX);
	}

	// initialize Matrix4 class
	Matrix4::Init();

	// initialize DXGI
	if (CreateDXGI(hInstance))
	{
		// now set up Core configuration
		Pimp::Configuration::Init();

		Pimp::Configuration::DisplayMode displayMode;
		displayMode.width = s_displayMode.Width;
		displayMode.height = s_displayMode.Height;
		Pimp::Configuration::Instance()->SetDisplayMode(displayMode);

		Pimp::Configuration::Instance()->SetFullscreen(false == kWindowed);
		Pimp::Configuration::Instance()->SetRenderAspectRatio(PIMPPLAYER_RENDER_ASPECT_RATIO);

		// create app. window
		if (CreateAppWindow(hInstance, nCmdShow))
		{
			// initialize BASS audio library
			if (1) // Audio_Create(-1, g_hWnd))
			{
				// initialize Direct3D
				if (CreateDirect3D())
				{
					// Initialize Core D3D.
					Pimp::gD3D = new Pimp::D3D(s_pD3D, s_pSwapChain);
					if (1) // FIXME: Move further Core D3D initialization out of constructor.
					{
						InitMaterialCompilationSystem();
						DrawLoadProgress(false);

						GenerateWorld(&gWorld);
						if (nullptr != gWorld)
						{
#ifdef _DEBUG
							s_pDebugCamera = new DebugCamera(gWorld);
#endif	

							// reset loading bar
							gWorld->GetPostProcess()->SetLoadProgress(0.0f);

							Stopwatch stopwatch;
							
							// enter (render) loop
							bool renderFrame;
							while (true == UpdateAppWindow(renderFrame))
							{
								// render frame
								float timeElapsed = stopwatch.GetSecondsElapsedAndReset();
#ifdef _DEBUG
								if (s_isPaused)
									timeElapsed = 0;
#endif

								gWorld->Tick(timeElapsed);
								gWorld->Render();

								// frame rendered?
								if (true == renderFrame)
								{
									// yes: flip/blit
									const HRESULT hRes = s_pSwapChain->Present((kWindowed) ? 0 : 1, 0);
									// FIXME: ignoring DXGI_ERROR_DEVICE_RESET
								}
							}

							delete Pimp::gD3D;
						}
					}
				}

				DestroyDirect3D();
			}
			
//			Audio_Destroy();
		}
		
		DestroyAppWindow(hInstance);
	}

	// Yes, Pimp::Configuration is a singleton.
	// FIXME: I'd like to change that as there's no real reason for it (at least not anymore).
	Pimp::Configuration::Free();

	DestroyDXGI();

	if (!s_lastError.empty())
	{
		MessageBox(NULL, s_lastError.c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	return 0;
}
