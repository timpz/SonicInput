#include "settings.h"
#include "app.h"
#include "app_math.h"
#include "app_math.cpp"

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
