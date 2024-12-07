#include <d3dx9.h>

class ClothSystem
{
public:
	ClothSystem(int iRows, int iColumns);
	~ClothSystem();

private:
	int m_iRows, m_iColumns;

	D3DXVECTOR3 *mPositions;
	D3DXVECTOR3 *mVelocities;
	D3DXVECTOR3 *mInternalForces;
	D3DXVECTOR3 *mExternalForces;

};
