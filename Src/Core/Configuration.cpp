#include "Configuration.h"
#include "D3D.h"
#include "Settings.h"

namespace Pimp
{
	Configuration::Configuration() :
		fullscreen(false),
		renderAspectRatio(16.f/9.f) {} // Seems like a sensible choice these times we live in.
}
