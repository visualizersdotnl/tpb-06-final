#pragma once

// Demo ID & title.
#define PIMPPLAYER_RELEASE_ID "TPB-06"
#define PIMPPLAYER_RELEASE_TITLE "Excessination"

// Aspect ratio the demo is to be presented in.
// For a fine selection of modes to choose from: https://github.com/visualizersdotnl/misc./blob/master/aspectratios.h
#define PIMPPLAYER_RENDER_ASPECT_RATIO 16.f/9.f

// Windowed (dev. only) settings.
const bool PIMPPLAYER_WINDOWED_DEV = true;
const unsigned int PIMPPLAYER_WINDOWED_RES_X = 1920/4;
const unsigned int PIMPPLAYER_WINDOWED_RES_Y = 1080/4;

// Shader binaries (with a 'b' appended to extension) are dumped on initial load.
// By enabling this boolean you can use them at runtime for public release.
const bool PIMPPLAYER_RUN_FROM_SHADER_BINARIES = false;

// Audio settings.
const std::string PIMPPLAYER_MP3_PATH = "ROSS2.mp3"; // Relative to asset root (see Demo.cpp).
const bool PIMPPLAYER_MUTE_AUDIO = false;

