#ifndef APP_MATH_CPP

inline float32 Sine(float32 Angle)
{
	float32 Result = sinf(Angle);
	return Result;
}

inline float32 SmallestFloat32(float32 A, float32 B)
{
	float32 Result = 0;
	if(A < B)
	{
		Result = A;
	} else
	{
		Result = B;
	}

	return Result;
}

inline float32 LargestFloat32(float32 A, float32 B)
{
	float32 Result = 0;
	if(A > B)
	{
		Result = A;
	} else
	{
		Result = B;
	}

	return Result;
}

inline float32 ClampFloat32(float32 Value, float32 ClampingValue, bool32 UpperClamp)
{
	float32 Result = 0;

	if(UpperClamp)
	{
		if(Value > ClampingValue)
		{ Result = ClampingValue; }
		else
		{ Result = Value; }
	} else
	{
		if(Value < ClampingValue)
		{ Result = ClampingValue; }
		else
		{ Result = Value; }
	}

	return Result;
}

inline float32 LinerarInterpolationCubed(float32 Value, float32 Start, float32 End)
{
	Assert (Start < End);
	float32 Range = End - Start;
	float32 Ratio = Value - Start;
	Ratio /= Range;

	// Ratio *= Ratio*Ratio; //Cubed

	// (x - 1)^3 +1

	Ratio = Ratio - 1;

	Ratio = Ratio*Ratio*Ratio*Ratio*Ratio + 1;

	float32 Result = Ratio * Range + Start;
	return Result;

}

inline float32 ClampFloat32(float32 Value, float32 LowerBound, float32 UpperBound)
{
	Assert(LowerBound < UpperBound);
	float Result = 0;
	
	if(Value < LowerBound) 
	{ Result = LowerBound; }
	else if(Value > UpperBound) 
	{ Result = UpperBound; }
	else 
	{ Result = Value; }

	return Value;
}


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

inline float32 ModuloFloat32(float32 Float32, float32 Mod)
{
	float32 Result = fmodf(Float32, Mod);
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

#define APP_MATH_CPP
#endif