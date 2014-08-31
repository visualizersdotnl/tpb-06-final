#pragma once


// Note: Also modify the config dialog's caption in Player.rc!
#define PIMPPLAYER_WINDOWCAPTION "Untitled demo"

// Do you want to see the config dialog? Set to 1 for the final build.
#define PIMPPLAYER_USECONFIGDIALOG 0

// Settings when not using the config dialog (see above setting)
#define PIMPPLAYER_FORCEDRESOLUTION_X 1280
#define PIMPPLAYER_FORCEDRESOLUTION_Y 720
#define PIMPPLAYER_FORCEDRESOLUTION_FULLSCREEN 0
#define PIMPPLAYER_FORCEDRESOLUTION_ASPECT 16.0f/9.0f

// Duration in seconds, after which we auto terminate.
#define PIMPPLAYER_DURATION 200.0f

// Should our shaders be compiled in multiple threads? Speeds things up considerably.
#define PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE 1

// Should we start playback at a specific time? (in seconds)
#define PIMPPLAYER_FORCEDSTARTTIME 0.0f

// Should we try to sync the visuals to the soundtrack's playback time?
#define PIMPPLAYER_ENABLE_HARDSYNC 0

// Record the demo to png's at a fixed framerate?
#define PIMPPLAYER_RECORD_TO_PNG 0
#define PIMPPLAYER_RECORD_FRAMERATE 30

