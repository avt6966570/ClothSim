//
// Basic type definitions and wrappers for our physics vector and matrix representations.
//

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <d3dx9.h>

typedef float SCALAR;

// Wrapper around the D3DXVECTOR3 structure and D3DX helper functions to make it nicer from a OO standpoint.
struct VECTOR3 : public D3DXVECTOR3 {
	inline VECTOR3() { }
	inline VECTOR3(const D3DXVECTOR3& v) { x = v.x; y = v.y; z = v.z; }
	inline VECTOR3(const VECTOR3& v) { *this = v; }
	inline VECTOR3(SCALAR inx, SCALAR iny, SCALAR inz) { x=inx; y=iny; z=inz; }
	
	inline SCALAR length() const {
		return sqrtf(x*x+y*y+z*z); 
	}

	inline VECTOR3 normalize() const {
		SCALAR l = length(); return VECTOR3(x/l, y/l, z/l);
	}

	inline SCALAR operator [] (int i) const {
		switch(i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default: // error!
			return 0.0;
		}		
	}

	inline static SCALAR dot(const VECTOR3& a, const VECTOR3& b) {	
		return D3DXVec3Dot(&a, &b);
	}

	inline static VECTOR3 cross(const VECTOR3& a, const VECTOR3& b) {
		VECTOR3 result;
		D3DXVec3Cross(&result, &a, &b);
		return result; 
	}

};

static VECTOR3 VZERO = VECTOR3(0.0f, 0.0f, 0.0f);

struct MATRIX3X3 {
	inline MATRIX3X3() { }

	inline MATRIX3X3(  const SCALAR r1c1, const SCALAR r1c2, const SCALAR r1c3,
				const SCALAR r2c1, const SCALAR r2c2, const SCALAR r2c3,
				const SCALAR r3c1, const SCALAR r3c2, const SCALAR r3c3) {
		m_00 = r1c1; m_01 = r1c2; m_02 = r1c3;
		m_10 = r2c1; m_11 = r2c2; m_12 = r2c3;
		m_20 = r3c1; m_21 = r3c2; m_22 = r3c3;
	};

	inline MATRIX3X3(const VECTOR3& row1, const VECTOR3& row2, const VECTOR3& row3) {
		m_00 = row1[0]; m_01 = row1[1]; m_02 = row1[2];
		m_10 = row2[0]; m_11 = row2[1]; m_12 = row2[2];
		m_20 = row3[0]; m_21 = row3[1]; m_22 = row3[2];
	}

	inline MATRIX3X3(const MATRIX3X3& m) {
		m_00 = m.m_00; m_01 = m.m_01; m_02 = m.m_02;
		m_10 = m.m_10; m_11 = m.m_11; m_12 = m.m_12;
		m_20 = m.m_20; m_21 = m.m_21; m_22 = m.m_22;
	}

	inline VECTOR3 operator * (const VECTOR3& v) const {
		return VECTOR3( v.x*m_00 + v.y*m_01 + v.z*m_02, 
						v.x*m_10 + v.y*m_11 + v.z*m_12,
						v.x*m_20 + v.y*m_21 + v.z*m_22);
	}

	inline MATRIX3X3 operator * (const MATRIX3X3& m) const {
		return MATRIX3X3(   m_00*m.m_00 + m_01*m.m_10 + m_02*m.m_20, // r0c0
							m_00*m.m_01 + m_01*m.m_11 + m_02*m.m_21, // r0c1	
							m_00*m.m_02 + m_01*m.m_12 + m_02*m.m_22, // r0c2	

							m_10*m.m_00 + m_11*m.m_10 + m_12*m.m_20, // r1c0
							m_10*m.m_01 + m_11*m.m_11 + m_12*m.m_21, // r1c1
							m_10*m.m_02 + m_11*m.m_12 + m_12*m.m_22, // r1c2

							m_20*m.m_00 + m_21*m.m_10 + m_22*m.m_20, // r2c0
							m_20*m.m_01 + m_21*m.m_11 + m_22*m.m_21, // r2c1
							m_20*m.m_02 + m_21*m.m_12 + m_22*m.m_22);// r2c2
	}

	inline MATRIX3X3 operator * (const SCALAR x) const {
		return MATRIX3X3(m_00*x, m_01*x, m_02*x,
						 m_10*x, m_11*x, m_12*x,
						 m_20*x, m_21*x, m_22*x);
	}

	inline MATRIX3X3& operator *= (const SCALAR x) {
		m_00 *= x; m_01 *= x; m_02 *= x;
		m_10 *= x; m_11 *= x; m_12 *= x;
		m_20 *= x; m_21 *= x; m_22 *= x;

		return *this;
	}

	inline friend MATRIX3X3 operator * (const SCALAR x, const MATRIX3X3& m);


	SCALAR m_00, m_01, m_02,
		   m_10, m_11, m_12,
		   m_20, m_21, m_22;
};

inline MATRIX3X3 operator * (const SCALAR x, const MATRIX3X3&m) {
		return MATRIX3X3(m.m_00*x, m.m_01*x, m.m_02*x,
						 m.m_10*x, m.m_11*x, m.m_12*x,
						 m.m_20*x, m.m_21*x, m.m_22*x);
}

struct CLOTHVERTEX
{
	VECTOR3 p;
	VECTOR3 n;
	D3DCOLOR kd;
};

// Taken from <dxutil.h>
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#endif