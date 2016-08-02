//
// vector.h
//
// craig richardson
//

#ifndef _VECTOR_H
#define _VECTOR_H

#include <math.h>

#define random() ((float)rand()/(float)RAND_MAX)

class Vec2
{
public:
	Vec2(){};

	Vec2(float fnewX,float fnewY){Set(fnewX,fnewY);};

	~Vec2(){};

	inline void Set(float fnewX,float fnewY)
	{
		fX=fnewX;
		fY=fnewY;
	};

	float Length() const
	{
		return (float)sqrt( fX*fX + fY*fY);
	};

	float Normalise()
	{
		float d,oned;

		d=(float)sqrt( fX*fX + fY*fY );

		oned=1.f/d;
		fX*=oned; 
		fY*=oned;

		return d;
	};

//	const float GetX() const { return fX; };
	const float GetX() const { return fX; };
	const void SetX(float fnewX) { fX=fnewX; };
	const float GetY() const { return fY; };
	const float GetY() { return fY; };
	const void SetY(float fnewY) { fY=fnewY; };

	Vec2 operator*(float fMul)
	{
		return Vec2(fX*fMul,fY*fMul);
	};
protected:
	float fX;
	float fY;
};

class Vec3 : public Vec2
{
public:
	Vec3(){};

	Vec3(float fnewX,float fnewY,float fnewZ){Set(fnewX,fnewY,fnewZ);};

	Vec3(const Vec3 &vec)
	{
		fX=vec.GetX();
		fY=vec.GetY();
		fZ=vec.GetZ();
	};

	~Vec3(){};

	inline void Set(float fnewX,float fnewY,float fnewZ)
	{
		Vec2::Set(fnewX,fnewY);
		fZ=fnewZ;
	};

	const float GetZ() const { return fZ; };
	const float GetZ() { return fZ; };
	const void SetZ(float fnewZ) { fZ=fnewZ; };

	// util functions
	float Dot(const Vec3 &vec)
	{
		return (fX*vec.GetX() + fY*vec.GetY() + fZ*vec.GetZ());
	};

	float Length() const
	{
		return (float)sqrt( fX*fX + fY*fY + fZ*fZ );
	};

	float LengthSquared() const
	{
		return ( fX*fX + fY*fY + fZ*fZ );
	};

	float Normalise()
	{
		float d,oned;

		d=(float)sqrt( fX*fX + fY*fY + fZ*fZ );

		oned=1.f/d;
		fX*=oned; 
		fY*=oned;
		fZ*=oned;
	//	fX=fX/d;
	//	fY=fY/d;
	//	fZ=fZ/d;

		return d;
	};

	const Vec3 CrossProd( const Vec3 &vec )
	{
		return Vec3
		( 
			fY*vec.GetZ()-fZ*vec.GetY(),
			fZ*vec.GetX()-fX*vec.GetZ(),
			fX*vec.GetY()-fY*vec.GetX() 
		); 
	};

	const Vec3 &CalcNormal(const Vec3 &vec1,const Vec3 &vec2,const Vec3 &vec3)
	{
		float d;
		
		Vec3 newvec1(vec2-vec1); 
		Vec3 newvec2(vec3-vec1);
		
		*this=newvec2.CrossProd(newvec1);

		d=(float)sqrt(Dot(*this));

		if(d)
		{
			d=1.f/d;
			Set(fX*d, fY*d, fZ*d);
		}
		else
		{
			Set(1.f, 1.f, 1.f);
		}
		return *this;
	};

	// operators
	const Vec3 &operator=(const Vec3 &vec)
	{
		fX=vec.GetX();
		fY=vec.GetY();
		fZ=vec.GetZ();

		return *this;
	};

	// addition
	const Vec3 operator+(float fAdd) { return Vec3(fX+fAdd,fY+fAdd,fZ+fAdd); };
	const Vec3 operator+(const Vec3 &vec) const
	{ 
		return Vec3(fX+vec.GetX(),fY+vec.GetY(),fZ+vec.GetZ()); 
	};
	const Vec3 &operator+=(float fAdd) 
	{ 
		fX+=fAdd;
		fY+=fAdd;
		fZ+=fAdd;

		return *this; 
	};
	const Vec3 &operator+=(const Vec3 &vec) 
	{ 
		fX+=vec.GetX();
		fY+=vec.GetY();
		fZ+=vec.GetZ();

		return *this; 
	};

	// subtraction
	const Vec3 operator-(float fSub) { return Vec3(fX-fSub,fY-fSub,fZ-fSub); };
	const Vec3 operator-(const Vec3 &vec) const
	{ 
		return Vec3(fX-vec.GetX(),fY-vec.GetY(),fZ-vec.GetZ()); 
	};

	const Vec3 &operator-()
	{ 
		fX= -fX;
		fY= -fY;
		fZ= -fZ;

		return *this; 
	};
/*	friend Vec3 operator-(const Vec3 &veca,const Vec3 &vecb)
	{ 
		return Vec3(veca.GetX()-vecb.GetX(),veca.GetY()-vecb.GetY(),veca.GetZ()-vecb.GetZ()); 
	};*/
	const Vec3 &operator-=(const Vec3 &vec) 
	{ 
		fX-=vec.GetX();
		fY-=vec.GetY();
		fZ-=vec.GetZ();

		return *this; 
	};

	// multiplication
	const Vec3 operator*(float fMul) const { return Vec3(fX*fMul,fY*fMul,fZ*fMul); };
	const Vec3 operator*(const Vec3 &vec) 
	{ 
		return Vec3(fX*vec.GetX(),fY*vec.GetY(),fZ*vec.GetZ()); 
	};

	// division
	const Vec3 operator/(unsigned short uDiv) const { return Vec3(fX/uDiv,fY/uDiv,fZ/uDiv); };
	const Vec3 operator/(float fDiv) const { return Vec3(fX/fDiv,fY/fDiv,fZ/fDiv); };
	const Vec3 operator/(const Vec3 &vec) 
	{ 
		return Vec3(fX/vec.GetX(),fY/vec.GetY(),fZ/vec.GetZ()); 
	};
	const Vec3 &operator/=(const float fDiv) 
	{ 
		float fNewDiv=1.f/fDiv;
		fX*=fNewDiv;
		fY*=fNewDiv;
		fZ*=fNewDiv;

		return *this; 
	};
	const Vec3 &operator/=(const Vec3 &vec) 
	{ 
		fX/=vec.GetX();
		fY/=vec.GetY();
		fZ/=vec.GetZ();

		return *this; 
	};

protected:
	float fZ;
};

class Vec4 : public Vec3
{
public:
	Vec4(){};

	Vec4(float fnewX,float fnewY,float fnewZ,float fnewW){Set(fnewX,fnewY,fnewZ,fnewW);};

	Vec4(const Vec3 *p_vec)
	{
		fX=p_vec->GetX();
		fY=p_vec->GetY();
		fZ=p_vec->GetZ();
		fW=1.f;
	};

	~Vec4(){};

	const float GetW() const { return fW; };
	const void SetW(float fnewW) { fW=fnewW; };

	inline void Set(float fnewX,float fnewY,float fnewZ,float fnewW)
	{
		Vec3::Set(fnewX,fnewY,fnewZ);
		fW=fnewW;
	};

	inline Vec4 Interp(float t,const Vec4 &vec)
	{
		return Vec4
		(
			fX*(1.f-t) + vec.GetX()*t,
			fY*(1.f-t) + vec.GetY()*t,
			fZ*(1.f-t) + vec.GetZ()*t,
			fW*(1.f-t) + vec.GetW()*t
		);
	};

	// operators
	const Vec4 &operator=(const Vec3 &vec)
	{
		fX=vec.GetX();
		fY=vec.GetY();
		fZ=vec.GetZ();
		fW=1.f;

		return *this;
	};
	const Vec4 &operator=(const Vec3 *p_vec)
	{
		fX=p_vec->GetX();
		fY=p_vec->GetY();
		fZ=p_vec->GetZ();
		fW=1.f;

		return *this;
	};

	const Vec4 &operator=(const Vec4 &vec)
	{
		fX=vec.GetX();
		fY=vec.GetY();
		fZ=vec.GetZ();
		fW=vec.GetW();

		return *this;
	};

		// subtraction
	const Vec4 operator-(float fSub) { return Vec4(fX-fSub,fY-fSub,fZ-fSub,1.f); };
	const Vec4 operator-(const Vec4 &vec) 
	{ 
		return Vec4(fX-vec.GetX(),fY-vec.GetY(),fZ-vec.GetZ(),1.f); 
	};
	const Vec4 operator-() 
	{ 
		return Vec4(-fX,-fY,-fZ,-fW); 
	};

	const Vec4 operator+(const Vec4 &vec) const
	{ 
		return Vec4(fX+vec.GetX(),fY+vec.GetY(),fZ+vec.GetZ(), 1.f); 
	};

	const Vec3 operator-(const Vec3 &vec) const
	{ 
		return Vec3(fX-vec.GetX(),fY-vec.GetY(),fZ-vec.GetZ()); 
	};

	const Vec3 operator+(const Vec3 &vec) const
	{ 
		return Vec3(fX+vec.GetX(),fY+vec.GetY(),fZ+vec.GetZ()); 
	};

	// multiplication
	const Vec4 operator*(float fMul) { return Vec4(fX*fMul,fY*fMul,fZ*fMul, 1.f); };
	const Vec4 operator*(const Vec4 &vec) 
	{ 
		return Vec4(fX*vec.GetX(),fY*vec.GetY(),fZ*vec.GetZ(),1.f); 
	};

protected:
	float fW;
};

#endif