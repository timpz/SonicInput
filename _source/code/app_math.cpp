#include <math.h>
#include "app.h"
#include "app_math.h"

inline int32 FloorFloat32ToInt32(float32 Float32)
{
	int32 Result = (int32)floorf(Float32);
	return Result;
}

inline int32 RoundFloat32ToInt32(float32 Float32)
{
	int32 Result = (int32)roundf(Float32);
	return Result;
}

inline uint32 RoundFloat32ToUInt32(float32 Float32)
{
	int32 Int32 = RoundFloat32ToInt32(Float32);
	Assert(Int32 >= 0);
	uint32 Result = (uint32)Int32;
	return Result;
}

inline uint32 SafeTruncateUInt64(uint64 Value)
{
	//TODO: Defines for maximum values
	Assert(Value <= 0xFFFFFFFF);
	uint32 Result = (uint32)Value;
	return(Result);
}

inline float32 RandomFloat32(float32 Low, float32 High)
{
	float32 RandomValue = rand() % 10001 / 10000.0f; // Gives a value between 0.0f and 1.0f
	float32 Result = (High - Low) * RandomValue + Low;
	return Result;
}

inline float32 RootFloat32(float32 Float32)
{
	float32 Result = sqrtf(Float32);
	return Result;
}

inline float32 AbsoluteFloat32(float32 Float32)
{
	float32 Result = Float32 * Float32;
	Result = RootFloat32(Result);
	return Result;
}

internal float32 Magnitude(vector2 A)
{
	float32 DeltaX = A.X * A.X;
	float32 DeltaY = A.Y * A.Y;

	float32 Result = RootFloat32(DeltaX + DeltaY);

	return Result;
}

internal vector2 Normalise(vector2 A)
{
	vector2 Result = { 0, 0 };

	float32 M = Magnitude(A);
	
	Assert(M > 0);

	Result.X = A.X / M;
	Result.Y = A.Y / M;

	return Result;
}

internal float32 Dot(vector2 A, vector2 B)
{
	float32 Result = A.X *B.X + A.Y * B.Y;
	return Result;
}

internal vector2 AddVector2(vector2 A, vector2 B)
{
	vector2 Result = { 0, 0 };
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;

	return Result;
}

internal vector2 InverseVector2(vector2 A)
{
	vector2 Result = { -A.X, -A.Y };
	return Result;
}
