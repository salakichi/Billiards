#pragma once

// GLEW
// version 2.0.0
// http://glew.sourceforge.net/

#define GLEW_STATIC
#include "GL/glew.h"

#if _DEBUG
	#pragma comment(lib, "glew32sd.lib")
#else
	#pragma comment(lib, "glew32s.lib")
#endif 