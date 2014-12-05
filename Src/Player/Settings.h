
#pragma once

// Demo ID & title (06).
#define PIMPPLAYER_RELEASE_ID "TPB-06"
#define PIMPPLAYER_RELEASE_TITLE "Excessination"

// Demo ID & title (07).
// #define PIMPPLAYER_RELEASE_ID "TPB-07"
// #define PIMPPLAYER_RELEASE_TITLE "A backhand for Sally"

// Aspect ratio the demo is to be presented in.
// For a fine selection of modes to choose from: https://github.com/visualizersdotnl/misc./blob/master/aspectratios.h
#define PIMPPLAYER_RENDER_ASPECT_RATIO 16.f/9.f

// Super Panavision 70
// #define PIMPPLAYER_RENDER_ASPECT_RATIO 2.2f

// Virtual resolution for sprite rendering (should match PIMPPLAYER_ASPECT_RATIO).
const float PIMPPLAYER_SPRITE_RES_X = 1920.f;
const float PIMPPLAYER_SPRITE_RES_Y = 1080.f;

// Windowed (dev. only) settings.
const bool PIMPPLAYER_WINDOWED_DEV = false;

// Development 720p.
const unsigned int PIMPPLAYER_WINDOWED_RES_X = 1280;
const unsigned int PIMPPLAYER_WINDOWED_RES_Y = 720;

// Use these to test letterboxing in windowed mode.
// const unsigned int PIMPPLAYER_WINDOWED_RES_X = 1280; // Letter.
// const unsigned int PIMPPLAYER_WINDOWED_RES_Y = 1024;
// const unsigned int PIMPPLAYER_WINDOWED_RES_X = 640; // Letter.
// const unsigned int PIMPPLAYER_WINDOWED_RES_Y = 480;
// const unsigned int PIMPPLAYER_WINDOWED_RES_X = 800; // Pillar.
// const unsigned int PIMPPLAYER_WINDOWED_RES_Y = 400;

// Shader binaries (with a 'b' appended to extension) are dumped on initial load.
// By enabling this boolean you can use them at runtime for public release
const bool PIMPPLAYER_RUN_FROM_SHADER_BINARIES = true;

// Audio settings.
const std::string PIMPPLAYER_MP3_PATH = "ROSS2.mp3"; // Relative to asset root (see Demo.cpp).
const bool PIMPPLAYER_MUTE_AUDIO = false;

// Audio sync. settings.
const double PIMPPLAYER_ROCKET_BPM = 127.0;
const int PIMPPLAYER_ROCKET_RPB = 16; // Rows per beat, or: Rocket tracker precision.
