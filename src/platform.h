#ifndef PLATFORM_H
#define PLATFORM_H

#include <final_platform_layer.h>

#ifdef _DEBUG
#define RONA_ASSERT(exp) fplAssert(exp)
#else
#define RONA_ASSERT(exp)
#endif

#define RONA_ERROR fplConsoleFormatError
#define RONA_INFO fplConsoleFormatOut

#define RONA_OUT fplConsoleOut


#endif
