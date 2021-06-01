#ifndef APP_H


#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif
    
#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

#define internal static
#define global static
#define local_persist static

#if BUILD_SLOW
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)
#define Petabytes(Value) (Terabytes(Value)*1024LL)

#include <stdint.h>

typedef int32_t bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float float32;
typedef double float64;


struct debug_read_file_result
{
	uint32 ContentSize;
	void *Content;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_FILE_MEMORY(name) bool32 name(char *FileName, uint32 FileSize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_FILE_MEMORY(debug_platform_write_file_memory);

struct app_memory
{
	bool32 IsInitialized;

	uint64 PermanentStorageSize;
	void *PermanentStorage; //NOTE: REQUIRED to be cleared to zero at startup

	uint64 TransientStorageSize;
	void *TransientStorage; //NOTE: REQUIRED to be cleared to zero at startup

	debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
	debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
	debug_platform_write_file_memory *DEBUGPlatformWriteFileMemory;
};

struct offscreen_buffer
{
	void *Memory;
	int32 Width;
	int32 Height;
	int32 Pitch;
	int32 BytesPerPixel;
};

struct app_button_state
{
	int32 HeldDownCount;
	bool32 IsDown;
};


struct game_input
{
	union
	{
		app_button_state Buttons[9];
		struct
		{
			app_button_state ExitApp;

			app_button_state Enter;
			app_button_state MoveUp;
			app_button_state MoveDown;
			app_button_state MoveLeft;
			app_button_state MoveRight;

			app_button_state ActionA;
			app_button_state ActionB;
			app_button_state ActionC;
			
		};
	};
	
};

struct input
{
	int32 SelectedDevice;
	union
	{
		game_input Device[3];
		struct
		{
			game_input KeyboardController;
			game_input DInputController;
			game_input XInputController;
		};
	};
	

};

struct display_input
{
	uint32 Dpad;

	union
	{
		bool32 Buttons[4];
		struct
		{
			bool32 Start;
			bool32 A;
			bool32 B;
			bool32 C;
		};
		
	};
	
};

#pragma pack(push, 1)
struct bmp_header
{
	uint16 BM;
	uint32 Size;
	uint16 Reserved1; // must be 0
	uint16 Reserved2;	// must be 0
	uint32 ImageOffset;
	uint32 HeaderSize;
	int32 Width;
	int32 Height;
	uint16 Planes; // must be 1
	uint16 BitsPerPixel;
	uint32 Compression; // 3 = uncompressed RGB with three masks used
	uint32 ImageSize;
	int32 PixelsPerMeterX;
	int32 PixelsPerMeterY;
	uint32 ColoursUsed; // should be 0 without palettes
	uint32 ImportantColours; // also 0 if not using indexed colours
	uint32 RedMask;
	uint32 GreenMask;
	uint32 BlueMask;
	uint32 AlphaMask;
};
#pragma pack(pop)

struct bmp_image
{
	bmp_header Header;
	uint8 *PixelColourData;
};

struct image_header
{
	union
	{
		int32 HeaderData[4];
		struct
		{
			int32 Width;
			int32 Height;
			int32 FileSize;
			int32 Reserved;
		};
	};
	
};

struct image
{
	image_header Header;
	uint8 *Data;
};

struct app_state
{
	display_input DisplayInput;
};

#define APP_H
#endif