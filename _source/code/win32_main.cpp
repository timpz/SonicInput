#include "win32_main.h"
#include "app.h"
#include "settings.h"

#include "..\formatted_data\assets.h"

global bool32 GlobalRunning;

#include "app.cpp"

local_persist win32_offscreen_buffer *Win32BackBuffer;
local_persist keyboard_input Win32Keyboard[2];

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
	if(Memory)
	{
		VirtualFree(Memory, 0, MEM_RELEASE);
	}
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
	debug_read_file_result Result = {};
	HANDLE FileHandle = CreateFileA
		(
			Filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			0,
			0
		);

	if(FileHandle)
	{
		LARGE_INTEGER FileSize;
		if(GetFileSizeEx(FileHandle, &FileSize))
		{
			uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
			Result.Content = VirtualAlloc(0, FileSize32, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
			if(Result.Content)
			{
				DWORD BytesRead = {};
				//NOTE: Set address in the function instead?
				if(ReadFile(FileHandle, Result.Content, FileSize32, &BytesRead, 0) && (FileSize32 == BytesRead))
				{
					Result.ContentSize = FileSize32;
				} else
				{
					//TODO: Logging
					DEBUGPlatformFreeFileMemory(Result.Content);
					Result.Content = 0;
				}

			} else
			{
				//TODO: Logging
			}
		} else
		{
			//TODO: Logging
		}

		CloseHandle(FileHandle);

	} else
	{
		//TODO: Logging
	}

	return(Result);
}

internal uint32 StringLength(char *String)
{
	uint32 Result = 0;

	for(uint32 i = 0;; i++)
	{
		if(String[i] == 0)
		{
			break;
		}

		Result++;
	}

	return Result;
}

internal void ConcatStrings(char *A, char *B, char *Dest)
{

	uint32 LengthA = StringLength(A);
	uint32 LengthB = StringLength(B);

	for(uint32 i = 0; i < LengthA; i++)
	{
		Dest[i] = A[i];
	}

	for(uint32 i = 0; i < LengthB; i++)
	{
		Dest[i + LengthA] = B[i];
	}
}

internal void CopyString(char *From, char *To)
{
	uint32 LengthFrom = StringLength(From);
	uint32 LengthTo = StringLength(To);

	// Assert(LengthFrom <= LengthTo);

	for(uint32 i = 0; i < LengthFrom; i++)
	{
		To[i] = From[i];
	}
}

internal void CopyString(char *From, char *To, char EndCharacter)
{
	uint32 LengthFrom = StringLength(From);
	uint32 LengthTo = StringLength(To);

	// Assert(LengthFrom <= LengthTo);

	for(uint32 i = 0; i < LengthFrom; i++)
	{
		if(From[i] == EndCharacter){ break; }
		To[i] = From[i];
	}
}

internal void ConvertValueToHex(uint8 Value, char *String)
{
	uint8 HighByte = Value & 0xF0;
	HighByte = HighByte >> 4;
	uint8 LowByte = Value & 0x0F;

	char Result[2] = {};

	if(HighByte < 0xA)
	{
		Result[0] = HighByte + 0x30;
	} else
	{
		Result[0] = HighByte + 0x37;
	}

	if(LowByte < 0xA)
	{
		Result[1] = LowByte + 0x30;
	} else
	{
		Result[1] = LowByte + 0x37;
	}

	CopyString(Result, String);
}

DEBUG_PLATFORM_WRITE_FILE_MEMORY(DEBUGPlatformWriteFileMemory)
{

	bool32 Result = false;

	HANDLE FileHandle = CreateFileA
		(
			FileName,
			GENERIC_WRITE,
			0,
			0,
			CREATE_ALWAYS,
			0,
			0
		);
	
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD BytesWritten = {};

		char CodeStart[64] = {};
		char CopyBuffer[64] = {};

		char FileNameWithoutType[64] = {};
		CopyString(FileName, FileNameWithoutType, '.');

		char *BeginningOfFile = "static uint8_t ";
		ConcatStrings(BeginningOfFile, FileNameWithoutType, CopyBuffer);
		char *BeginningOfFile2 = "[] =\n{\n\t";
		
		ConcatStrings(CopyBuffer, BeginningOfFile2, CodeStart);
		// uint32 CodeStartLength = StringLength(CodeStart); 

		char *CodeEnd = "\n};";
		// uint32 CodeEndLength = StringLength(CodeEnd);

		char *ValuePrefix = "0x";
		// uint32 PrefixLength = StringLength(ValuePrefix);

		// uint8 Data = 0x1;

		uint8 *Data = (uint8 *)Memory;


		WriteFile(FileHandle, CodeStart, StringLength(CodeStart), &BytesWritten, 0);

		for(uint32 CurrentByte = 0; CurrentByte < FileSize; CurrentByte++)
		{

			char DataString[64] = {};
			ConvertValueToHex(*Data++, DataString);

			char ConcatenatedData[64] = {};
			ConcatStrings(ValuePrefix, DataString, ConcatenatedData);
			uint32 DataStringLength = StringLength(ConcatenatedData);

			WriteFile(FileHandle, ConcatenatedData, DataStringLength, &BytesWritten, 0);

			if(CurrentByte < FileSize - 1)
			{
				char *StringBetweenData = ", ";
				
				WriteFile(FileHandle, StringBetweenData, StringLength(StringBetweenData), &BytesWritten, 0);

				if(((CurrentByte + 1) % 8) == 0 && CurrentByte != 1)
				{
					char *NewLine = "\n\t";
					WriteFile(FileHandle, NewLine, StringLength(NewLine), &BytesWritten, 0);
				}
			}
		}

		

		WriteFile(FileHandle, CodeEnd, StringLength(CodeEnd), &BytesWritten, 0);

		// WriteFile(FileHandle, Memory, sizeof(image)-8, &BytesWritten, 0);

		// image *Image = (image *)Memory;
		// WriteFile(FileHandle, Image->Data, Image->Header.Width * Image->Header.Height, &BytesWritten, 0);


		//NOTE: File read successfully)
		// Result = (BytesWritten == Filesize);


		CloseHandle(FileHandle);

	} else
	{
		//TODO: Logging
	}

	return(Result);
}


internal void Win32ResizeDIB
(
	win32_offscreen_buffer *Buffer, 
	int32 Width, int32 Height
)
{
	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	// NOTE: biHeight < 0 means origin is at bottom left
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int32 BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void Win32DisplayBufferInWindow
(
	win32_offscreen_buffer *Buffer,
	HDC DeviceContext,
	int32 WindowWidth, int32 WindowHeight
)
{
	StretchDIBits
	(
		DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

inline void Win32ProcessKeyboardMessage(app_button_state *NewState, bool32 IsDown)
{
		NewState->IsDown = IsDown;
}

internal void Win32ProcessPendingMessages(keyboard_input *Keyboard)
{
	MSG Message;
	while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch(Message.message)
		{
			case WM_QUIT:
			{
				GlobalRunning = false;
			} break;

			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				uint32 VKCode = (uint32)Message.wParam;
				bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
				bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);
				if(IsDown != WasDown)
				{
					if(VKCode == 'W')
					{
						Win32ProcessKeyboardMessage(&Keyboard->W, IsDown);
					}

					if(VKCode == 'A')
					{
						Win32ProcessKeyboardMessage(&Keyboard->A, IsDown);
					}

					if(VKCode == 'S')
					{
						Win32ProcessKeyboardMessage(&Keyboard->S, IsDown);
					}

					if(VKCode == 'D')
					{
						Win32ProcessKeyboardMessage(&Keyboard->D, IsDown);
					}

					if(VKCode == 'J')
					{
						Win32ProcessKeyboardMessage(&Keyboard->J, IsDown);
					}

					if(VKCode == 'K')
					{
						Win32ProcessKeyboardMessage(&Keyboard->K, IsDown);
					}

					if(VKCode == 'L')
					{
						Win32ProcessKeyboardMessage(&Keyboard->L, IsDown);
					}

					if(VKCode == VK_RETURN)
					{
						Win32ProcessKeyboardMessage(&Keyboard->Enter, IsDown);
					}





					if(VKCode == VK_ESCAPE)
					{
						Win32ProcessKeyboardMessage(&Keyboard->ExitApp, IsDown);
					}

					bool32 AltKeyWasDown = Message.lParam & (1 << 29);
					if((VKCode == VK_F4) && AltKeyWasDown) 
					{
						GlobalRunning = false;
					}
				}
			} break;

			default:
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;
		}
	}
}

internal LRESULT CALLBACK Win32MainWindowCallback
(
	HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam
)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{

		} break;

		case WM_DESTROY:
		{
			GlobalRunning = false;
		} break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{	
			Assert(!"Keyboard input came in through a non-dispatched message!");
		} break;

		case WM_CLOSE:
		{
			GlobalRunning = false;
		} break;

		case WM_ACTIVATEAPP:
		{
			// Unfocusing the app could make the HeldDownCount behave badly. 
			// For this reason we always clear inputs to 0 when coming back into focus
			Win32Keyboard[0] = {};
			Win32Keyboard[1] = {};
		} break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			Win32DisplayBufferInWindow
			(
				Win32BackBuffer, DeviceContext,
				WINDOW_WIDTH, WINDOW_HEIGHT
			);
		} break;
		
		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}

	return Result;
}

inline int64 ReturnQPC()
{
	LARGE_INTEGER Query = {};
	QueryPerformanceCounter(&Query);
	int64 Result = (int64)Query.QuadPart;
	return Result;
}

inline float64 QPCElapsedInMS(int64 Start, int64 End, int64 Frequency)
{
	float64 DeltaCounts = (float64)End - (float64)Start;
	float64 CountsPerMilliSecond = (float64)Frequency / 1000.0f;
	float64 MilliSecondsElapsed = DeltaCounts / CountsPerMilliSecond;
	return MilliSecondsElapsed;
}

int32 CALLBACK WinMain
(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int32 ShowCode
)
{
	GlobalRunning = false;

	//NOTE: Set the Windows Scheduler granularity to 1ms
	UINT DesiredSchedulerMilliSecond = 1;
	bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMilliSecond) == TIMERR_NOERROR);

	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_VREDRAW|CS_HREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
	WindowClass.lpszClassName = APP_NAME;

	if(RegisterClassA(&WindowClass))
	{
		uint32 WindowStyle = WS_VISIBLE; // Make the window visible
		WindowStyle |= WS_CAPTION; // Title bar
		WindowStyle |= WS_SYSMENU; // Enables the close and minimize buttons
		WindowStyle |= WS_MINIMIZEBOX; // Minimize button

		uint32 WindowStyleEx = WS_EX_TOPMOST; // Always on-top

		RECT WindowRect = {};

		WindowRect.left = 0;
		WindowRect.top = 0;
		WindowRect.right = WINDOW_WIDTH;
		WindowRect.bottom = WINDOW_HEIGHT;	

		bool32 WindowMenu = false; // Window has no menu

		AdjustWindowRectEx(&WindowRect, WindowStyle, WindowMenu, WindowStyleEx);

		uint32 WindowWidth = WindowRect.right - WindowRect.left;
		uint32 WindowHeight = WindowRect.bottom - WindowRect.top;

		HWND Window = CreateWindowExA
		(
			0, 
			WindowClass.lpszClassName, 
			APP_NAME, 
			WindowStyle, 
			WINDOW_POSX, 
			WINDOW_POSY, 
			WindowWidth, 
			WindowHeight,
			0, 
			0, 
			Instance, 
			0
		);

		if(Window)
		{
			HDC DeviceContext = GetDC(Window);
			GlobalRunning = true;

			win32_offscreen_buffer BackBuffer = {};
			Win32BackBuffer = &BackBuffer;
			Win32ResizeDIB(Win32BackBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

			offscreen_buffer AppBuffer = {};
			AppBuffer.BytesPerPixel = Win32BackBuffer->BytesPerPixel;
			AppBuffer.Height = Win32BackBuffer->Height;
			AppBuffer.Width = Win32BackBuffer->Width;
			AppBuffer.Pitch = Win32BackBuffer->Pitch;
			AppBuffer.Memory = Win32BackBuffer->Memory;

			Win32Keyboard[0] = {};
			Win32Keyboard[1] = {};

			keyboard_input *NewKeyboard = &Win32Keyboard[0];
			keyboard_input *OldKeyboard = &Win32Keyboard[1];

		
#if BUILD_INTERNAL
			LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
			LPVOID BaseAddress = 0;
#endif

			app_memory AppMemory = {};
			AppMemory.PermanentStorageSize = Megabytes(64);
			AppMemory.TransientStorageSize = Megabytes(0);

			AppMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
			AppMemory.DEBUGPlatformReadEntireFile= DEBUGPlatformReadEntireFile;
			AppMemory.DEBUGPlatformWriteFileMemory = DEBUGPlatformWriteFileMemory;

			uint64 TotalSize = AppMemory.PermanentStorageSize + AppMemory.TransientStorageSize;

			AppMemory.PermanentStorage = VirtualAlloc(BaseAddress, (size_t)TotalSize,
											MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			AppMemory.TransientStorage = ((uint8 *)AppMemory.PermanentStorage + 
													AppMemory.PermanentStorageSize);


			Initialise(&AppMemory);

			LARGE_INTEGER _QPF;
			QueryPerformanceFrequency(&_QPF);
			int64 QPF = _QPF.QuadPart;
			float64 TargetFrameTime = 1000.0f / TARGET_FRAMERATE;
			int64  DesiredPerformanceCounts = (int64)(QPF / 1000.0f * TargetFrameTime);
			
			int64 FramerateCounterStart = ReturnQPC();

			while(GlobalRunning)
			{
				Win32ProcessPendingMessages(NewKeyboard);

				Update(NewKeyboard, &AppMemory);
				Render(&AppBuffer, &AppMemory);

				Win32DisplayBufferInWindow
				(
					Win32BackBuffer, DeviceContext,
					WINDOW_WIDTH, WINDOW_HEIGHT
				);

				keyboard_input *TempKeyboardPointer = NewKeyboard;
				NewKeyboard = OldKeyboard;
				OldKeyboard = TempKeyboardPointer;

				*NewKeyboard = {};
				for(int32 ButtonIndex = 0; ButtonIndex < ArrayCount(NewKeyboard->Buttons); ButtonIndex++)
				{
					if(OldKeyboard->Buttons[ButtonIndex].IsDown)
					{
						NewKeyboard->Buttons[ButtonIndex].IsDown = true;
						NewKeyboard->Buttons[ButtonIndex].HeldDownCount = 
							++OldKeyboard->Buttons[ButtonIndex].HeldDownCount;
					}
				}

				// Enforce Framerate
				int64 FramerateCounterEnd = ReturnQPC();

				int64 ElapsedCounts = FramerateCounterEnd - FramerateCounterStart;
				int64 CountsToWait = DesiredPerformanceCounts - ElapsedCounts;

				while(CountsToWait > 0)
				{
					float64 ElapsedMilliSeconds = 
						QPCElapsedInMS(FramerateCounterStart, FramerateCounterEnd, QPF);
					
					DWORD MilliSecondsToWait = (DWORD)(TargetFrameTime - ElapsedMilliSeconds);
					if(MilliSecondsToWait > 1)
					{
						Sleep(MilliSecondsToWait);
					}

					FramerateCounterEnd = ReturnQPC();
					ElapsedCounts = FramerateCounterEnd - FramerateCounterStart;
					CountsToWait = DesiredPerformanceCounts - ElapsedCounts;
				}

				FramerateCounterStart = ReturnQPC() + CountsToWait;
			}
		}
	}

	return (0);
}
