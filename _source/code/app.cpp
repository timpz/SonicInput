#include "app.h"
#include "app_math.h"
// #include "app_math.cpp"
#include "ShittyDrawingAPI.cpp"

local_persist bool32 TestButton = false;

internal void ProcessInput(input *DeviceInputs, app_state *State)
{
	display_input *DisplayInput = &State->DisplayInput;

	game_input *SelectedController = &DeviceInputs->Device[DeviceInputs->SelectedDevice];

	if(DeviceInputs->KeyboardController.ExitApp.IsDown != false)
	{
		GlobalRunning = false;
	}

	if(SelectedController->Enter.IsDown)
	{ DisplayInput->Start = true; }
	else { DisplayInput->Start = false; }

	if(SelectedController->ActionA.IsDown){ DisplayInput->A = true; }
	else { DisplayInput->A = false; }

	if(SelectedController->ActionB.IsDown){ DisplayInput->B = true; }
	else { DisplayInput->B = false; }

	if(SelectedController->ActionC.IsDown){ DisplayInput->C = true; }
	else { DisplayInput->C = false; }

	uint32 DpadValue = 0;
	int32 LeftRight = 0;
	int32 UpDown = 0;

	if(SelectedController->MoveLeft.IsDown && !SelectedController->MoveRight.IsDown)
	{
		LeftRight = -1;
	} else if(SelectedController->MoveDown.IsDown && !SelectedController->MoveLeft.IsDown)
	{
		LeftRight = 1;
	}

	if(SelectedController->MoveUp.IsDown && !SelectedController->MoveDown.IsDown)
	{
		UpDown = 1;
	} else if(SelectedController->MoveDown.IsDown && !SelectedController->MoveUp.IsDown)
	{
		UpDown = -1;
	}

	if(LeftRight == -1 && UpDown == 0){ DpadValue = 1; } else
	if(LeftRight == -1 && UpDown == 1){ DpadValue = 2; } else
	if(LeftRight == 0 && UpDown == 1){ DpadValue = 3; } else
	if(LeftRight == 1 && UpDown == 1){ DpadValue = 4; } else
	if(LeftRight == 1 && UpDown == 0){ DpadValue = 5; } else
	if(LeftRight == 1 && UpDown == -1){ DpadValue = 6; } else
	if(LeftRight == 0 && UpDown == -1){ DpadValue = 7; } else
	if(LeftRight == -1 && UpDown == -1){ DpadValue = 8; }
	

	if(SelectedController->MoveLeft.IsDown && !SelectedController->MoveRight.IsDown)
	{
		if(SelectedController->MoveUp.IsDown)
		{
			DpadValue = 2; 
		} else if(SelectedController->MoveDown.IsDown)
		{
			DpadValue = 8;
		} else
		{
			DpadValue = 1;
		}
	} else if(SelectedController->MoveRight.IsDown && !SelectedController->MoveLeft.IsDown)
	{
		if(SelectedController->MoveUp.IsDown)
		{
			DpadValue = 4; 
		} else if(SelectedController->MoveDown.IsDown)
		{
			DpadValue = 6;
		} else
		{
			DpadValue = 5;
		}
	} else if(SelectedController->MoveUp.IsDown && !SelectedController->MoveDown.IsDown)
	{
		DpadValue = 3;
	} else if(SelectedController->MoveDown.IsDown && !SelectedController->MoveUp.IsDown)
	{
		DpadValue = 7;
	}

	DisplayInput->Dpad = DpadValue;
}

inline uint32 FindLeastSignificantBit32(uint32 Number)
{
	uint32 Result = 0;

#if COMPILER_MSVC
	_BitScanForward((unsigned long *)&Result, Number);
#else
	uint32 Test = 1;

	for(uint32 Tries = 0; Tries < 32; Tries++)
	{
		if((Number & Test) == 0)
		{
			Test = Test << 1;
			Result++;
		} else
		{
			break;
		}
	}
#endif

	return Result;
}

inline uint32 FindLeastSignificantByte32(uint32 Number)
{
	uint32 Result = FindLeastSignificantBit32(Number);

	if(Result < 8)
	{
		Result = 0;
	} 
	else if(Result < 16)
	{
		Result = 1;
	}
	else if(Result < 24)
	{
		Result = 2;
	}
	else
	{
		Result = 3;
	}

	return Result;

}

internal void DEBUGLoadBMP(debug_platform_read_entire_file *ReadEntireFile, char *FileName, void *Memory)
{
	debug_read_file_result Contents = ReadEntireFile(FileName);
	bmp_image BMPFile = {};
	// uint32 FileSize = 0;
	// int32 PixelIndex_;

	if(Contents.Content)
	{
		bmp_header *Header = (bmp_header *)(Contents.Content);
		
		Assert(Header->BM == 0x4D42);
		Assert(Header->Reserved1 == 0);
		Assert(Header->Reserved2 == 0);
		Assert(Header->Planes == 1);
		Assert(Header->Compression == BI_RGB);

		BMPFile.Header = *(bmp_header *)(Contents.Content);
		BMPFile.PixelColourData = (uint8 *)Contents.Content + Header->ImageOffset;

		uint8 *Pixel = BMPFile.PixelColourData;

		uint32 *ImageHeader = (uint32 *)Memory;


		*ImageHeader++ = Header->Width;
		*ImageHeader++ = Header->Height;
		// FileSize = 
		*ImageHeader++ = Header->Width * Header->Height + sizeof(uint32)*4;
		*ImageHeader++ = 0xCCCCCCCC;

		uint8 *ImageData = (uint8 *)ImageHeader;
		

		for(int32 PixelIndex = 0; PixelIndex < Header->Width * Header->Height; PixelIndex++)
		{

			ImageData[PixelIndex] = *Pixel;
			Pixel += 3;
		}
	}
}



internal void Initialise(app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;
}


internal void Update(input *DeviceInputs, app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;

	ProcessInput(DeviceInputs, AppState);

}

internal void DrawButton
(
	bool32 Pressed, float32 PosX, float32 PosY, 
	uint8 *ButtonPressed, uint8 *ButtonNotPressed,
	offscreen_buffer *Buffer
)
{
	if(Pressed)
	{
		DrawImageOnTop(Buffer, PosX, PosY, ButtonPressed);
	} else
	{
		DrawImageOnTop(Buffer, PosX, PosY, ButtonNotPressed);
	}
}

internal void DrawButtonOnTop
(
	bool32 Pressed, float32 PosX, float32 PosY, 
	uint8 *ButtonPressed, uint8 *ButtonNotPressed,
	offscreen_buffer *Buffer
)
{
	if(Pressed)
	{
		DrawImageOnTop(Buffer, PosX, PosY, ButtonPressed);
	} else
	{
		DrawImageOnTop(Buffer, PosX, PosY, ButtonNotPressed);
	}
}

internal void DrawDirection
(
	uint32 Direction, float32 PosX, float32 PosY, 
	offscreen_buffer *Buffer
)
{
	switch(Direction)
	{
		case 1:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_1);
		} break;
		case 2:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_2);
		} break;
		case 3:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_3);
		} break;
		case 4:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_4);
		} break;
		case 5:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_5);
		} break;
		case 6:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_6);
		} break;
		case 7:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_7);
		} break;
		case 8:
		{
			DrawImageOnTop(Buffer, PosX, PosY, Direction_8);
		} break;
		
	}
}

global float32 testvalue = 0.0f;

internal void Render(offscreen_buffer *Buffer, app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;
	display_input *DisplayInput = &AppState->DisplayInput;

	// Blank
	// DrawRectangle(Buffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 0, 0);

	// vector3 Colour = {0.5f, 0.7f, 0};
	DrawCircle(Buffer, 20, 20, 10.0f, 0.5f, 0.7f, 0.3f, 0.5f);

	// DrawImage(Buffer, 10.0f, 10.0f, Dpad_Image);
	// DrawButton(DisplayInput->A, 70.0f, 10.0f, A_Press, A_Button, Buffer);
	// DrawButton(DisplayInput->B, 130.0f, 10.0f, B_Press, B_Button, Buffer);
	// DrawButton(DisplayInput->C, 190.0f, 10.0f, C_Press, C_Button, Buffer);

	// if(DisplayInput->Start)
	// {
	// 	DrawImageOnTop(Buffer, 10.0f, 10.0f, S_Press);
	// }

	// DrawDirection(DisplayInput->Dpad, 10.0f, 10.0f, Buffer);

	// DrawRainbowHorizontal(Buffer, testvalue);
	// testvalue = ModuloFloat32(testvalue + 0.001f, 1.0f);
}