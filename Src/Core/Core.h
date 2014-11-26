
// Main include for host application.
// Include Platform.h first.

#pragma once

#if !defined(CORE_PLATFORM_H)
	#error "Include Platform.h first!"
#endif

#include "Settings.h"
#include "Configuration.h"
#include "D3D.h"

#include "World.h"

// Nodes (not implicitly included above)
#include "Xform.h"
#include "MaterialParameter.h"

// Non-node drawables
#include "Metaballs.h"
#include "Sprites.h"
