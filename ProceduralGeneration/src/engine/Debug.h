#pragma once
// Checks _DEBUG which is only on Microsoft Visual Studio c++ - just like crtdbg
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#define _INC_MALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#ifdef CFG_DEBUG
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
	#else
		#define DBG_NEW new
	#endif
	#define new DBG_NEW
#endif