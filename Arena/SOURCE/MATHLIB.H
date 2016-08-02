//
// mathlib.h
//
// craig richardson
//

#ifndef _MATH_H
#define _MATH_H
#include <math.h>

#define DEGREE 7200

#define M_PI		3.14159265358979323846f
#define Pi180 0.017453292f
#define Pi360 0.008726646f

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( (a) * 180.0f ) / M_PI )

#define qfabs(a) ((a<0) ? -(a) : (a))

#define AngleNormalize360(x) ((float)((360.0 / 65536.f) * ((int)((x) * (65536.f / 360.0f)) & 65535)))

void SetupSinCos(void);

class MathLib
{
public:
	MathLib()
	{
		int i;
		float a;

		for(i=0;i<DEGREE+1;i++)
		{
			a=i/20.0f;
			sinc[i]=(float)sin(a*Pi180);
			cosc[i]=(float)cos(a*Pi180);
			if(a==90)
			{
				sinc[i]=1.0f;
				cosc[i]=0.0f;
			}
			if(a==180)
			{
				sinc[i]=0.0f;
				cosc[i]=-1.0f;
			}
			if(a==270)
			{
				sinc[i]=-1.0f;
				cosc[i]=0.0f;
			}
		}
	};

	float Sin(float fAng)
	{ 
		int a=(int)(AngleNormalize360(fAng)*20.f);
		return sinc[a]; 
	};

	float Cos(float fAng)
	{ 
		int a=(int)(AngleNormalize360(fAng)*20.f);
		return cosc[a]; 
	};

private:
		//sin cos table for extra speed
	static float cosc[DEGREE+1];
	static float sinc[DEGREE+1];
};

extern MathLib g_MathLib;

#endif