//
// ClothSystem.h
//
// The representation of our particle system which forms the basis for our cloth representation.
// It contains the world-space positions of each particle, their masses, their velocity, 
// their normal, as well as linked lists of the forces acting upon them.
//
// In this implementation, we also store the associated Direct3D vertex and index buffers for rendering
// but they can be easily separated out as well.
//

#ifndef _CLOTHSYSTEM_H_
#define _CLOTHSYSTEM_H_

// Prevent some spurious warnings from the STL header files.
#ifdef _MSC_VER
  #pragma warning (push, 3)
  #pragma warning (disable: 4245)
  #pragma warning (disable: 4100) 
#endif

#include <list>
using namespace std;

#ifdef _MSC_VER
  #pragma warning (pop)
#endif


#include <d3d9.h>
#include <d3dx9.h>
#include "dxutil.h"
#include "Physics.h"
#include "Force.h"
#include "Settings.h"

#include "EulerIntegrator.h"
#include "VerletIntegrator.h"
#include "RKIntegrator.h"


#define D3DFVF_CLOTHVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

#define POSITION(row, col)  (m_Positions[ArrayIndex((row), (col))].p)

class ClothSystem
{
public:
	ClothSystem(int iRows, int iColumns);
	~ClothSystem();

	HRESULT Initialize(const Settings& settings, bool bSetRestState);

	void Update(SCALAR timeStep);

	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
	HRESULT DeleteDeviceObjects();

	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	HRESULT Render();



private:
	int m_iRows, m_iColumns;

	HRESULT SetRestState(const Settings& settings);

	inline int ArrayIndex(int iRow, int iColumn) { return iRow*m_iRows + iColumn; }

	CLOTHVERTEX *m_Positions;		// In world space coordinates
	VECTOR3 *m_Velocities;		// In world space coordinates

	list<Force *> m_ForcesExt;
	list<Force *> m_ForcesInt;

	SCALAR *m_Masses;		
	SCALAR *m_MassesInv;

	ForceIntegrator *m_pIntegrator;

	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DDEVICE9 m_pDevice;
};



#endif // #ifdef _CLOTHSYSTEM_H_