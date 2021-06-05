#include "win32_main.h"
#include "app.h"
#include "settings.h"

#include "..\formatted_data\assets.h"

global bool32 GlobalRunning;

#include "app.cpp"

global win32_offscreen_buffer *Win32GlobalBackBuffer;
global input DeviceInputs[2];
global GUID GlobalControllerGUID;

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

internal void Win32ProcessPendingMessages(game_input *Keyboard)
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
						Win32ProcessKeyboardMessage(&Keyboard->MoveUp, IsDown);
					}

					if(VKCode == 'A')
					{
						Win32ProcessKeyboardMessage(&Keyboard->MoveLeft, IsDown);
					}

					if(VKCode == 'S')
					{
						Win32ProcessKeyboardMessage(&Keyboard->MoveDown, IsDown);
					}

					if(VKCode == 'D')
					{
						Win32ProcessKeyboardMessage(&Keyboard->MoveRight, IsDown);
					}

					if(VKCode == 'J')
					{
						Win32ProcessKeyboardMessage(&Keyboard->ActionA, IsDown);
					}

					if(VKCode == 'K')
					{
						Win32ProcessKeyboardMessage(&Keyboard->ActionB, IsDown);
					}

					if(VKCode == 'L')
					{
						Win32ProcessKeyboardMessage(&Keyboard->ActionC, IsDown);
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

		// case WM_ACTIVATEAPP:
		// {
			// Unfocusing the app could make the HeldDownCount behave badly. 
			// For this reason we always clear inputs to 0 when coming back into focus
			// DeviceInputs[0].DInputController = {};
			// DeviceInputs[1].DInputController = {};
		// } break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			Win32DisplayBufferInWindow
			(
				Win32GlobalBackBuffer, DeviceContext,
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


internal BOOL DirectInputEnumCallback(LPCDIDEVICEINSTANCE DirectInputPointer, LPVOID PointerValue)
{
	bool32 *Switcher = (bool32 *)PointerValue;
	if(*Switcher == false)
	{
		GlobalControllerGUID = DirectInputPointer->guidInstance;
		*Switcher = true;
	}
	return true;
}

internal void ProcessDInput(LPDIRECTINPUTDEVICE8 DInputDevice, game_input *DInputController)
{
	DIJOYSTATE DIState = {};

	if(DInputDevice != 0)
	{
		DInputDevice->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&DIState);
		game_input ZeroDIController = {};

		*DInputController = {};

		uint32 DIPOV = DIState.rgdwPOV[0]/4500;

		// DInputController->AverageX = 0.0f;
		if(DIPOV > 0 && DIPOV <= 3)
		{
			DInputController->MoveRight.IsDown = true;
		}
		else if(DIPOV > 4 && DIPOV <= 7)
		{
			DInputController->MoveLeft.IsDown = true;
		}

		// NewDIController->AverageY = 0.0f;
		if((DIPOV < 2 || DIPOV > 6) && DIPOV < 8)
		{
			DInputController->MoveUp.IsDown = true;
		}
		else if(DIPOV > 2 && DIPOV <= 5)
		{
			DInputController->MoveDown.IsDown = true;
		}

		DInputController->ActionA.IsDown = (bool32)DIState.rgbButtons[0];
		DInputController->ActionB.IsDown = (bool32)DIState.rgbButtons[1];
		DInputController->ActionC.IsDown = (bool32)DIState.rgbButtons[2];

		DInputController->Enter.IsDown = (bool32)DIState.rgbButtons[5];
	}
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
			WS_EX_LAYERED, 
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

			COLORREF rgbBlack =  0x00000000;

			SetLayeredWindowAttributes
			(
				Window, 
				rgbBlack,
				0xFF,
				LWA_COLORKEY
			);

			// BOOL SetLayeredWindowAttributes(
			// HWND     hwnd,
			// COLORREF crKey,
			// BYTE     bAlpha,
			// DWORD    dwFlags
			// );

			win32_offscreen_buffer Win32BackgroundBuffer = {};
			win32_offscreen_buffer Win32ForegroundBuffer = {};

			Win32GlobalBackBuffer = &Win32BackgroundBuffer;

			Win32ResizeDIB(&Win32BackgroundBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);
			Win32ResizeDIB(&Win32ForegroundBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

			offscreen_buffer BackgroundBuffer = {};
			BackgroundBuffer.BytesPerPixel = Win32BackgroundBuffer.BytesPerPixel;
			BackgroundBuffer.Height = Win32BackgroundBuffer.Height;
			BackgroundBuffer.Width = Win32BackgroundBuffer.Width;
			BackgroundBuffer.Pitch = Win32BackgroundBuffer.Pitch;
			BackgroundBuffer.StartOfBuffer = Win32BackgroundBuffer.Memory;

			// This is only safe because the foreground and background are identical size
			offscreen_buffer ForegroundBuffer = BackgroundBuffer;
			ForegroundBuffer.StartOfBuffer = Win32ForegroundBuffer.Memory;

			render_layers RenderLayers = {};
			RenderLayers.NumberOfBuffers = 2;
			RenderLayers.BackBuffer[0] = &BackgroundBuffer;
			RenderLayers.BackBuffer[1] = &ForegroundBuffer;
			
			// AppBuffer.ForegroundLayer = ForegroundBuffer.Memory;


			input *NewInput = &DeviceInputs[0];
			input *OldInput = &DeviceInputs[1];

			*NewInput = {};
			*OldInput = {};

			DeviceInputs[0].SelectedDevice = INPUT_DEVICE;
			DeviceInputs[1].SelectedDevice = INPUT_DEVICE;

			game_input *KeyboardController = &NewInput->KeyboardController;
			game_input *DInputController = &NewInput->DInputController;

			LPDIRECTINPUT8 DInputPointer = 0;
			LPDIRECTINPUTDEVICE8 DInputDevice = 0;

			if(DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&DInputPointer, NULL) == DI_OK)
			{

				bool32 EnumValue = 0;
				DInputPointer->EnumDevices(DI8DEVCLASS_GAMECTRL, DirectInputEnumCallback, (LPVOID)&EnumValue, DIEDFL_ATTACHEDONLY);

				DInputPointer->CreateDevice(GlobalControllerGUID, &DInputDevice, NULL);
				if(DInputDevice != 0)
				{
					DInputDevice->SetDataFormat(&c_dfDIJoystick);
					DInputDevice->Acquire();
				}
			}

#if BUILD_INTERNAL
			LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
			LPVOID BaseAddress = 0;
#endif

			app_memory AppMemory = {};
			AppMemory.PermanentStorageSize = Megabytes(64);
			AppMemory.TransientStorageSize = Megabytes(0);

			// AppMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
			// AppMemory.DEBUGPlatformReadEntireFile= DEBUGPlatformReadEntireFile;
			// AppMemory.DEBUGPlatformWriteFileMemory = DEBUGPlatformWriteFileMemory;

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
				Win32ProcessPendingMessages(KeyboardController);
				ProcessDInput(DInputDevice, DInputController);

				Update(DeviceInputs, &AppMemory);
				Render(&RenderLayers, &AppMemory);

				Win32DisplayBufferInWindow
				(
					&Win32BackgroundBuffer, DeviceContext,
					WINDOW_WIDTH, WINDOW_HEIGHT
				);

				for(int32 ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput->KeyboardController.Buttons); ButtonIndex++)
				{
					if(OldInput->KeyboardController.Buttons[ButtonIndex].IsDown)
					{
						NewInput->KeyboardController.Buttons[ButtonIndex].IsDown = true;
						NewInput->KeyboardController.Buttons[ButtonIndex].HeldDownCount = 
							++OldInput->KeyboardController.Buttons[ButtonIndex].HeldDownCount;
					}
				}

				input *TempDeviceInput = NewInput;
				OldInput = NewInput;
				OldInput = TempDeviceInput;

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
