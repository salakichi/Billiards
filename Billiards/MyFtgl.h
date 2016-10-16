#pragma once

// FTGL
// version 2.1.3~rc5
// https://sourceforge.net/projects/ftgl/files/

// The Free Type Library
// version 2.7
// https://sourceforge.net/projects/freetype/files/


#include "FTGL/ftgl.h"

#if _DEBUG
	#pragma comment(lib, "freetype27MTd.lib")
	#pragma comment(lib, "ftgl_D.lib")
#else
	#pragma comment(lib, "freetype27MT.lib")
	#pragma comment(lib, "ftgl.lib")
#endif