#include "app.h"
#include "app_math.cpp"

local_persist bool32 TestButton = false;

internal void ProcessInput(keyboard_input *KeyboardInput, app_state *State)
{
	display_input *DisplayInput = &State->DisplayInput;

	if(KeyboardInput->ExitApp.IsDown != false)
	{
		GlobalRunning = false;
	}

	if(KeyboardInput->Enter.IsDown)
	{ DisplayInput->Start = true; }
	else { DisplayInput->Start = false; }

	if(KeyboardInput->J.IsDown){ DisplayInput->A = true; }
	else { DisplayInput->A = false; }

	if(KeyboardInput->K.IsDown){ DisplayInput->B = true; }
	else { DisplayInput->B = false; }

	if(KeyboardInput->L.IsDown){ DisplayInput->C = true; }
	else { DisplayInput->C = false; }

	uint32 DpadValue = 0;
	int32 LeftRight = 0;
	int32 UpDown = 0;

	if(KeyboardInput->A.IsDown && !KeyboardInput->D.IsDown)
	{
		LeftRight = -1;
	} else if(KeyboardInput->D.IsDown && !KeyboardInput->A.IsDown)
	{
		LeftRight = 1;
	}

	if(KeyboardInput->W.IsDown && !KeyboardInput->S.IsDown)
	{
		UpDown = 1;
	} else if(KeyboardInput->S.IsDown && !KeyboardInput->W.IsDown)
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
	

	if(KeyboardInput->A.IsDown && !KeyboardInput->D.IsDown)
	{
		if(KeyboardInput->W.IsDown)
		{
			DpadValue = 2; 
		} else if(KeyboardInput->S.IsDown)
		{
			DpadValue = 8;
		} else
		{
			DpadValue = 1;
		}
	} else if(KeyboardInput->D.IsDown && !KeyboardInput->A.IsDown)
	{
		if(KeyboardInput->W.IsDown)
		{
			DpadValue = 4; 
		} else if(KeyboardInput->S.IsDown)
		{
			DpadValue = 6;
		} else
		{
			DpadValue = 5;
		}
	} else if(KeyboardInput->W.IsDown && !KeyboardInput->S.IsDown)
	{
		DpadValue = 3;
	} else if(KeyboardInput->S.IsDown && !KeyboardInput->W.IsDown)
	{
		DpadValue = 7;
	}

	DisplayInput->Dpad = DpadValue;
}

internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, 
	float32 XSize, float32 YSize, 
	float32 Red, float32 Green, float32 Blue
)
{
	if(XOffset < 0)
	{
		XSize += XOffset;
		XOffset = 0;
	}

	if(YOffset < 0)
	{
		YSize += YOffset;
		YOffset = 0;
	}

	if(XOffset + XSize >= WINDOW_WIDTH)
	{
		XSize = WINDOW_WIDTH - XOffset;
	}

	if(YOffset + YSize >= WINDOW_HEIGHT)
	{
		YSize = WINDOW_HEIGHT - YOffset;
	}

	int32 XOffsetInt = FloorFloat32ToInt32(XOffset);
	int32 YOffsetInt = FloorFloat32ToInt32(YOffset);
	int32 XSizeInt = FloorFloat32ToInt32(XSize);
	int32 YSizeInt = FloorFloat32ToInt32(YSize);

	uint32 *Row = (uint32 *)Buffer->Memory;

	Row += XOffsetInt;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - YOffsetInt);

	uint32 PixelColour = 
	(
		(0xFF << 24) |
		(RoundFloat32ToUInt32(255.0f * Red) << 16)|
		(RoundFloat32ToUInt32(255.0f * Green) << 8)|
		(RoundFloat32ToUInt32(255.0f * Blue) << 0)
	);

	for (int32 Y = 0; Y < YSizeInt; ++Y)
	{
		uint32 *Pixel = Row;
		for(int32 X = 0; X < XSizeInt; ++X)
		{
			*Pixel++ = PixelColour;
		}
		Row -= Buffer->Pitch/4;
	}
}

internal void DrawImage
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, 
	uint8 *Image
)
{

	int32 Width = *(int32 *)Image;
	int32 Height = *(int32 *)(Image+4);

	uint32 StartOfData = *(uint32 *)(Image + 8);

	uint8 *Data = Image + StartOfData;
	
	int32 StartX = 0;
	int32 StartY = 0;


	if(XOffset < 0)
	{
		StartX -= RoundFloat32ToInt32(XOffset);
		Width -= StartX;
		XOffset = 0;
	}

	if(YOffset < 0)
	{
		StartY -= RoundFloat32ToInt32(YOffset);
		Height -= StartY;
		YOffset = 0;
	}

	if(XOffset + Width >= WINDOW_WIDTH)
	{
		Width -= (RoundFloat32ToInt32(XOffset) + Width) - WINDOW_WIDTH;
		if(Width < 0)
		{
			Width = 0;
		}

	}

	if(YOffset + Height >= WINDOW_HEIGHT)
	{
		Height -= (RoundFloat32ToInt32(YOffset) + Height) - WINDOW_HEIGHT;
		if(Height < 0)
		{
			Height = 0;
		}

	}

	int32 XOffsetInt = RoundFloat32ToInt32(XOffset);
	int32 YOffsetInt = RoundFloat32ToInt32(YOffset);

	uint32 *Row = (uint32 *)Buffer->Memory;

	Row += XOffsetInt;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - YOffsetInt);


	uint8 *ImageData = Data + StartX + StartY * Width;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		uint32 *Pixel = Row;
		uint8 *Source = ImageData;
		for(int32 X = 0; X < Width; ++X)
		{
			uint8 Blue = *Source;
			uint8 Green = *Source;
			uint8 Red = *Source;

			*Pixel++ = 
			(
				(0xFF << 24) |
				(Red << 16) |
				(Green << 8) |
				(Blue << 0)
			);

			Source++;
		}
		Row -= Buffer->Pitch/4;
		ImageData += Width;
	}

}


internal void DrawImageOnTop
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, 
	uint8 *Image
)
{

	int32 Width = *(int32 *)Image;
	int32 Height = *(int32 *)(Image+4);

	uint32 StartOfData = *(uint32 *)(Image + 8);

	uint8 *Data = Image + StartOfData;
	
	int32 StartX = 0;
	int32 StartY = 0;


	if(XOffset < 0)
	{
		StartX -= RoundFloat32ToInt32(XOffset);
		Width -= StartX;
		XOffset = 0;
	}

	if(YOffset < 0)
	{
		StartY -= RoundFloat32ToInt32(YOffset);
		Height -= StartY;
		YOffset = 0;
	}

	if(XOffset + Width >= WINDOW_WIDTH)
	{
		Width -= (RoundFloat32ToInt32(XOffset) + Width) - WINDOW_WIDTH;
		if(Width < 0)
		{
			Width = 0;
		}

	}

	if(YOffset + Height >= WINDOW_HEIGHT)
	{
		Height -= (RoundFloat32ToInt32(YOffset) + Height) - WINDOW_HEIGHT;
		if(Height < 0)
		{
			Height = 0;
		}

	}

	int32 XOffsetInt = RoundFloat32ToInt32(XOffset);
	int32 YOffsetInt = RoundFloat32ToInt32(YOffset);

	uint32 *Row = (uint32 *)Buffer->Memory;

	Row += XOffsetInt;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - YOffsetInt);


	uint8 *ImageData = Data + StartX + StartY * Width;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		uint32 *Pixel = Row;
		uint8 *Source = ImageData;
		for(int32 X = 0; X < Width; ++X)
		{
			uint32 DestColour = *(uint8 *)Pixel;

			uint32 ImageColour = *Source;
			Source++;

			int32 ResultColour = DestColour + ImageColour;

			if(ResultColour > 255){ ResultColour = 255; }

			*Pixel++ = 
			(
				(0xFF << 24) |
				(ResultColour << 16) |
				(ResultColour << 8) |
				(ResultColour << 0)
			);
		}
		Row -= Buffer->Pitch/4;
		ImageData += Width;
	}

}






// NOTE: Drawing with alpha is much slower since we need to blend with the existing background
internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, 
	float32 XSize, float32 YSize, 
	float32 Red, float32 Green, float32 Blue, float32 Alpha
)
{
	if(XOffset < 0)
	{
		XSize += XOffset;
		XOffset = 0;
	}

	if(YOffset < 0)
	{
		YSize += YOffset;
		YOffset = 0;
	}

	if(XOffset + XSize >= WINDOW_WIDTH)
	{
		XSize = WINDOW_WIDTH - XOffset;
	}

	if(YOffset + YSize >= WINDOW_HEIGHT)
	{
		YSize = WINDOW_HEIGHT - YOffset;
	}

	int32 XOffsetInt = FloorFloat32ToInt32(XOffset);
	int32 YOffsetInt = FloorFloat32ToInt32(YOffset);
	int32 XSizeInt = FloorFloat32ToInt32(XSize);
	int32 YSizeInt = FloorFloat32ToInt32(YSize);

	uint32 *Row = (uint32 *)Buffer->Memory;

	Row += XOffsetInt;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - YOffsetInt);

	for (int32 Y = 0; Y < YSizeInt; ++Y)
	{
		uint32 *Pixel = Row;
		for(int32 X = 0; X < XSizeInt; ++X)
		{
			uint8 *Colour = (uint8 *)Pixel;
			float32 DestBlue = (float32)*Colour++;
			float32 DestGreen = (float32)*Colour++;
			float32 DestRed = (float32)*Colour;

			float32 Ratio = (float32)Alpha / 1.0f; 

			int32 ResultRed = RoundFloat32ToInt32(DestRed*(1-Ratio) + 255.0f * Red*Ratio);
			int32 ResultGreen = RoundFloat32ToInt32(DestGreen*(1-Ratio) + 255.0f * Green*Ratio);
			int32 ResultBlue = RoundFloat32ToInt32(DestBlue*(1-Ratio) + 255.0f * Blue*Ratio);

			*Pixel++ = 
			(
				(0xFF << 24) |
				(ResultRed << 16) |
				(ResultGreen << 8) |
				(ResultBlue << 0)
			);
		}
		Row -= Buffer->Pitch/4;
	}
}

internal void DrawCircle
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, float32 Radius, 
	float32 Red, float32 Green, float32 Blue
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(XOffset - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(XOffset + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(YOffset - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(YOffset + Radius);

	if(LowerBoundX < 0)
	{
		LowerBoundX = 0;
	}

	if(LowerBoundY < 0)
	{
		LowerBoundY = 0;
	}

	if(UpperBoundX >= WINDOW_WIDTH)
	{
		UpperBoundX = WINDOW_WIDTH;
	}

	if(UpperBoundY >= WINDOW_HEIGHT)
	{
		UpperBoundY = WINDOW_HEIGHT;
	}

	uint32 *Row = (uint32 *)Buffer->Memory;
	Row += LowerBoundX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - LowerBoundY);

	uint32 PixelColour = 
	(
		(0xFF << 24) |
		(RoundFloat32ToUInt32(255.0f * Red) << 16)|
		(RoundFloat32ToUInt32(255.0f * Green) << 8)|
		(RoundFloat32ToUInt32(255.0f * Blue) << 0)
	);

	for(int32 PixelY = LowerBoundY; PixelY < UpperBoundY; PixelY++)
	{
		uint32 *Pixel = Row;
		for(int32 PixelX = LowerBoundX; PixelX < UpperBoundX; PixelX++)
		{

			float32 RelativePixelX = PixelX - XOffset;
			float32 RelativePixelY = PixelY - YOffset;

			float32 Hypothenuse = RootFloat32((float32)(RelativePixelX * RelativePixelX + RelativePixelY * RelativePixelY));
			if(Hypothenuse <= Radius)
			{
				*Pixel = PixelColour;
			}
			Pixel++;
		}
		Row -= Buffer->Pitch/4;
	}
}


internal void DrawCircle
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, float32 Radius, 
	vector3 Colour
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(XOffset - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(XOffset + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(YOffset - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(YOffset + Radius);

	if(LowerBoundX < 0)
	{
		LowerBoundX = 0;
	}

	if(LowerBoundY < 0)
	{
		LowerBoundY = 0;
	}

	if(UpperBoundX >= WINDOW_WIDTH)
	{
		UpperBoundX = WINDOW_WIDTH;
	}

	if(UpperBoundY >= WINDOW_HEIGHT)
	{
		UpperBoundY = WINDOW_HEIGHT;
	}

	uint32 *Row = (uint32 *)Buffer->Memory;
	Row += LowerBoundX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - LowerBoundY);

	uint32 PixelColour = 
	(
		(0xFF << 24) |
		(RoundFloat32ToUInt32(255.0f * Colour.X) << 16)|
		(RoundFloat32ToUInt32(255.0f * Colour.Y) << 8)|
		(RoundFloat32ToUInt32(255.0f * Colour.Z) << 0)
	);

	for(int32 PixelY = LowerBoundY; PixelY < UpperBoundY; PixelY++)
	{
		uint32 *Pixel = Row;
		for(int32 PixelX = LowerBoundX; PixelX < UpperBoundX; PixelX++)
		{

			float32 RelativePixelX = PixelX - XOffset;
			float32 RelativePixelY = PixelY - YOffset;

			float32 Hypothenuse = RootFloat32((float32)(RelativePixelX * RelativePixelX + RelativePixelY * RelativePixelY));
			if(Hypothenuse <= Radius)
			{
				*Pixel = PixelColour;
			}
			Pixel++;
		}
		Row -= Buffer->Pitch/4;
	}
}

// NOTE: Drawing with alpha is much slower since we need to blend with the existing background
internal void DrawCircle
(
	offscreen_buffer *Buffer, 
	float32 XOffset, float32 YOffset, float32 Radius, 
	float32 Red, float32 Green, float32 Blue, float32 Alpha
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(XOffset - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(XOffset + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(YOffset - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(YOffset + Radius);

	if(LowerBoundX < 0)
	{
		LowerBoundX = 0;
	}

	if(LowerBoundY < 0)
	{
		LowerBoundY = 0;
	}

	if(UpperBoundX >= WINDOW_WIDTH)
	{
		UpperBoundX = WINDOW_WIDTH;
	}

	if(UpperBoundY >= WINDOW_HEIGHT)
	{
		UpperBoundY = WINDOW_HEIGHT;
	}

	uint32 *Row = (uint32 *)Buffer->Memory;
	Row += LowerBoundX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - LowerBoundY);

	for(int32 PixelY = LowerBoundY; PixelY < UpperBoundY; PixelY++)
	{
		uint32 *Pixel = Row;
		for(int32 PixelX = LowerBoundX; PixelX < UpperBoundX; PixelX++)
		{

			float32 RelativePixelX = PixelX - XOffset;
			float32 RelativePixelY = PixelY - YOffset;

			float32 Hypothenuse = RootFloat32((float32)(RelativePixelX * RelativePixelX + RelativePixelY * RelativePixelY));
			if(Hypothenuse <= Radius)
			{
				// Draw pixel
				uint8 *Colour = (uint8 *)Pixel;
				float32 DestBlue = (float32)*Colour++;
				float32 DestGreen = (float32)*Colour++;
				float32 DestRed = (float32)*Colour;

				float32 Ratio = (float32)Alpha / 1.0f; 

				int32 ResultRed = RoundFloat32ToInt32(DestRed*(1-Ratio) + 255.0f * Red*Ratio);
				int32 ResultGreen = RoundFloat32ToInt32(DestGreen*(1-Ratio) + 255.0f * Green*Ratio);
				int32 ResultBlue = RoundFloat32ToInt32(DestBlue*(1-Ratio) + 255.0f * Blue*Ratio);

				*Pixel = 
				(
					(0xFF << 24) |
					(ResultRed << 16) |
					(ResultGreen << 8) |
					(ResultBlue << 0)
				);
			}
			Pixel++;
		}
		Row -= Buffer->Pitch/4;
	}
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

			// PixelIndex_ = PixelIndex;
		}
	}

	// return FileSize;
}



internal void Initialise(app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;
}


internal void Update(keyboard_input *KeyboardInput, app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;

	ProcessInput(KeyboardInput, AppState);

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
		DrawImage(Buffer, PosX, PosY, ButtonPressed);
	} else
	{
		DrawImage(Buffer, PosX, PosY, ButtonNotPressed);
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

internal void Render(offscreen_buffer *Buffer, app_memory *Memory)
{
	app_state *AppState = (app_state *)Memory->PermanentStorage;
	display_input *DisplayInput = &AppState->DisplayInput;

	// Blank
	DrawRectangle(Buffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0);

	DrawImage(Buffer, 10.0f, 10.0f, Dpad_Image);
	DrawButton(DisplayInput->A, 70.0f, 10.0f, A_Press, A_Button, Buffer);
	DrawButton(DisplayInput->B, 130.0f, 10.0f, B_Press, B_Button, Buffer);
	DrawButton(DisplayInput->C, 190.0f, 10.0f, C_Press, C_Button, Buffer);

	if(DisplayInput->Start)
	{
		DrawImageOnTop(Buffer, 10.0f, 10.0f, S_Press);
	}

	DrawDirection(DisplayInput->Dpad, 10.0f, 10.0f, Buffer);
}