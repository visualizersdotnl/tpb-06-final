#pragma once

// Undef. to resort to 64K cutbacks (i.e. no error messages, not releasing objects, no BASS/Rocket, et cetera).
#define PIMPPLAYER_IS_DEMO

#define PIMPPLAYER_RELEASE_ID "TPB-06"
#define PIMPPLAYER_RELEASE_TITLE "Excessination"

// Do you want to see the config dialog? Set to 1 for the final build.
#define PIMPPLAYER_USECONFIGDIALOG 0

// Settings when not using the config dialog (see above setting)
// @plek: I'm using a slow-ass laptop these days so I'll have to settle for "a little less" in dev. mode.
#define PIMPPLAYER_FORCEDRESOLUTION_X (1280/2)
#define PIMPPLAYER_FORCEDRESOLUTION_Y (720/2)
#define PIMPPLAYER_FORCEDRESOLUTION_FULLSCREEN 0
#define PIMPPLAYER_FORCEDRESOLUTION_ASPECT 16.0f/9.0f

// Should our shaders be compiled in multiple threads? Speeds things up considerably.
#define PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE 1
