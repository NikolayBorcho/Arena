//
// matrix.h
//
// craig richardson
//

#ifndef _MATRIX_H
#define _MATRIX_H

#include "vector.h"
#include "mathlib.h"

class Matrix
{
public:
	Matrix(){};

	Matrix(const Vec4 &row1,const Vec4 &row2,const Vec4 &row3,const Vec4 &row4)
	{
		SetColumn(0,row1);
		SetColumn(1,row2);
		SetColumn(2,row3);
		SetColumn(3,row4);
	};

	Matrix(const float *p_fmat)
	{
		int i,j;

		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				fMat[i][j]=p_fmat[j];
			}
			p_fmat+=4;
		}

	};

	Matrix(	
		const float r1c1,const float r1c2,const float r1c3,const float r1c4,
		const float r2c1,const float r2c2,const float r2c3,const float r2c4,
		const float r3c1,const float r3c2,const float r3c3,const float r3c4,
		const float r4c1,const float r4c2,const float r4c3,const float r4c4)
	{
		fMat[0][0]=r1c1;fMat[0][1]=r1c2;fMat[0][2]=r1c3;fMat[0][3]=r1c4;
		fMat[1][0]=r2c1;fMat[1][1]=r2c2;fMat[1][2]=r2c3;fMat[1][3]=r2c4;
		fMat[2][0]=r3c1;fMat[2][1]=r3c2;fMat[2][2]=r3c3;fMat[2][3]=r3c4;
		fMat[3][0]=r4c1;fMat[3][1]=r4c2;fMat[3][2]=r4c3;fMat[3][3]=r4c4;
	};

	Matrix(const Matrix &mat)
	{
		SetColumn(0,mat.GetColumn(0));
		SetColumn(1,mat.GetColumn(1));
		SetColumn(2,mat.GetColumn(2));
		SetColumn(3,mat.GetColumn(3));
	};

	~Matrix(){};

	inline void Init()
	{
		fMat[0][0]=1.f;fMat[0][1]=0.f;fMat[0][2]=0.f;fMat[0][3]=0.f;

		fMat[1][0]=0.f;fMat[1][1]=1.f;fMat[1][2]=0.f;fMat[1][3]=0.f;

		fMat[2][0]=0.f;fMat[2][1]=0.f;fMat[2][2]=1.f;fMat[2][3]=0.f;

		fMat[3][0]=0.f;fMat[3][1]=0.f;fMat[3][2]=0.f;fMat[3][3]=1.f;
	};

	inline float GetVal(int iCol,int iRow) const
	{
		return fMat[iCol][iRow];
	};

	inline void SetVal(int iCol,int iRow,float fVal)
	{
		fMat[iCol][iRow]=fVal;
	};

	inline const float *Getfloat() const
	{
		return &fMat[0][0];
	};

	inline Vec4 GetColumn(int iCol) const
	{
		return Vec4(fMat[iCol][0],fMat[iCol][1],fMat[iCol][2],fMat[iCol][3]);
	};

	inline Matrix Interp(float t,const Matrix &mat) const
	{
		return Matrix
		(
			GetColumn(0).Interp(t,mat.GetColumn(0)),
			GetColumn(1).Interp(t,mat.GetColumn(1)),
			GetColumn(2).Interp(t,mat.GetColumn(2)),
			GetColumn(3).Interp(t,mat.GetColumn(3))
		);
	};

	inline void SetColumn(int iCol,const Vec4 &vec)
	{
		fMat[iCol][0]=vec.GetX();
		fMat[iCol][1]=vec.GetY();
		fMat[iCol][2]=vec.GetZ();
		fMat[iCol][3]=vec.GetW();
	};

	inline void SetColumn(int iCol,const Vec3 &vec)
	{
		fMat[iCol][0]=vec.GetX();
		fMat[iCol][1]=vec.GetY();
		fMat[iCol][2]=vec.GetZ();
		fMat[iCol][3]=1.f;
	};

	void RotX(float ang)
	{

		fMat[0][0]=1.f;

		fMat[1][1]=g_MathLib.Cos(ang);

		fMat[1][2]=g_MathLib.Sin(ang);

		fMat[2][1]=-g_MathLib.Sin(ang);

		fMat[2][2]=g_MathLib.Cos(ang);
	};

	void RotY(float ang)
	{

		fMat[0][0]=g_MathLib.Cos(ang);

		fMat[0][2]=-g_MathLib.Sin(ang);

		fMat[1][1]=1.f;

		fMat[2][0]=g_MathLib.Sin(ang);

		fMat[2][2]=g_MathLib.Cos(ang);
	};

	void RotZ(float ang)
	{

		fMat[0][0]=g_MathLib.Cos(ang);

		fMat[0][1]=g_MathLib.Sin(ang);

		fMat[1][0]=-g_MathLib.Sin(ang);

		fMat[1][1]=g_MathLib.Cos(ang);

		fMat[2][2]=1.f;
	};

	const Matrix &Inverse()
	{
		float fTemp;
		float fTrans0,fTrans1;

		// transpose of rotational component
		fTemp = fMat[0][1];
		fMat[0][1] = fMat[1][0];
		fMat[1][0] = fTemp;

		fTemp = fMat[0][2];
		fMat[0][2] = fMat[2][0];
		fMat[2][0] = fTemp;

		fTemp = fMat[1][2];
		fMat[1][2] = fMat[2][1];
		fMat[2][1] = fTemp;

		// project -ve of old translation
		fTrans0 = -fMat[0][0]*fMat[3][0] - fMat[1][0]*fMat[3][1] - fMat[2][0]*fMat[3][2];
		fTrans1 = -fMat[0][1]*fMat[3][0] - fMat[1][1]*fMat[3][1] - fMat[2][1]*fMat[3][2];
		fMat[3][2] = -fMat[0][2]*fMat[3][0] - fMat[1][2]*fMat[3][1] - fMat[2][2]*fMat[3][2];
		fMat[3][0] = fTrans0;
		fMat[3][1] = fTrans1;

		return *this;
	};

	void Inverse(Matrix *p_mat) const
	{
		float fTemp;
		float fTrans0,fTrans1;

		p_mat->SetVal(0,0, fMat[0][0]);
		p_mat->SetVal(0,3, fMat[0][3]);

		p_mat->SetVal(1,1, fMat[1][1]);
		p_mat->SetVal(1,3, fMat[1][3]);

		p_mat->SetVal(2,2, fMat[2][2]);
		p_mat->SetVal(2,3, fMat[2][3]);

		p_mat->SetVal(3,3, fMat[3][3]);

		// transpose of rotational component
		fTemp = fMat[0][1];
		p_mat->SetVal(0,1, fMat[1][0]);
		p_mat->SetVal(1,0, fTemp);

		fTemp = fMat[0][2];
		p_mat->SetVal(0,2, fMat[2][0]);
		p_mat->SetVal(2,0, fTemp);

		fTemp = fMat[1][2];
		p_mat->SetVal(1,2, fMat[2][1]);
		p_mat->SetVal(2,1, fTemp);

		// project -ve of old translation
		fTrans0 = -p_mat->GetVal(0,0)*fMat[3][0] - p_mat->GetVal(1,0)*fMat[3][1] - p_mat->GetVal(2,0)*fMat[3][2];
		fTrans1 = -p_mat->GetVal(0,1)*fMat[3][0] - p_mat->GetVal(1,1)*fMat[3][1] - p_mat->GetVal(2,1)*fMat[3][2];
		p_mat->SetVal(3,2, -p_mat->GetVal(0,2)*fMat[3][0] - p_mat->GetVal(1,2)*fMat[3][1] - p_mat->GetVal(2,2)*fMat[3][2]);
		p_mat->SetVal(3,0, fTrans0);
		p_mat->SetVal(3,1, fTrans1);
	};

	void InverseMatSlow(Matrix *p_mat)
	{
		float det;
		int i, j;

		p_mat->SetVal(0,0, (fMat[1][1]*fMat[2][2]-fMat[1][2]*fMat[2][1]) );
		p_mat->SetVal(1,0, (fMat[1][2]*fMat[2][0]-fMat[1][0]*fMat[2][2]) );
		p_mat->SetVal(2,0, (fMat[1][0]*fMat[2][1]-fMat[1][1]*fMat[2][0]) );
		p_mat->SetVal(0,3, 0.f);

		p_mat->SetVal(0,1, (fMat[2][1]*fMat[0][2]-fMat[2][2]*fMat[0][1]) );
		p_mat->SetVal(1,1, (fMat[2][2]*fMat[0][0]-fMat[2][0]*fMat[0][2]) );
		p_mat->SetVal(2,1, (fMat[2][0]*fMat[0][1]-fMat[2][1]*fMat[0][0]) );
		p_mat->SetVal(1,3, 0.f);

		p_mat->SetVal(0,2, (fMat[0][1]*fMat[1][2]-fMat[0][2]*fMat[1][1]) );
		p_mat->SetVal(1,2, (fMat[0][2]*fMat[1][0]-fMat[0][0]*fMat[1][2]) );
		p_mat->SetVal(2,2, (fMat[0][0]*fMat[1][1]-fMat[0][1]*fMat[1][0]) );
		p_mat->SetVal(2,3, 0.f);
		p_mat->SetVal(3,3, 1.f);

		det=0.f;
		for (i=0;i<3;i++)
		{
    		det+=fMat[i][0]*p_mat->GetVal(0,i);
		}

		if (det==0.f) det=.0000001f;
		for (i=0;i<3;i++)
		{
			for (j=0;j<3;j++) 
			{
				p_mat->SetVal(i,j, p_mat->GetVal(i,j)/det);
			}

     		p_mat->SetVal(3,i, -fMat[3][i]);
		}
	}

	Matrix Inverse() const
	{
		float fmatTemp[4][4];
		float fTemp;
		float fTrans0,fTrans1;

		fmatTemp[0][0]= fMat[0][0];
		fmatTemp[0][3]= fMat[0][3];

		fmatTemp[1][1]= fMat[1][1];
		fmatTemp[1][3]= fMat[1][3];

		fmatTemp[2][2]= fMat[2][2];
		fmatTemp[2][3]= fMat[2][3];

		fmatTemp[3][3]= fMat[3][3];

		// transpose of rotational component
		fTemp = fMat[0][1];
		fmatTemp[0][1]= fMat[1][0];
		fmatTemp[1][0]= fTemp;

		fTemp = fMat[0][2];
		fmatTemp[0][2]= fMat[2][0];
		fmatTemp[2][0]= fTemp;

		fTemp = fMat[1][2];
		fmatTemp[1][2]= fMat[2][1];
		fmatTemp[2][1]= fTemp;

		// project -ve of old translation
		fTrans0 = -fmatTemp[0][0]*fMat[3][0] - fmatTemp[1][0]*fMat[3][1] - fmatTemp[2][0]*fMat[3][2];
		fTrans1 = -fmatTemp[0][1]*fMat[3][0] - fmatTemp[1][1]*fMat[3][1] - fmatTemp[2][1]*fMat[3][2];
		fmatTemp[3][2]= -fmatTemp[0][2]*fMat[3][0] - fmatTemp[1][2]*fMat[3][1] - fmatTemp[2][2]*fMat[3][2];
		fmatTemp[3][0]= fTrans0;
		fmatTemp[3][1]= fTrans1;

		return Matrix(&fmatTemp[0][0]);
	};

	const Matrix &Transpose() 
	{
		float fTemp;

		fTemp=fMat[0][1];
		fMat[0][1]=fMat[1][0];
		fMat[1][0]=fTemp;

		fTemp=fMat[0][2];
		fMat[0][2]=fMat[2][0];
		fMat[2][0]=fTemp;

		fTemp=fMat[0][3];
		fMat[0][3]=fMat[3][0];
		fMat[3][0]=fTemp;

		fTemp=fMat[1][2];
		fMat[1][2]=fMat[2][1];
		fMat[2][1]=fTemp;

		fTemp=fMat[1][3];
		fMat[1][3]=fMat[3][1];
		fMat[3][1]=fTemp;

		fTemp=fMat[2][3];
		fMat[2][3]=fMat[3][2];
		fMat[3][2]=fTemp;

		return *this;
	};

	void LookAt(Vec3 &vecPos, Vec3 &vecTarget)
	{
		Vec4 vecX;
		Vec4 vecY;
		Vec4 vecZ;

		vecZ = vecTarget - vecPos;

		if( vecZ.Normalise()==0.f )
		{
			Init();
		}
		else
		{
			if(qfabs(vecZ.GetY())>0.9999f)
			{
				vecX.Set(1.f,0.f,0.f,0.f);
				vecY=vecZ.CrossProd(vecX);
				vecY.Normalise();
				
				vecX=vecZ.CrossProd(vecY);
				vecX.Normalise();
			}
			else
			{
				vecY.Set(0.f,1.f,0.f,0.f);
				vecX=vecZ.CrossProd(vecY);
				vecX.Normalise();
				
				vecY=vecX.CrossProd(vecZ);
				vecY.Normalise();
		
			}
			
			vecX.SetW(0.f);
			vecY.SetW(0.f);
			vecZ.SetW(0.f);
			SetColumn(0,vecX);
			SetColumn(1,vecY);
			SetColumn(2,vecZ);
		}

		SetVal(3,0, vecPos.GetX() );
		SetVal(3,1, vecPos.GetY() );
		SetVal(3,2, vecPos.GetZ() );
		SetVal(3,3, 1.f );
	};

	void Orth()
	{
		Vec4 Col;
		u32 i;

		for(i=0;i<3;i++)
		{
			Col = GetColumn(i);
			Col.Normalise();
			SetColumn(i, Col);
		}
	};

#if 0
	Matrix &QuaternionToMatrix(const Vec4 &q)
	{
		/*
		If q is guaranteed to be a unit quaternion, s will always
		be 1.  In that case, this calculation can be optimized out.
		*/

		float fnorm=q.GetX()*q.GetX() + q.GetY()*q.GetY() + q.GetZ()*q.GetZ() + q.GetW()*q.GetW(); 
		float s = (fnorm > 0) ? 2.f/fnorm : 0;
		//double
		//norm = q[kX]*q[kX] + q[kY]*q[kY] + q[kZ]*q[kZ] + q[kW]*q[kW],
		//s = (norm > 0) ? 2/norm : 0,
		
		/*
		Precalculate coordinate products
		*/
		float xx = q.GetX() * q.GetX() * s;
		float yy = q.GetY() * q.GetY() * s;
		float zz = q.GetZ() * q.GetZ() * s;
		float xy = q.GetX() * q.GetY() * s;
		float xz = q.GetX() * q.GetZ() * s;
		float yz = q.GetY() * q.GetZ() * s;
		float wx = q.GetW() * q.GetX() * s;
		float wy = q.GetW() * q.GetY() * s;
		float wz = q.GetW() * q.GetZ() * s;
		/*xx = q[kX] * q[kX] * s,
		yy = q[kY] * q[kY] * s,
		zz = q[kZ] * q[kZ] * s,
		xy = q[kX] * q[kY] * s,
		xz = q[kX] * q[kZ] * s,
		yz = q[kY] * q[kZ] * s,
		wx = q[kW] * q[kX] * s,
		wy = q[kW] * q[kY] * s,
		wz = q[kW] * q[kZ] * s;*/

		/*
		Calculate 3x3 matrix from orthonormal basis
		*/

		/*
		x axis
		*/
		SetVal(0,0, 1.0f - (yy + zz));
		SetVal(0,1, xy + wz);
		SetVal(0,2, xz - wy);

	/*	m[kX][kX] = 1.0 - (yy + zz);
		m[kY][kX] = xy + wz;
		m[kZ][kX] = xz - wy;*/
		
		/*
		y axis
		*/
		SetVal(1,0, xy - wz);
		SetVal(1,1, 1.0f - (xx + zz));
		SetVal(1,2, yz + wx);

	/*	m[kX][kY] = xy - wz;
		m[kY][kY] = 1.0 - (xx + zz);
		m[kZ][kY] = yz + wx;*/
		
		/*
		z axis
		*/
		SetVal(2,0, xz + wy);
		SetVal(2,1, yz - wx);
		SetVal(2,2, 1.0f - (xx + yy));

	/*	m[kX][kZ] = xz + wy;
		m[kY][kZ] = yz - wx;
		m[kZ][kZ] = 1.0 - (xx + yy);*/

		/*
		4th row and column of 4x4 matrix
		Translation and scale are not stored in quaternions, so these
		values are set to default (no scale, no translation).
		For systems where m comes pre-loaded with scale and translation
		factors, this code can be excluded.
		*/
		
		SetVal(0,3, 0);
		SetVal(1,3, 0);
		SetVal(2,3, 0);
		SetVal(3,0, 0);
		SetVal(3,1, 0);
		SetVal(3,2, 0);
		SetVal(3,3, 1.f);

		return *this;
	//	m[kW][kX] = m[kW][kY] = m[kW][kZ] = m[kX][kW] = m[kY][kW] = m[kZ][kW] = 0.0;
	//	m[kW][kW] = 1.0;

	}

	
	void ToQuaternion(Vec4 &q) const
	{
		/*
		This code can be optimized for m[kW][kW] = 1, which 
		should always be true.  This optimization is excluded
		here for clarity.
		*/
		
		float Tr = GetVal(0,0) + GetVal(1,1) + GetVal(2,2) + 1.f,fourD;
		int i,j,k;
		
		/*
		w >= 0.5 ?
		*/
		if (Tr >= 1.0)
		{
			fourD = 2.0f*(float)sqrt(Tr);
			q.SetW(fourD/4.0f);
			fourD=1.f/fourD;
			q.SetX( (GetVal(1,2) - GetVal(2,1))*fourD );
			q.SetY( (GetVal(2,0) - GetVal(0,2))*fourD );
			q.SetZ( (GetVal(0,1) - GetVal(1,0))*fourD );
		}
		else
		{
			/*
			Find the largest component.  
			*/
			if (GetVal(0,0) > GetVal(1,1))
			{
				i = 0;
			}
			else
			{
				i = 1;
			}
			if (GetVal(2,2) > GetVal(i,i))
			{
				i = 2;
			}
			
			/*
			Set j and k to point to the next two components
			*/
			j = (i+1)%3;
			k = (j+1)%3;

			/*
			fourD = 4 * largest component
			*/
			fourD = 2.0f*(float)sqrt(GetVal(i,i) - GetVal(j,j) - GetVal(k,k) + 1.0f );

			/*
			Set the largest component
			*/
			switch(i)
			{
			case 0:
				q.SetX( fourD/4.0f );
				break;
			case 1:
				q.SetY( fourD/4.0f );
				break;
			case 2:
				q.SetZ( fourD/4.0f );
				break;
			}

			switch(j)
			{
			case 0:
				q.SetX( (GetVal(i,j) + GetVal(j,i))/fourD );
				break;
			case 1:
				q.SetY( (GetVal(i,j) + GetVal(j,i))/fourD );
				break;
			case 2:
				q.SetZ( (GetVal(i,j) + GetVal(j,i))/fourD );
				break;
			}

			switch(k)
			{
			case 0:
				q.SetX( (GetVal(i,k) + GetVal(k,j))/fourD );
				break;
			case 1:
				q.SetY( (GetVal(i,k) + GetVal(k,j))/fourD );
				break;
			case 2:
				q.SetZ( (GetVal(i,k) + GetVal(k,j))/fourD );
				break;
			}
			
			
			/*
			Calculate remaining components
			*/
			q.SetW( (GetVal(j,k) - GetVal(k,j))/fourD );
		}
	}
#endif

	const Matrix &operator=(const Matrix &mat)
	{
		SetColumn(0,mat.GetColumn(0));
		SetColumn(1,mat.GetColumn(1));
		SetColumn(2,mat.GetColumn(2));
		SetColumn(3,mat.GetColumn(3));

		return *this;
	};

	Matrix operator*(Matrix &mat)
	{
		float fMatNew[4][4];
		int i;
		
		for(i=0;i<4;i++)
		{
			fMatNew[i][0]=	fMat[i][0]*mat.GetColumn(0).GetX() + 
							fMat[i][1]*mat.GetColumn(1).GetX() + 
							fMat[i][2]*mat.GetColumn(2).GetX() +
							fMat[i][3]*mat.GetColumn(3).GetX();

			fMatNew[i][1]=	fMat[i][0]*mat.GetColumn(0).GetY() + 
							fMat[i][1]*mat.GetColumn(1).GetY() + 
							fMat[i][2]*mat.GetColumn(2).GetY() +
							fMat[i][3]*mat.GetColumn(3).GetY();

			fMatNew[i][2]=	fMat[i][0]*mat.GetColumn(0).GetZ() + 
							fMat[i][1]*mat.GetColumn(1).GetZ() + 
							fMat[i][2]*mat.GetColumn(2).GetZ() +
							fMat[i][3]*mat.GetColumn(3).GetZ();

			fMatNew[i][3]=	fMat[i][0]*mat.GetColumn(0).GetW() + 
							fMat[i][1]*mat.GetColumn(1).GetW() + 
							fMat[i][2]*mat.GetColumn(2).GetW() +
							fMat[i][3]*mat.GetColumn(3).GetW();
		};


		
		return Matrix(&fMatNew[0][0]);
	};

	Matrix operator+(const Matrix &mat) const
	{
		float fMatNew[4][4];
		int i;
		
		for(i=0;i<4;i++)
		{
			fMatNew[i][0]=	fMat[i][0]+mat.GetColumn(0).GetX();

			fMatNew[i][1]=	fMat[i][1]+mat.GetColumn(1).GetY();

			fMatNew[i][2]=	fMat[i][2]+mat.GetColumn(2).GetZ();

			fMatNew[i][3]=	fMat[i][3]+mat.GetColumn(3).GetW();
		};

		return Matrix(&fMatNew[0][0]);
	};

	Matrix operator*(const Matrix &mat) const
	{
		float fMatNew[4][4];
		int i;
		
		for(i=0;i<4;i++)
		{
			fMatNew[i][0]=	fMat[i][0]*mat.GetColumn(0).GetX() + 
							fMat[i][1]*mat.GetColumn(1).GetX() + 
							fMat[i][2]*mat.GetColumn(2).GetX() +
							fMat[i][3]*mat.GetColumn(3).GetX();

			fMatNew[i][1]=	fMat[i][0]*mat.GetColumn(0).GetY() + 
							fMat[i][1]*mat.GetColumn(1).GetY() + 
							fMat[i][2]*mat.GetColumn(2).GetY() +
							fMat[i][3]*mat.GetColumn(3).GetY();

			fMatNew[i][2]=	fMat[i][0]*mat.GetColumn(0).GetZ() + 
							fMat[i][1]*mat.GetColumn(1).GetZ() + 
							fMat[i][2]*mat.GetColumn(2).GetZ() +
							fMat[i][3]*mat.GetColumn(3).GetZ();

			fMatNew[i][3]=	fMat[i][0]*mat.GetColumn(0).GetW() + 
							fMat[i][1]*mat.GetColumn(1).GetW() + 
							fMat[i][2]*mat.GetColumn(2).GetW() +
							fMat[i][3]*mat.GetColumn(3).GetW();
		};

		return Matrix(&fMatNew[0][0]);
	};

	Matrix operator*(const float xScale) const
	{
		float fMatNew[4][4];
		int i;
		
		for(i=0;i<4;i++)
		{
			fMatNew[i][0]=	fMat[i][0]*xScale;

			fMatNew[i][1]=	fMat[i][1]*xScale;

			fMatNew[i][2]=	fMat[i][2]*xScale;

			fMatNew[i][3]=	fMat[i][3]*xScale;
		};

		return Matrix(&fMatNew[0][0]);
	};

	const Vec4 operator*(const Vec4 &vec) const 
	{
		return Vec4
		(
			fMat[0][0]*vec.GetX() + fMat[1][0]*vec.GetY() +
			fMat[2][0]*vec.GetZ() + fMat[3][0]*vec.GetW(),

			fMat[0][1]*vec.GetX() + fMat[1][1]*vec.GetY() +
			fMat[2][1]*vec.GetZ() + fMat[3][1]*vec.GetW(),

			fMat[0][2]*vec.GetX() + fMat[1][2]*vec.GetY() +
			fMat[2][2]*vec.GetZ() + fMat[3][2]*vec.GetW(),

			fMat[0][3]*vec.GetX() + fMat[1][3]*vec.GetY() +
			fMat[2][3]*vec.GetZ() + fMat[3][3]*vec.GetW()
		);
	};

	const Vec3 operator*(const Vec3 &vec) const
	{
		return Vec3
		(
			fMat[0][0]*vec.GetX() + fMat[1][0]*vec.GetY() +
			fMat[2][0]*vec.GetZ(),

			fMat[0][1]*vec.GetX() + fMat[1][1]*vec.GetY() +
			fMat[2][1]*vec.GetZ(),

			fMat[0][2]*vec.GetX() + fMat[1][2]*vec.GetY() +
			fMat[2][2]*vec.GetZ()
		);
	};

	const Matrix operator-() 
	{ 
		return Matrix(
			-fMat[0][0],-fMat[0][1],-fMat[0][2],-fMat[0][3],
			-fMat[1][0],-fMat[1][1],-fMat[1][2],-fMat[1][3],
			-fMat[2][0],-fMat[2][1],-fMat[2][2],-fMat[2][3],
			-fMat[3][0],-fMat[3][1],-fMat[3][2],-fMat[3][3]); 
	};

private:
	float fMat[4][4];
};


#endif