#pragma once

// Demo ID & title.
#define PIMPPLAYER_RELEASE_ID "TPB-06"
#define PIMPPLAYER_RELEASE_TITLE "Excessination"

// Aspect ratio the demo is to be presented in.
// For a fine selection of modes to choose from: https://github.com/visualizersdotnl/misc./blob/master/aspectratios.h
#define PIMPPLAYER_RENDER_ASPECT_RATIO 16.f/9.f

// Should our shaders be compiled in multiple threads? Speeds things up considerably.
#define PIMPPLAYER_USEMULTITHREADED_EFFECTS_COMPILE 1

// Force windowed (uses dev. resolution) or else the desktop resolution is adapted.
// @plek: Adapting the desktop resolution makes good sense: it's usually the viewer's optimal resolution
//        without monitor distortion. And a beam team can very well be instructed to select an appropriate one
//        for performance reasons.
#define PIMPPLAYER_FORCE_WINDOWED 1
#define PIMPPLAYER_DEV_RES_X 1920/2
#define PIMPPLAYER_DEV_RES_Y 1080/2
