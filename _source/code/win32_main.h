#ifndef WIN32_MAIN_H

#include <windows.h>
#include <iostream>
#include "app.h"

struct win32_window_dimension
{
	int32 Width;
	int32 Height;
};

struct win32_offscreen_buffer
{
	// Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
	BITMAPINFO Info;
	void *Memory;
	int32 Width;
	int32 Height;
	int32 Pitch;
	int32 BytesPerPixel;
};

#define WIN32_MAIN_H
#endif