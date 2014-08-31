#include <Core/Core.h>
#include <Windows.h>
#include "Settings.h"
#include "Resource.h"
#include "SceneTools.h"


/*static*/ HWND gHwnd;
/*static*/ Pimp::World* gWorld;


#if PIMPPLAYER_RECORD_TO_PNG
ID3D10Texture2D* recordToPNGCaptureBuffer;
void CaptureFrame();
int recordFrameIndex = 0;
#endif


extern void GenerateWorld_scene(Pimp::World** outWorld);


// Dummy window proc.
LRESULT CALLBACK D3DWindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_CLOSE: 
	case WM_DESTROY:
	case WM_CHAR:
		if (msg != WM_CHAR || (char)wParam == 27)
			PostQuitMessage(0);
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

#ifndef _DEBUG
void TerminateWithInitError()
{
	MessageBox(NULL, "Could not init!", "Error!", MB_OK|MB_ICONEXCLAMATION);
	ExitProcess(1);
}
#endif

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

				int adapterindex = SendMessage(GetDlgItem(hwndDlg, IDC_COMBOADAPTER), CB_GETCURSEL, 0, 0);
				int resoindex = SendMessage(GetDlgItem(hwndDlg, IDC_COMBORESO), CB_GETCURSEL, 0, 0);

				mainConfiguration->SetSelectedAdapter(adapterindex);
				mainConfiguration->SetSelectedGraphicsMode(resoindex);

				mainConfiguration->SetSelectedDisplayAspectRatio( 
					SendMessage(GetDlgItem(hwndDlg, IDC_COMBOASPECT), CB_GETCURSEL, 0, 0)
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

#ifdef _DEBUG
	if (result <= 0)
		throw Win32Exception();
#endif

	return (result == IDOK);
}

#endif


int WINAPI WinMain(
	HINSTANCE hInstance,      // handle to current instance
	HINSTANCE hPrevInstance,  // handle to previous instance
	LPSTR lpCmdLine,          // command line
	int nCmdShow              // show state
	)
{
#ifdef _DEBUG
	try
	{
#endif
		Matrix4::Init();

		Pimp::Configuration::Init();	
		

#if !PIMPPLAYER_USECONFIGDIALOG
		Pimp::Configuration::Instance()->SetFullscreen(PIMPPLAYER_FORCEDRESOLUTION_FULLSCREEN);
		Pimp::Configuration::Instance()->SetForcedResolutionAndAspect(PIMPPLAYER_FORCEDRESOLUTION_X, PIMPPLAYER_FORCEDRESOLUTION_Y, PIMPPLAYER_FORCEDRESOLUTION_ASPECT);
#else		
		Pimp::Configuration::Instance()->SetFullscreen(false);

		if (!RunConfiguration())
			return 1;
#endif

		// Register the window class
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, (WNDPROC) D3DWindowProc, 0L, 0L, 
			GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
			"pimp", NULL };

		if (!RegisterClassEx( &wc ))
		{
#ifdef _DEBUG
			throw Exception("Could not register window class!");
#else
			TerminateWithInitError();
#endif
		}

		const Pimp::Configuration::DisplayMode& mode = 
			Pimp::Configuration::Instance()->GetSelectedDisplayMode();

		int w = mode.width;
		int h = mode.height;

		if (!Pimp::Configuration::Instance()->GetFullscreen())
		{
			w += 2*GetSystemMetrics(SM_CYDLGFRAME);
			h += GetSystemMetrics(SM_CYCAPTION) + 2*GetSystemMetrics(SM_CXDLGFRAME);
		}

		gHwnd = CreateWindow( "pimp", PIMPPLAYER_WINDOWCAPTION, 
			WS_VISIBLE|WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, w, h,
			GetDesktopWindow(), NULL, wc.hInstance, NULL );

		if (!gHwnd)
		{
#ifdef _DEBUG
			throw Exception("Could not create window!");
#else
			TerminateWithInitError();
#endif
		}

		SetCursor(NULL);
		ShowCursor(FALSE);

		Pimp::gD3D = new Pimp::D3D(gHwnd);

		BringWindowToTop(gHwnd);

		InitMaterialCompilationSystem();




		DrawLoadProgress(false);

		GenerateWorld_scene(&gWorld);
		ASSERT( gWorld != NULL );
		

		gWorld->GetPostProcess()->SetLoadProgress(0.0f);

		Pimp::World* currentWorld;

		currentWorld = gWorld;


#ifdef _DEBUG
		gWorld->ForceSetTime(PIMPPLAYER_FORCEDSTARTTIME);
#endif

#if PIMPPLAYER_RECORD_TO_PNG
		recordToPNGCaptureBuffer = Pimp::gD3D->CreateIntermediateCPUTarget(DXGI_FORMAT_B8G8R8A8_UNORM);
#endif

		// start sound?
		
		MSG msg; 
		ZeroMemory( &msg, sizeof(msg) );

		Stopwatch stopwatch;


#ifdef _DEBUG
		int numFramesRendered = 0;
		float totalTimeElapsed = 0;
#endif

		while (msg.message != WM_QUIT)
		{
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
#if PIMPPLAYER_RECORD_TO_PNG
				float timeElapsed = 1.0f/PIMPPLAYER_RECORD_FRAMERATE;
#else
#if PIMPPLAYER_ENABLE_HARDSYNC
				static float prevTime = 0.0f;
				static bool isFirstFrame = true;

				float currentTime = currentSoundSystem->GetPlaybackTime();

				float timeElapsedSincePreviousFrame = stopwatch.GetSecondsElapsedAndReset();
				float estimatedDurationOfCurrentFrame = isFirstFrame ? (1.0f/60.0f) : timeElapsedSincePreviousFrame;
				
				float sceneTimeAtEndOfUpdate = currentTime + estimatedDurationOfCurrentFrame*1.0f;				

				// timeElapsed = actually the time to elapse this frame.
				float timeElapsed = sceneTimeAtEndOfUpdate - currentWorld->GetCurrentTime();
				if (timeElapsed < 0.0f)
					timeElapsed = 0.0f;

				prevTime += timeElapsed;
				isFirstFrame = false;

				//if (timeElapsed == 0.0f)
				//{
				//	DEBUG_LOG("timeElapsed: %f timeElapsedSincePreviousFrame: %f <----<----", timeElapsed, timeElapsedSincePreviousFrame);
				//}
				//else
				//{
				//	DEBUG_LOG("timeElapsed: %f timeElapsedSincePreviousFrame: %f", timeElapsed, timeElapsedSincePreviousFrame);
				//}
				
#else
				float timeElapsed = stopwatch.GetSecondsElapsedAndReset();
#endif	
#endif	

				
				currentWorld->Tick(timeElapsed);
				currentWorld->Render();

#ifdef _DEBUG
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

#if PIMPPLAYER_RECORD_TO_PNG
				CaptureFrame();
#endif

				if (gWorld->GetCurrentTime() > PIMPPLAYER_DURATION)
					PostQuitMessage(1);

			}
		}

#ifdef _DEBUG 
		delete gWorld;

		delete Pimp::gD3D;
		ReleaseDC( gHwnd, GetDC(gHwnd));
		DestroyWindow(gHwnd);
		UnregisterClass("pimp", GetModuleHandle(NULL));

		Pimp::Configuration::Free();
#endif

#ifdef _DEBUG
	}
	catch(const Exception& e)
	{
		char s[8192];
		sprintf_s(s, 8192, "Got an exception:\n%s", e.GetMessage().c_str());
		MessageBox(0, s, "pimp: Aaargh!", MB_ICONEXCLAMATION|MB_OK);
	}
#endif

	return 0;
}


#if PIMPPLAYER_RECORD_TO_PNG

char* captureBuffer = NULL;

void CaptureFrame()
{

	// Resolve GPU backbuffer to our intermediate CPU texture
	Pimp::gD3D->CopyTextures(recordToPNGCaptureBuffer, Pimp::gD3D->GetRenderTargetBackBuffer()->GetTexture());

	// Map to memory so we can read it
	D3D10_MAPPED_TEXTURE2D mappedTexture;
	D3D_VERIFY( 
		recordToPNGCaptureBuffer->Map(0, D3D10_MAP_READ, 0, &mappedTexture),
		"ID3D10Texture2D:Map" );

	int w,h;
	Pimp::gD3D->GetViewportSize(&w, &h);

	char filename[512];
	sprintf_s(filename,512, "DemoCapture_%05d.tga", recordFrameIndex);

	ASSERT(mappedTexture.RowPitch == w*4);

	if (captureBuffer == NULL)
		captureBuffer = new char[w*h*4];

	// Copy a row at a time.
	// The jump by pitch, may differ if pitch is not the same as width.
	//
	unsigned int myLineSize = w * 4;
	unsigned int offsetMyData = (h-1) * myLineSize;
	char* pixelSource = (char*)mappedTexture.pData;
	char* pixelDest = (char*)captureBuffer + offsetMyData;

	for ( int i=0 ; i < h; i++ )
	{
		memcpy(pixelDest, pixelSource, myLineSize);
		pixelDest -= myLineSize;
		pixelSource += mappedTexture.RowPitch;
	}

	StoreTGAImageToFile(filename, w, h, (unsigned char*)captureBuffer);


	const int bytesPerPixel = 4;



	recordToPNGCaptureBuffer->Unmap(0);


	recordFrameIndex++;
}
#endif