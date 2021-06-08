#include "app.h"
#include "app_math.h"
// #include "app_math.cpp"
#include "ShittyDrawingAPI.cpp"

local_persist bool32 TestButton = false;

internal void ProcessInput(input *DeviceInputs, app_state *State)
{
	display_input *DisplayInput = &State->DisplayInput;

	game_input *SelectedController = &DeviceInputs->Device[DeviceInputs->SelectedDevice];

	// if(DeviceInputs->SystemInput.ExitApp.IsDown)
	// {
	// 	GlobalRunning = false;
	// }

	if(SelectedController->Enter.IsDown)
	{ 
		DisplayInput->Start.IsDown = true; 
		DisplayInput->Start.HeldDownCount = SelectedController->Enter.HeldDownCount;
	}
	else 
	{
		DisplayInput->Start.IsDown = false;
		DisplayInput->Start.HeldDownCount = 0; 
	}

	if(SelectedController->ActionA.IsDown)
	{
		DisplayInput->A.IsDown = true;
		DisplayInput->A.HeldDownCount = SelectedController->ActionA.HeldDownCount;

	}
	else 
	{ 
		DisplayInput->A.IsDown = false; 
		DisplayInput->A.HeldDownCount = 0;
	}

	if(SelectedController->ActionB.IsDown)
	{
		DisplayInput->B.IsDown = true;
		DisplayInput->B.HeldDownCount = SelectedController->ActionB.HeldDownCount;

	}
	else 
	{ 
		DisplayInput->B.IsDown = false; 
		DisplayInput->B.HeldDownCount = 0;
	}

	if(SelectedController->ActionC.IsDown)
	{
		DisplayInput->C.IsDown = true;
		DisplayInput->C.HeldDownCount = SelectedController->ActionC.HeldDownCount;

	}
	else 
	{ 
		DisplayInput->C.IsDown = false; 
		DisplayInput->C.HeldDownCount = 0;
	}

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

// global int32 CircleValueStart = 0;
// global int32 CircleValueA = 0;
// global int32 CircleValueB = 0;
// global int32 CircleValueC = 0;
global int32 CircleCountStart = 0;
global int32 CircleCountA = 0;
global int32 CircleCountB = 0;
global int32 CircleCountC = 0;

inline int32 ButtonCircleCount(int32 CurrentCount, app_button_state Button)
{
	int32 CountCap = 10;
	int32 Result;

	if(Button.IsDown)
	{
		Result = Button.HeldDownCount;
	} else
	{
		Result = CurrentCount - 2;
	}
	if(Result > CountCap){ Result = CountCap; }
	if(Result < 0){ Result = 0; }

	return Result;
}

internal void Render(render_layers *RenderLayers, app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;
	display_input *DisplayInput = &AppState->DisplayInput;

	DrawRainbowHorizontal(RenderLayers->BackgroundBuffer, testvalue);
	testvalue = ModuloFloat32(testvalue + 0.001f, 1.0f);


	// Blank
	DrawRectangle(RenderLayers->ForegroundBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, TRANSPARENT_COLOUR, 1.0f);

	float32 LogoBottom = 5.0f;
	DrawImage(RenderLayers->ForegroundBuffer, 10, LogoBottom, timpz_logo);

	float32 ButtonLeft = 10.0f;
	float32 ButtonBottom = LogoBottom + 85.0f;
	float32 ButtonDistanceBetween = 60.0f;

	float32 ButtonSide = 56.0f;

	float32 ButtonLeftStart = ButtonLeft;
	float32 ButtonLeftA = ButtonLeft + ButtonDistanceBetween;
	float32 ButtonLeftB = ButtonLeft + ButtonDistanceBetween*2;
	float32 ButtonLeftC = ButtonLeft + ButtonDistanceBetween*3;

	DrawImage(RenderLayers->ForegroundBuffer, ButtonLeftStart, ButtonBottom, Dpad_Image);
	DrawDirection(DisplayInput->Dpad, ButtonLeftStart, ButtonBottom, RenderLayers->ForegroundBuffer);

	if(DisplayInput->Start.IsDown)
	{
		DrawImageOnTop(RenderLayers->ForegroundBuffer, ButtonLeftStart, ButtonBottom, S_Press);
	}

	DrawButton(DisplayInput->A.IsDown, ButtonLeftA, ButtonBottom, A_Press, A_Button, RenderLayers->ForegroundBuffer);
	DrawButton(DisplayInput->B.IsDown, ButtonLeftB, ButtonBottom, B_Press, B_Button, RenderLayers->ForegroundBuffer);
	DrawButton(DisplayInput->C.IsDown, ButtonLeftC, ButtonBottom, C_Press, C_Button, RenderLayers->ForegroundBuffer);


	JoinBuffersFirstPass(RenderLayers);

	DrawRectangle(RenderLayers->BackgroundBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, TRANSPARENT_COLOUR, 1.0f);

	DrawImage(RenderLayers->BackgroundBuffer, ButtonLeftStart, ButtonBottom, DpadInner_Image);

	vector3 CircleColour = {0, 0, 0};

	float32 MinCircle = 27.5f;
	float32 MaxCircle = 35.0f;
	float32 SpeedMultiplier = 0.5f;

	CircleCountStart = ButtonCircleCount(CircleCountStart, DisplayInput->Start);
	CircleCountA = ButtonCircleCount(CircleCountA, DisplayInput->A);
	CircleCountB = ButtonCircleCount(CircleCountB, DisplayInput->B);
	CircleCountC = ButtonCircleCount(CircleCountC, DisplayInput->C);

	float32 CurrentCircleStart = MinCircle - 1.0f + CircleCountStart*SpeedMultiplier*1.2f;
	float32 CurrentCircleA = MinCircle + CircleCountA*SpeedMultiplier;
	float32 CurrentCircleB = MinCircle + CircleCountB*SpeedMultiplier;
	float32 CurrentCircleC = MinCircle + CircleCountC*SpeedMultiplier;

	if(CurrentCircleStart > MaxCircle){ CurrentCircleStart = MaxCircle; }
	CurrentCircleStart = LinerarInterpolationCubed(CurrentCircleStart, MinCircle, MaxCircle);

	if(CurrentCircleA > MaxCircle){ CurrentCircleA = MaxCircle; }
	CurrentCircleA = LinerarInterpolationCubed(CurrentCircleA, MinCircle, MaxCircle);

	if(CurrentCircleB > MaxCircle){ CurrentCircleB = MaxCircle; }
	CurrentCircleB = LinerarInterpolationCubed(CurrentCircleB, MinCircle, MaxCircle);

	if(CurrentCircleC > MaxCircle){ CurrentCircleC = MaxCircle; }
	CurrentCircleC = LinerarInterpolationCubed(CurrentCircleC, MinCircle, MaxCircle);

	DrawCircle(RenderLayers->BackgroundBuffer, ButtonLeftStart + ButtonSide/2, ButtonBottom + ButtonSide/2, CurrentCircleStart, CircleColour);

	DrawCircle(RenderLayers->BackgroundBuffer, ButtonLeftA + ButtonSide/2, ButtonBottom + ButtonSide/2, CurrentCircleA, CircleColour);
	DrawCircle(RenderLayers->BackgroundBuffer, ButtonLeftB + ButtonSide/2, ButtonBottom + ButtonSide/2, CurrentCircleB, CircleColour);
	DrawCircle(RenderLayers->BackgroundBuffer, ButtonLeftC + ButtonSide/2, ButtonBottom + ButtonSide/2, CurrentCircleC, CircleColour);
	
	JoinBuffersSecondPass(RenderLayers);
}