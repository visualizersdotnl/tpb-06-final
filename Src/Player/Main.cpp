
/* @plek:

	I'm going through this file to get it a bit more up to demo standard (i.e. proper release of resources and
	error messages) whilst maintaining backward compatibility with 64KBs (see Settings.h for a new toggle).
		
	I also took out some stuff I deemed useless for this project and added a ton of obvious comments, but those are
	chiefly for me to keep oversight during this process. After all this code and it's intricacies are new to me :-)

	Issues I've spotted and intened to fix:
	- D3D CreateDevice() throws "Invalid parameter passed to C runtime function." into my output window.
	- DXGI leaks on exit (see output).

	Where I'll go from here:
	- Add demo mode to Core and have D3D (and more?) throw exceptions when due.
	- Add BASS/Rocket support:
	  + Plug in my BASS stuff and play the MP3, obviously.
	  + Integrate and connect to Rocket.
	  + Revise timing logic (world tick) to fit Rocket tracks.
	  + Lastly look at the Rocket content files and the (final) replay mode.
	- Divert attention to what's going on in Content/Scene.cpp!
	- Implement GP exception handler (for release builds and pesky full screen crashes).
	  I already fixed the latter a little bit with a nifty SetErrorMode() call.
*/

#include <Core/Core.h>
#include <Windows.h>
#include "Settings.h"
#include "Resource.h"
#include "SceneTools.h"
#include "DebugCamera.h"

//
// Global handles/pointers.
//

HWND gHwnd = 0;
Pimp::World* gWorld = nullptr;

#ifdef _DEBUG
DebugCamera* gDebugCamera;
#endif

//
// PNG recorder stuff.
//

#if PIMPPLAYER_RECORD_TO_PNG
ID3D10Texture2D* recordToPNGCaptureBuffer;
void CaptureFrame();
int recordFrameIndex = 0;
#endif

//
// Debug input state.
//

#ifdef _DEBUG
bool gIsPaused = false;
bool gIsMouseTracking = false;
int gMouseTrackInitialX;
int gMouseTrackInitialY;
#endif

//
// World creator.
//

// @plek: Resides in Content/Scene.cpp
extern void GenerateWorld_scene(Pimp::World** outWorld);

//
// Window proc.
//

LRESULT CALLBACK D3DWindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
#ifdef _DEBUG
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			gIsPaused = !gIsPaused;
			DEBUG_LOG(gIsPaused ? "Updating has been paused." : "Updating has been resumed!")

			gDebugCamera->SetEnabled(gIsPaused);

			ShowCursor(gIsPaused);
		}
		if (gIsPaused)
		{
			if (wParam == 'A')
				gDebugCamera->Move(Vector3(-1.0f, 0.0f, 0.0f));
			else if (wParam == 'D')
				gDebugCamera->Move(Vector3(+1.0f, 0.0f, 0.0f));
			else if (wParam == 'W')
				gDebugCamera->Move(Vector3( 0.0f, 0.0f,-1.0f));
			else if (wParam == 'S')
				gDebugCamera->Move(Vector3( 0.0f, 0.0f,+1.0f));
			else if (wParam == 'Q' && !gIsMouseTracking)
				gDebugCamera->Roll(false);
			else if (wParam == 'E' && !gIsMouseTracking)
				gDebugCamera->Roll(true);			
		}
		
		break;
	case WM_LBUTTONDOWN:
		gIsMouseTracking = true;
		gMouseTrackInitialX = LOWORD(lParam);
		gMouseTrackInitialY = HIWORD(lParam);
		gDebugCamera->StartLookAt();
		break;
	case WM_LBUTTONUP:
		gIsMouseTracking = false;
		gDebugCamera->EndLookAt();
		break;
	case WM_MOUSEMOVE:
		if (gIsMouseTracking)
		{
			int posX = LOWORD(lParam);
			int posY = HIWORD(lParam);

			gDebugCamera->LookAt(posX - gMouseTrackInitialX, posY - gMouseTrackInitialY);
		}
		break;
#endif
	case WM_CLOSE:
	case WM_CHAR:
		if (msg != WM_CHAR || (char)wParam == 27)
			// If WM_CLOSE or ESC key pressed, bail out.
			PostQuitMessage(0);
			gHwnd = NULL;// @plek: Important -> after the break, DefWindowProc() will call DestroyWindow() for us!
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

//
// 64KB quick terminate function.
//

#if !defined(_DEBUG) && !defined(PIMPPLAYER_IS_DEMO)
void TerminateOnError64KB()
{
	// At the very least enhance our chances of returning to the desktop.
	delete Pimp::gD3D; 
	
	MessageBox(NULL, "Could not initialize.", "Error!", MB_OK|MB_ICONEXCLAMATION);
	ExitProcess(1);
}
#endif

//
// Configuration dialog.
//

#if PIMPPLAYER_USECONFIGDIALOG

static void ConfigFillResolutionsComboBox(int adapterIndex)
{
	Pimp::Configuration* mainConfiguration = Pimp::Configuration::Instance();

	const FixedSizeList<Pimp::Configuration::Adapter>& adapters = 
		mainConfiguration->GetAdapters();


}

INT_PTR CALLBACK ConfigDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	Pimp::Configuration* mainConfiguration = Pimp::Configuration::Instance();

	switch (message) 
	{ 
	case WM_INITDIALOG: 
		{
			const FixedSizeList<Pimp::Configuration::Adapter>& adapters = 
				mainConfiguration->GetAdapters();

			const FixedSizeList<Pimp::Configuration::DisplayMode>& displayModes = 
				mainConfiguration->GetDisplayModes();

			HWND hwndRes = GetDlgItem(hwndDlg, IDC_COMBORESO);
			HWND hwndAdapt = GetDlgItem(hwndDlg, IDC_COMBOADAPTER);
			HWND hwndAspect = GetDlgItem(hwndDlg, IDC_COMBOASPECT);

			// adapters

			for (int i=0; i<adapters.Size(); ++i)
			{
				const Pimp::Configuration::Adapter& adapter = adapters[i];

				char s[256], tmp[256];
				strcpy(s, "Adapter ");
				_itoa(i, tmp, 10);
				strcat(s, tmp);
				strcat(s, ": ");
				strcat(s, adapter.name);

				SendMessage(hwndAdapt, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)s);
			}


			for (int j=0; j<displayModes.Size(); ++j)
			{
				const Pimp::Configuration::DisplayMode& mode = 
					displayModes[j];

				char s[256], tmp[256];
				_itoa(mode.width, s, 10);
				strcat(s, "x");
				_itoa(mode.height, tmp, 10);
				strcat(s, tmp);

				SendMessage(hwndRes, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)s);
			}



			// aspect

			const Pimp::Configuration::DisplayAspectRatio* aspectRatio = 
				mainConfiguration->GetDisplayAspectRatios();

			for (int i=0; i<mainConfiguration->GetNumDisplayAspectRatios(); ++i, ++aspectRatio)
			{
				SendMessage(
					hwndAspect, 
					CB_ADDSTRING, 
					0, 
					(LPARAM)(LPCTSTR)aspectRatio->name);
			}

			SendMessage(hwndRes, CB_SETCURSEL, mainConfiguration->GetSelectedDisplayModeIndex(), 0);
			SendMessage(hwndAdapt, CB_SETCURSEL, mainConfiguration->GetSelectedAdapterIndex(), 0);
			SendMessage(hwndAspect, CB_SETCURSEL, mainConfiguration->GetSelectedDisplayAspectRatioIndex(), 0);

#ifdef _DEBUG
			CheckDlgButton(hwndDlg, IDC_CHECKWIN, BST_CHECKED);
#endif
			CheckDlgButton(hwndDlg, IDC_CHECKVS, BST_CHECKED);
		}

		return TRUE; 

	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			{
				// store

				int adapterindex = (int) SendMessage(GetDlgItem(hwndDlg, IDC_COMBOADAPTER), CB_GETCURSEL, 0, 0);
				int resoindex = (int) SendMessage(GetDlgItem(hwndDlg, IDC_COMBORESO), CB_GETCURSEL, 0, 0);

				mainConfiguration->SetSelectedAdapter(adapterindex);
				mainConfiguration->SetSelectedGraphicsMode(resoindex);

				mainConfiguration->SetSelectedDisplayAspectRatio( 
					(int) SendMessage(GetDlgItem(hwndDlg, IDC_COMBOASPECT), CB_GETCURSEL, 0, 0)
					);

				mainConfiguration->SetFullscreen(
					IsDlgButtonChecked(hwndDlg, IDC_CHECKWIN) != BST_CHECKED);
			}
			EndDialog(hwndDlg, IDOK);
			break;

		case IDCANCEL: 
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
	} 
	return FALSE; 
}


bool RunConfiguration()
{
	INT_PTR result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOGCONFIG), 
		NULL, ConfigDialogProc);

#if defined(_DEBUG) || defined(PIMPPLAYER_IS_DEMO)
	if (result <= 0)
		throw Win32Exception();
#else
		TerminateOnError64KB();
#endif

	return (result == IDOK);
}

#endif

//
// WinMain.
//

int WINAPI WinMain(
	HINSTANCE hInstance,      // handle to current instance
	HINSTANCE hPrevInstance,  // handle to previous instance
	LPSTR lpCmdLine,          // command line
	int nCmdShow              // show state
	)
{
#if defined(_DEBUG) || defined(PIMPPLAYER_IS_DEMO)
	char exceptionMsg[8192] = { 0 };
	try
	{
#endif
		// This won't allocate anything, so ride along.
		Matrix4::Init();

		// Handle configuration.
		// Will throw an exception or terminate. 

		Pimp::Configuration::Init();

#if !PIMPPLAYER_USECONFIGDIALOG
		Pimp::Configuration::Instance()->SetFullscreen(PIMPPLAYER_FORCEDRESOLUTION_FULLSCREEN);
		Pimp::Configuration::Instance()->SetForcedResolutionAndAspect(PIMPPLAYER_FORCEDRESOLUTION_X, PIMPPLAYER_FORCEDRESOLUTION_Y, PIMPPLAYER_FORCEDRESOLUTION_ASPECT);
#else		
		Pimp::Configuration::Instance()->SetFullscreen(false);
		if (false == RunConfiguration())
		{
			// User bailed.
#if defined(PIMPPLAYER_IS_DEMO)
			Pimp::Configuration::Free();
#endif
			return 1;
		}
#endif

#if defined(DEBUG) || defined(PIMPPLAYER_IS_DEMO)
		const bool windowed = !Pimp::Configuration::Instance()->GetFullscreen();
		if (false == windowed)
		{
			// @plek: Now you'll hate or love this but it takes a crashing full screen D3D
			//        application right down instead of "hanging" on either a debugger or Windows error box
			//        that can't get any focus. If complemented with a general exception handler that tells you
			//        something wen't wrong as well, it's an ideal combination for that particular frustration.
			SetErrorMode(SEM_NOGPFAULTERRORBOX);
		}
#endif

		// Create window.
		//

		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, (WNDPROC) D3DWindowProc, 0L, 0L, 
			GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
			PIMPPLAYER_RELEASE_ID, NULL };

		if (!RegisterClassEx( &wc ))
		{
#if defined(_DEBUG) || defined(PIMPPLAYER_IS_DEMO)
			throw Exception("Could not register window class!");
#else
			TerminateOnError64KB();
#endif
		}

		const Pimp::Configuration::DisplayMode& mode = 
			Pimp::Configuration::Instance()->GetSelectedDisplayMode();

		int w = mode.width;
		int h = mode.height;

		if (true == windowed)
		{
			w += 2*GetSystemMetrics(SM_CYDLGFRAME);
			h += GetSystemMetrics(SM_CYCAPTION) + 2*GetSystemMetrics(SM_CXDLGFRAME);
		}

		gHwnd = CreateWindow(PIMPPLAYER_RELEASE_ID, PIMPPLAYER_RELEASE_TITLE, 
			WS_VISIBLE|WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, w, h,
			NULL, NULL, wc.hInstance, NULL );

		if (!gHwnd)
		{
#if defined(_DEBUG) || defined(PIMPPLAYER_IS_DEMO)
			throw Exception("Could not create window!");
#else
			TerminateOnError64KB();
#endif
		}

		// @plek: Keep cursor in windowed mode. I'm used to clicking that little cross :)
		if (false == windowed)
		{
			ShowCursor(FALSE);
		}

		// Initialize Direct3D.
		// @plek: This needs error checking when running as a demo in a few key points and throw due exceptions.

		Pimp::gD3D = new Pimp::D3D(gHwnd);

		// Now do a few things that won't fail either.
		BringWindowToTop(gHwnd);
		InitMaterialCompilationSystem();
		DrawLoadProgress(false);

		// Generate the entire "world" (or demo content if you like).
		GenerateWorld_scene(&gWorld);
		// @plek: ^ This should be able to fail by throwing (local) exceptions as well.

#ifdef _DEBUG
		gDebugCamera = new DebugCamera(gWorld);
#endif		

		gWorld->GetPostProcess()->SetLoadProgress(0.0f);

		Pimp::World* currentWorld;
		currentWorld = gWorld;

#ifdef _DEBUG
		// For FPS counter.
		int numFramesRendered = 0;
		float totalTimeElapsed = 0;
#endif

		DEBUG_LOG("============================================================================");
		DEBUG_LOG("Pimp is up and running!");
		DEBUG_LOG("");
		DEBUG_LOG("> SPACE: Toggle pause");
		DEBUG_LOG("");
		DEBUG_LOG("If paused:");
		DEBUG_LOG("> W,S,A,D: Translate current view forward, back, left or right.");
		DEBUG_LOG("> Q,E: Roll current view, in either positive or negative direction.");
		DEBUG_LOG("> Left mouse button+dragging: Adjust yaw and pitch of current view.");
		DEBUG_LOG("============================================================================");

		// @plek: As soon as Rocket runs the show this won't be for much else than the FPS counter.
		Stopwatch stopwatch;

		MSG msg; 
		ZeroMemory( &msg, sizeof(msg) );

		// @plek: For now this loop is continuous, fix it with a Rocket track.
		while (msg.message != WM_QUIT)
		{
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				float timeElapsed = stopwatch.GetSecondsElapsedAndReset();

#ifdef _DEBUG
				if (gIsPaused)
					timeElapsed = 0;
#endif
				
				// Tick and render.
				// @plek: For Rocket integration I'll have to think of something like a master tick track.
				currentWorld->Tick(timeElapsed);
				currentWorld->Render();

#ifdef _DEBUG
				// FPS counter.
				totalTimeElapsed += timeElapsed;

				if (++numFramesRendered == 15)
				{
					float fps = 15.0f / totalTimeElapsed;

					char s[256];
					sprintf_s(s,256, "%.2f fps", fps);
					SetWindowText(gHwnd, s);

					numFramesRendered = 0;
					totalTimeElapsed = 0;
				}
#endif
			}
		}
	}

#if defined(_DEBUG) || defined(PIMPPLAYER_IS_DEMO)
	catch(const Exception& e)
	{
		// Custom exception occured. Store it for display in a bit...
		sprintf_s(exceptionMsg, 8192, "Got an exception:\n%s", e.GetMessage().c_str());
	}

	// Get rid of World and D3D.
	delete gWorld;
	delete Pimp::gD3D;
	
	// If necessary, destroy window, then unregister class (harmless if it fails).
	if (NULL != gHwnd) DestroyWindow(gHwnd);
	UnregisterClass(PIMPPLAYER_RELEASE_ID, GetModuleHandle(NULL)); // DestroyWindow() has been called implicitly.

	Pimp::Configuration::Free();

	// Display exception if necessary.
	if (0 != strlen(exceptionMsg))
		MessageBox(0, exceptionMsg, PIMPPLAYER_RELEASE_TITLE, MB_ICONEXCLAMATION|MB_OK);
#endif

	return 0;
}
