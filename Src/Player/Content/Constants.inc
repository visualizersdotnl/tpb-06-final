
// Global shader parameters provided by demo.

#if !defined(DEMO_CONSTANTS)
#define DEMO_CONSTANTS

cbuffer DemoConstants
{
	// Effect timer (used for "ambient" animation like slow rotation et cetera).
	float g_fxTime;

	// 0 = Visible, 1 = White, -100 = Black
	float g_preSpriteFade; 

	// Noise opacity [0..1].
	float g_sceneNoise;

	// Noise random seed.
	float g_sceneNoiseT;
};

#endif // DEMO_CONSTANTS
