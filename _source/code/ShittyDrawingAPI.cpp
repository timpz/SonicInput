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
	// HueIndex = HueIndex * HueIndex;
	int32 HueIndexSelection = FloorFloat32ToInt32(HueIndex);
	float32 VariableComponent = Chroma * (1.0f - AbsoluteFloat32(ModuloFloat32(HueIndex, 2.0f) - 1));
	// VariableComponent = VariableComponent * VariableComponent;
	// VariableComponent = sqrtf(VariableComponent);

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
	float32 XOffset, float32 YOffset, 
	float32 XSize, float32 YSize
)
{
	clamped_area Result = {};

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

	Result.OffsetX = FloorFloat32ToInt32(XOffset);
	Result.OffsetX  = FloorFloat32ToInt32(YOffset);
	Result.SizeX  = FloorFloat32ToInt32(XSize);
	Result.SizeY  = FloorFloat32ToInt32(YSize);

	return Result;
}


internal void DrawRainbowHorizontal
(
	offscreen_buffer *Buffer, 
	float32 Offset
)
{
	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;
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

internal void DrawRectangle
(
	offscreen_buffer *Buffer, 
	float32 OffsetX, float32 OffsetY, 
	float32 SizeX, float32 SizeY, 
	float32 Red, float32 Green, float32 Blue
)
{
	clamped_area Clamp = ClampDrawingArea(OffsetX, OffsetY, SizeX, SizeY);

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;

	Row += Clamp.OffsetX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

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

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;

	Row += Clamp.OffsetX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

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

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;

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

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;

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

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;
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

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;
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
	float32 OffsetX, float32 OffsetY, float32 Radius, 
	float32 Red, float32 Green, float32 Blue, float32 Alpha
)
{
	// int32 LowerBoundX = RoundFloat32ToInt32(XOffset - Radius);
	// int32 UpperBoundX = RoundFloat32ToInt32(XOffset + Radius);
	// int32 LowerBoundY = RoundFloat32ToInt32(YOffset - Radius);
	// int32 UpperBoundY = RoundFloat32ToInt32(YOffset + Radius);

	clamped_area Clamp = ClampDrawingArea
	(
		OffsetX - Radius, OffsetY - Radius, 
		OffsetX + Radius, OffsetY + Radius
	);

	// if(LowerBoundX < 0)
	// {
	// 	LowerBoundX = 0;
	// }

	// if(LowerBoundY < 0)
	// {
	// 	LowerBoundY = 0;
	// }

	// if(UpperBoundX >= WINDOW_WIDTH)
	// {
	// 	UpperBoundX = WINDOW_WIDTH;
	// }

	// if(UpperBoundY >= WINDOW_HEIGHT)
	// {
	// 	UpperBoundY = WINDOW_HEIGHT;
	// }

	uint32 *Row = (uint32 *)Buffer->BackgroundLayer;
	Row += Clamp.OffsetX;
	Row += (Buffer->Pitch / 4) * (WINDOW_HEIGHT - 1 - Clamp.OffsetY);

	for(int32 PixelY = Clamp.OffsetY; PixelY < Clamp.SizeY; PixelY++)
	{
		uint32 *Pixel = Row;
		for(int32 PixelX = Clamp.OffsetX; PixelX < Clamp.SizeX; PixelX++)
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
