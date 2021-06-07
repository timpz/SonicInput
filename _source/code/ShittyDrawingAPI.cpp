#include "settings.h"
#include "app.h"
#include "app_math.h"
#include "app_math.cpp"



internal vector3 HSVtoRBG(float32 Hue, float32 Saturation, float32 Value)
{
	// For Hue 0 to 1
	// Saturation 0 to 1
	// Value 0 to 1

	float32 Chroma = Saturation * Value;
	float32 HueIndex = Hue / (1.0f / 6.0f);
	int32 HueIndexSelection = FloorFloat32ToInt32(HueIndex);
	float32 VariableComponent = Chroma * (1.0f - AbsoluteFloat32(ModuloFloat32(HueIndex, 2.0f) - 1));

	vector3 Result = {};

	switch (HueIndexSelection)
	{
		case 0:
		{
			Result.X = Chroma;
			Result.Y = VariableComponent;
			Result.Z = 0;
		} break;
		case 1:
		{
			Result.X = VariableComponent;
			Result.Y = Chroma;
			Result.Z = 0;
		} break;
		case 2:
		{
			Result.X = 0;
			Result.Y = Chroma;
			Result.Z = VariableComponent;
		} break;
		case 3:
		{
			Result.X = 0;
			Result.Y = VariableComponent;
			Result.Z = Chroma;
		} break;
		case 4:
		{
			Result.X = VariableComponent;
			Result.Y = 0;
			Result.Z = Chroma;
		} break;
		case 5:
		{
			Result.X = Chroma;
			Result.Y = 0;
			Result.Z = VariableComponent;
		} break;
		
		default:
		{
			Result.X = 0;
			Result.Y = 0;
			Result.Z = 0;
		};
	}

	return Result;
}

struct clamped_area
{
	int32 OffsetX;
	int32 OffsetY;
	int32 SizeX;
	int32 SizeY;
};

internal clamped_area ClampDrawingArea
(
	float32 OffsetX, float32 OffsetY, 
	float32 SizeX, float32 SizeY
)
{
	clamped_area Result = {};

	if(OffsetX < 0)
	{
		SizeX += OffsetX;
		OffsetX = 0;
	}

	if(OffsetY < 0)
	{
		SizeY += OffsetY;
		OffsetY = 0;
	}

	if(OffsetX + SizeX >= WINDOW_WIDTH)
	{
		SizeX = WINDOW_WIDTH - OffsetX;
	}

	if(OffsetY + SizeY >= WINDOW_HEIGHT)
	{
		SizeY = WINDOW_HEIGHT - OffsetY;
	}

	Result.OffsetX = FloorFloat32ToInt32(OffsetX);
	Result.OffsetY = FloorFloat32ToInt32(OffsetY);
	Result.SizeX  = FloorFloat32ToInt32(SizeX);
	Result.SizeY  = FloorFloat32ToInt32(SizeY);

	return Result;
}

// Finds if Dot is on the left or right side of a vector between two vertices
inline float32 CalculateSideOfDot(vector2 Dot, vector2 VerticeFrom, vector2 VerticeTo)
{
	// Based on the 3rd method here
	// http://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html

}

internal void DrawTriangle(offscreen_buffer *Buffer, vector2 PosA, vector2 PosB, vector2 PosC, vector3 Colour)
{
	// Find Area
	float32 OffsetX = 0.0f;

	OffsetX = SmallestFloat32(PosA.X, PosB.X);
	OffsetX = SmallestFloat32(OffsetX, PosC.X);

	float32 SizeX = 0.0f;

	SizeX = LargestFloat32(PosA.X, PosB.X);
	SizeX = LargestFloat32(SizeX, PosC.X) - OffsetX;

	float32 OffsetY = 0.0f;

	OffsetY = SmallestFloat32(PosA.Y, PosB.Y);
	OffsetY = SmallestFloat32(OffsetY, PosC.Y);

	float32 SizeY = 0.0f;

	SizeY = LargestFloat32(PosA.Y, PosB.Y);
	SizeY = LargestFloat32(SizeY, PosC.Y) - OffsetY;

	// Clamp Area
	clamped_area Clamp = ClampDrawingArea(OffsetX, OffsetY, SizeX, SizeY);

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

	Row += Clamp.OffsetX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

	float32 Red = Colour.X;
	float32 Green = Colour.Y;
	float32 Blue = Colour.Z;

	uint32 PixelColour = 
	(
		(0xFF << 24) |
		(RoundFloat32ToUInt32(255.0f * Red) << 16)|
		(RoundFloat32ToUInt32(255.0f * Green) << 8)|
		(RoundFloat32ToUInt32(255.0f * Blue) << 0)
	);

	for (int32 Y = 0; Y < Clamp.SizeY; ++Y)
	{
		uint32 *Pixel = Row;
		for(int32 X = 0; X < Clamp.SizeX; ++X)
		{
			*Pixel++ = PixelColour;
		}
		Row -= Buffer->Pitch/4;
	}
}

internal void DrawRainbowHorizontal
(
	offscreen_buffer *Buffer, 
	float32 Offset
)
{
	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1);

	int32 XSizeInt = WINDOW_WIDTH;
	int32 YSizeInt = WINDOW_HEIGHT;

	for (int32 Y = 0; Y < YSizeInt; ++Y)
	{
		uint32 *Pixel = Row;
		for(int32 X = 0; X < XSizeInt; ++X)
		{

			float32 XValue = ModuloFloat32(Offset + (float32)X / ((float32)WINDOW_WIDTH), 1.0f);
			vector3 Colours = HSVtoRBG(XValue, 1, 1);

			float32 Red = Colours.X;
			float32 Green = Colours.Y;
			float32 Blue = Colours.Z;

			uint32 PixelColour = 
			(
				(0xFF << 24) |
				(RoundFloat32ToUInt32(255.0f * Red) << 16) |
				(RoundFloat32ToUInt32(255.0f * Green) << 8) |
				(RoundFloat32ToUInt32(255.0f * Blue) << 0)
			);

			*Pixel++ = PixelColour;
		}
		Row -= Buffer->Pitch/4;
	}


}

// internal void DrawRectangle
// (
// 	offscreen_buffer *Buffer, 
// 	float32 OffsetX, float32 OffsetY, 
// 	float32 SizeX, float32 SizeY, 
// 	float32 Red, float32 Green, float32 Blue
// )
// {
// 	clamped_area Clamp = ClampDrawingArea(OffsetX, OffsetY, SizeX, SizeY);

// 	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

// 	Row += Clamp.OffsetX;
// 	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

// 	uint32 PixelColour = 
// 	(
// 		(0xFF << 24) |
// 		(RoundFloat32ToUInt32(255.0f * Red) << 16)|
// 		(RoundFloat32ToUInt32(255.0f * Green) << 8)|
// 		(RoundFloat32ToUInt32(255.0f * Blue) << 0)
// 	);

// 	for (int32 Y = 0; Y < Clamp.SizeY; ++Y)
// 	{
// 		uint32 *Pixel = Row;
// 		for(int32 X = 0; X < Clamp.SizeX; ++X)
// 		{
// 			*Pixel++ = PixelColour;
// 		}
// 		Row -= Buffer->Pitch/4;
// 	}
// }


// NOTE: Drawing with alpha is much slower since we need to blend with the existing background
internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	float32 SizeX, float32 SizeY, 
	float32 Red, float32 Green, float32 Blue, float32 Alpha
)
{
	clamped_area Clamp = ClampDrawingArea(OffsetX, OffsetY, SizeX, SizeY);

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

	Row += Clamp.OffsetX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

	if(Alpha >= 1.0f)
	{
		uint32 PixelColour = 
		(
			(0xFF << 24) |
			(RoundFloat32ToUInt32(255.0f * Red) << 16)|
			(RoundFloat32ToUInt32(255.0f * Green) << 8)|
			(RoundFloat32ToUInt32(255.0f * Blue) << 0)
		);

		for (int32 Y = 0; Y < Clamp.SizeY; ++Y)
		{
			uint32 *Pixel = Row;
			for(int32 X = 0; X < Clamp.SizeX; ++X)
			{
				*Pixel++ = PixelColour;
			}
			Row -= Buffer->Pitch/4;
		}
	} else
	{
		for (int32 Y = 0; Y < Clamp.SizeY; ++Y)
		{
			uint32 *Pixel = Row;
			for(int32 X = 0; X < Clamp.SizeX; ++X)
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
}

internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	float32 SizeX, float32 SizeY, 
	uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha
)
{
	float32 RedFloat32 = Red / 255.0f;
	float32 GreenFloat32 = Green / 255.0f;
	float32 BlueFloat32 = Blue / 255.0f;
	float32 AlphaFloat32 = Alpha / 255.0f;

	DrawRectangle
	(
		Buffer, OffsetX, OffsetY, SizeX, SizeY,
		RedFloat32, GreenFloat32, BlueFloat32, AlphaFloat32
	);
}

internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	float32 SizeX, float32 SizeY, 
	uint32 Colour, float32 Alpha
)
{
	// float32 AlphaFloat32 = (float32)((Colour & 0xFF000000) >> 24) / 255.0f;
	float32 RedFloat32 = (float32)((Colour & 0xFF0000) >> 16) / 255.0f;
	float32 GreenFloat32 = (float32)((Colour & 0xFF00) >> 8) / 255.0f;
	float32 BlueFloat32 = (float32)(Colour & 0xFF) / 255.0f;

	DrawRectangle
	(
		Buffer, OffsetX, OffsetY, SizeX, SizeY,
		RedFloat32, GreenFloat32, BlueFloat32, Alpha
	);
}





internal void DrawImage
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	uint8 *Image
)
{

	int32 Width = *(int32 *)Image;
	int32 Height = *(int32 *)(Image+4);

	uint32 StartOfData = *(uint32 *)(Image + 8);

	uint8 *Data = Image + StartOfData;
	
	int32 StartX = 0;
	int32 StartY = 0;


	if(OffsetX < 0)
	{
		StartX -= RoundFloat32ToInt32(OffsetX);
		Width -= StartX;
		OffsetX = 0;
	}

	if(OffsetY < 0)
	{
		StartY -= RoundFloat32ToInt32(OffsetY);
		Height -= StartY;
		OffsetY = 0;
	}

	if(OffsetX + Width >= WINDOW_WIDTH)
	{
		Width -= (RoundFloat32ToInt32(OffsetX) + Width) - WINDOW_WIDTH;
		if(Width < 0)
		{
			Width = 0;
		}

	}

	if(OffsetY + Height >= WINDOW_HEIGHT)
	{
		Height -= (RoundFloat32ToInt32(OffsetY) + Height) - WINDOW_HEIGHT;
		if(Height < 0)
		{
			Height = 0;
		}

	}

	int32 XOffsetInt = RoundFloat32ToInt32(OffsetX);
	int32 YOffsetInt = RoundFloat32ToInt32(OffsetY);

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

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
#if 0
internal void DrawImageInverted
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	uint8 *Image
)
{

	int32 Width = *(int32 *)Image;
	int32 Height = *(int32 *)(Image+4);

	uint32 StartOfData = *(uint32 *)(Image + 8);

	uint8 *Data = Image + StartOfData;
	
	int32 StartX = 0;
	int32 StartY = 0;


	if(OffsetX < 0)
	{
		StartX -= RoundFloat32ToInt32(OffsetX);
		Width -= StartX;
		OffsetX = 0;
	}

	if(OffsetY < 0)
	{
		StartY -= RoundFloat32ToInt32(OffsetY);
		Height -= StartY;
		OffsetY = 0;
	}

	if(OffsetX + Width >= WINDOW_WIDTH)
	{
		Width -= (RoundFloat32ToInt32(OffsetX) + Width) - WINDOW_WIDTH;
		if(Width < 0)
		{
			Width = 0;
		}

	}

	if(OffsetY + Height >= WINDOW_HEIGHT)
	{
		Height -= (RoundFloat32ToInt32(OffsetY) + Height) - WINDOW_HEIGHT;
		if(Height < 0)
		{
			Height = 0;
		}

	}

	int32 XOffsetInt = RoundFloat32ToInt32(OffsetX);
	int32 YOffsetInt = RoundFloat32ToInt32(OffsetY);

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

	Row += XOffsetInt;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - YOffsetInt);


	uint8 *ImageData = Data + StartX + StartY * Width;

	for (int32 Y = 0; Y < Height; ++Y)
	{
		uint32 *Pixel = Row;
		uint8 *Source = ImageData;
		for(int32 X = 0; X < Width; ++X)
		{
			uint8 Blue = 0xFF - *Source;
			uint8 Green = 0xFF - *Source;
			uint8 Red = 0xFF - *Source;

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
#endif

internal void DrawImageOnTop
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	uint8 *Image
)
{

	int32 Width = *(int32 *)Image;
	int32 Height = *(int32 *)(Image+4);

	uint32 StartOfData = *(uint32 *)(Image + 8);

	uint8 *Data = Image + StartOfData;
	
	int32 StartX = 0;
	int32 StartY = 0;


	if(OffsetX < 0)
	{
		StartX -= RoundFloat32ToInt32(OffsetX);
		Width -= StartX;
		OffsetX = 0;
	}

	if(OffsetY < 0)
	{
		StartY -= RoundFloat32ToInt32(OffsetY);
		Height -= StartY;
		OffsetY = 0;
	}

	if(OffsetX + Width >= WINDOW_WIDTH)
	{
		Width -= (RoundFloat32ToInt32(OffsetX) + Width) - WINDOW_WIDTH;
		if(Width < 0)
		{
			Width = 0;
		}

	}

	if(OffsetY + Height >= WINDOW_HEIGHT)
	{
		Height -= (RoundFloat32ToInt32(OffsetY) + Height) - WINDOW_HEIGHT;
		if(Height < 0)
		{
			Height = 0;
		}

	}

	int32 XOffsetInt = RoundFloat32ToInt32(OffsetX);
	int32 YOffsetInt = RoundFloat32ToInt32(OffsetY);

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;

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






internal void DrawCircle
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, float32 Radius, 
	float32 Red, float32 Green, float32 Blue
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(OffsetX - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(OffsetX + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(OffsetY - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(OffsetY + Radius);

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

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;
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

			float32 RelativePixelX = PixelX - OffsetX;
			float32 RelativePixelY = PixelY - OffsetY;

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
	float32 OffsetX, float32 OffsetY, float32 Radius, 
	vector3 Colour
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(OffsetX - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(OffsetX + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(OffsetY - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(OffsetY + Radius);

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

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;
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

			float32 RelativePixelX = PixelX - OffsetX;
			float32 RelativePixelY = PixelY - OffsetY;

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
	float32 OffsetX, float32 OffsetY, float32 Radius, 
	float32 Red, float32 Green, float32 Blue, float32 Alpha
)
{
	int32 LowerBoundX = RoundFloat32ToInt32(OffsetX - Radius);
	int32 UpperBoundX = RoundFloat32ToInt32(OffsetX + Radius);
	int32 LowerBoundY = RoundFloat32ToInt32(OffsetY - Radius);
	int32 UpperBoundY = RoundFloat32ToInt32(OffsetY + Radius);


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

	uint32 *Row = (uint32 *)Buffer->StartOfBuffer;
	Row += LowerBoundX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - UpperBoundY);

	for(int32 PixelY = LowerBoundY; PixelY < UpperBoundY; PixelY++)
	{
		uint32 *Pixel = Row;
		for(int32 PixelX = LowerBoundX; PixelX < UpperBoundX; PixelX++)
		{

			float32 RelativePixelX = PixelX - OffsetX;
			float32 RelativePixelY = PixelY - OffsetY;

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

internal void JoinBuffersFirstPass(render_layers *RenderLayers)
{
	uint32 *BGRow = (uint32 *)RenderLayers->BackgroundBuffer->StartOfBuffer;
	BGRow += (RenderLayers->BackgroundBuffer->Pitch / 4) * (WINDOW_HEIGHT - 1);

	uint32 *FGRow = (uint32 *)RenderLayers->ForegroundBuffer->StartOfBuffer;
	FGRow += (RenderLayers->ForegroundBuffer->Pitch / 4) * (WINDOW_HEIGHT - 1);


	for(int32 PixelY = 0; PixelY < WINDOW_HEIGHT; PixelY++)
	{
		uint32 *BGPixel = BGRow;
		uint32 *FGPixel = FGRow;
		for(int32 PixelX = 0; PixelX < WINDOW_WIDTH; PixelX++)
		{

			uint8 *PixelColour = (uint8 *)BGPixel++;
			float32 Blue = *PixelColour++;
			float32 Green = *PixelColour++;
			float32 Red = *PixelColour;


			int32 ResultBlue = 1;
			int32 ResultGreen = 1;
			int32 ResultRed = 1;
						
			uint8 MaskValue = *(uint8 *)FGPixel;
			if(MaskValue > 0xFF / 2)
			{ 
				ResultBlue = RoundFloat32ToInt32(Blue);
				ResultGreen = RoundFloat32ToInt32(Green);
				ResultRed = RoundFloat32ToInt32(Red);
			}

			// float32 Ratio = ((float32)MaskValue) / ((float32)0xFF);

			
			*FGPixel++ = 
			(
				(MaskValue << 24) |
				(ResultRed << 16) |
				(ResultGreen << 8) |
				(ResultBlue << 0)
			);
			
		}
		BGRow -= RenderLayers->BackgroundBuffer->Pitch/4;
		FGRow -= RenderLayers->ForegroundBuffer->Pitch/4;
	}
}


internal void JoinBuffersSecondPass(render_layers *RenderLayers)
{
	uint32 *BGRow = (uint32 *)RenderLayers->BackgroundBuffer->StartOfBuffer;
	BGRow += (RenderLayers->BackgroundBuffer->Pitch / 4) * (WINDOW_HEIGHT - 1);

	uint32 *FGRow = (uint32 *)RenderLayers->ForegroundBuffer->StartOfBuffer;
	FGRow += (RenderLayers->ForegroundBuffer->Pitch / 4) * (WINDOW_HEIGHT - 1);


	for(int32 PixelY = 0; PixelY < WINDOW_HEIGHT; PixelY++)
	{
		uint32 *BGPixel = BGRow;
		uint32 *FGPixel = FGRow;
		for(int32 PixelX = 0; PixelX < WINDOW_WIDTH; PixelX++)
		{

			uint8 *PixelColour = (uint8 *)FGPixel++;
			float32 Blue = *PixelColour++;
			float32 Green = *PixelColour++;
			float32 Red = *PixelColour++;
			uint8 MaskValue = *PixelColour;

			float32 Ratio = ((float32)MaskValue) / ((float32)0xFF);

			int32 ResultBlue = RoundFloat32ToInt32(Blue * Ratio);
			int32 ResultGreen = RoundFloat32ToInt32(Green * Ratio);
			int32 ResultRed = RoundFloat32ToInt32(Red * Ratio);

			uint32 NewColourValue = 
			(
				(0xFF << 24) |
				(ResultRed << 16) |
				(ResultGreen << 8) |
				(ResultBlue << 0)
			);

			if(MaskValue != 1)
			{
				*BGPixel = NewColourValue;
			}

			BGPixel++;
			
			// *BGPixel++ = 
			// (
			// 	(0xFF << 24) |
			// 	(ResultRed << 16) |
			// 	(ResultGreen << 8) |
			// 	(ResultBlue << 0)
			// );
			
		}
		BGRow -= RenderLayers->BackgroundBuffer->Pitch/4;
		FGRow -= RenderLayers->ForegroundBuffer->Pitch/4;
	}
}