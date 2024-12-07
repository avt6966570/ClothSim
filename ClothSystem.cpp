// ClothSystem.cpp
// ===============
// 

#include "ClothSystem.h"
#include "UniformForce.h"
#include "SpringForce.h"

// Constructor: Allocate heap data storage.
ClothSystem::ClothSystem(int iRows, int iColumns)
{
	m_iRows = iRows;
	m_iColumns = iColumns;

	m_Positions = new CLOTHVERTEX[iRows*iColumns];
	m_Velocities = new VECTOR3[iRows*iColumns];

	m_Masses = new SCALAR[iRows*iColumns];
	m_MassesInv = new SCALAR[iRows*iColumns];

	m_pIntegrator = NULL;

}

ClothSystem::~ClothSystem()
{	
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	
	SAFE_DELETE(m_pIntegrator);

	SAFE_DELETE_ARRAY(m_Positions);
	SAFE_DELETE_ARRAY(m_Velocities);

	SAFE_DELETE_ARRAY(m_Masses);
	SAFE_DELETE_ARRAY(m_MassesInv);

	list<Force *>::iterator li;
	for(li = m_ForcesExt.begin(); li != m_ForcesExt.end(); li++) {
		SAFE_DELETE(*li);
	}

	for(li = m_ForcesInt.begin(); li != m_ForcesInt.end(); li++) {
		SAFE_DELETE(*li);
	}
}


HRESULT ClothSystem::SetRestState(const Settings& settings)
{
	D3DXMATRIX matRotation;
	D3DXMatrixRotationAxis(&matRotation, &settings.vInitialRotationAxis, settings.fInitialRotationAngle);

	D3DXVECTOR4 vNormal(0.0f, 0.0f, -1.0f, 1.0f);
	D3DXVec4Transform(&vNormal, &vNormal, &matRotation);
	
	// Set initial particle positions, velocities, normal, diffuse color
	int row, col;

	for(row = 0; row < m_iRows; row++) {
		for(col = 0; col < m_iColumns; col++) {
			D3DXVECTOR4 vPosition( 
				((float) col / (float) (m_iColumns-1) - 0.5f)*settings.fxScale,
				((float) row / (float) (m_iRows-1) - 0.5f)*settings.fyScale,
				0.0f, 
				1.0f);
			D3DXVec4Transform(&vPosition, &vPosition, &matRotation);
			m_Positions[ArrayIndex(row, col)].p.x = vPosition.x + settings.vInitialPosition.x;
			m_Positions[ArrayIndex(row, col)].p.y = vPosition.y + settings.vInitialPosition.y;
			m_Positions[ArrayIndex(row, col)].p.z = vPosition.z + settings.vInitialPosition.z;

			m_Positions[ArrayIndex(row, col)].n.x = vNormal.x;
			m_Positions[ArrayIndex(row, col)].n.y = vNormal.y;
			m_Positions[ArrayIndex(row, col)].n.z = vNormal.z;

			if((row+col)%2) {
				m_Positions[ArrayIndex(row, col)].kd = D3DCOLOR_ARGB(255,255,255,255);
			} else {
				m_Positions[ArrayIndex(row, col)].kd = D3DCOLOR_ARGB(255,255,0,0);
			}

			m_Velocities[ArrayIndex(row, col)] = VZERO;
		}
	}

	
	return S_OK;
}


//
// Initialize initial positions, velocities, forces.
//

HRESULT ClothSystem::Initialize(const Settings& settings, bool bSetRestState)
{
	// Create our integrator object
	SAFE_DELETE(m_pIntegrator);

	switch(settings.integrator) {
	case INT_EULER:
		m_pIntegrator = new EulerIntegrator(m_iRows*m_iColumns, m_Masses, m_MassesInv, 
			m_Positions, m_Velocities);
		break;
	case INT_VERLET:
		m_pIntegrator = new VerletIntegrator(m_iRows*m_iColumns, m_Masses, m_MassesInv, 
			m_Positions, m_Velocities);
		break;
	case INT_RK:
		m_pIntegrator = new RKIntegrator(m_iRows*m_iColumns, m_Masses, m_MassesInv, 
			m_Positions, m_Velocities);
		break;
	}

	// Set initial velocity and position of particles if required
	if(bSetRestState)
		SetRestState(settings);

	// Set particle masses
	for(int row = 0; row < m_iRows; row++) {
		for(int col = 0; col < m_iColumns; col++) {
			m_Masses[ArrayIndex(row, col)] = settings.kMassPerParticle;
			m_MassesInv[ArrayIndex(row, col)] = (1.0f / settings.kMassPerParticle);
		}
	}

	// Add constraints
	if(settings.bConstrainTopLeft) {
		m_MassesInv[ArrayIndex(m_iRows-1, 0)] = 0.0f;
		m_Velocities[ArrayIndex(m_iRows-1, 0)] = VZERO;
	}

	if(settings.bConstrainTopRight) {
		m_MassesInv[ArrayIndex(m_iRows-1, m_iColumns-1)] = 0.0f;
		m_Velocities[ArrayIndex(m_iRows-1, m_iColumns-1)] = VZERO;
	}

	if(settings.bConstrainBotLeft) {
		m_MassesInv[ArrayIndex(0, 0)] = 0.0f;
		m_Velocities[ArrayIndex(0, 0)] = VZERO;
	}

	if(settings.bConstrainBotRight) {
		m_MassesInv[ArrayIndex(0, m_iColumns-1)] = 0.0f;
		m_Velocities[ArrayIndex(0, m_iColumns-1)] = VZERO;
	}

	// (Re-)initialize integrator if needed
	m_pIntegrator->Initialize();	


	// Add forces
	// ==========
	list<Force *>::iterator li;
	for(li = m_ForcesExt.begin(); li != m_ForcesExt.end(); li++) {
		SAFE_DELETE(*li);
	}
	m_ForcesExt.clear();
	for(li = m_ForcesInt.begin(); li != m_ForcesInt.end(); li++) {
		SAFE_DELETE(*li);
	}
	m_ForcesInt.clear();


	// Gravity
	Force *gravityForce = new UniformForce(VECTOR3(0.0, -1.0f*(settings.kGravityAccel), 0.0f));
	m_ForcesExt.push_back(gravityForce);

	// Spring Forces
	//       x
	//      /
	//     / 
	//    /
	//   /          <= Shear
	// OO----x      <= Stretch
	// OO---------x <= Bend
	// ||\
	// || \
	// ||  \
	// ||   \
	// |x    x
	// |
	// |
	// x
	for(row = 0; row < m_iRows; row++) {
		for(int col = 0; col < m_iColumns; col++) {
			Force *stretchForce, *shearForce, *bendForce;

			// Stretch springs
			if(settings.bStretch) {
				if(row < m_iRows-1) {
					stretchForce = new SpringForce( VECTOR3(POSITION(row+1, col)-POSITION(row, col)).length(),
													settings.kStretchSpring, settings.kStretchDamp,
													ArrayIndex(row, col),
													ArrayIndex(row+1, col));
					m_ForcesInt.push_back(stretchForce);
				}
				if(col < m_iColumns-1) {
					stretchForce = new SpringForce( VECTOR3(POSITION(row, col+1)-POSITION(row, col)).length(),
													settings.kStretchSpring, settings.kStretchDamp,
													ArrayIndex(row, col),
													ArrayIndex(row, col+1));
					m_ForcesInt.push_back(stretchForce);
				}
			}
		
			// Shear springs
			if(settings.bShear) {
				if(row > 0 && col < m_iColumns-1) {
					shearForce = new SpringForce( VECTOR3(POSITION(row-1, col+1)-POSITION(row, col)).length(),
													settings.kShearSpring, settings.kShearDamp,
													ArrayIndex(row, col),
													ArrayIndex(row-1, col+1));
					m_ForcesInt.push_back(shearForce);
				}
				if(row < m_iRows-1 && col < m_iColumns-1) {
					shearForce = new SpringForce( VECTOR3(POSITION(row+1, col+1)-POSITION(row, col)).length(),
													settings.kShearSpring, settings.kShearDamp,
													ArrayIndex(row, col),
													ArrayIndex(row+1, col+1));
					m_ForcesInt.push_back(shearForce);
				}
			}


			// Bend springs
			if(settings.bBend) {
				if(row < m_iRows-2) {
					bendForce = new SpringForce( VECTOR3(POSITION(row+2, col)-POSITION(row, col)).length(),
													settings.kBendSpring, settings.kBendDamp,
													ArrayIndex(row, col),
													ArrayIndex(row+2, col));
					m_ForcesInt.push_back(bendForce);
				}
				if(col < m_iColumns-2) {
					bendForce = new SpringForce( VECTOR3(POSITION(row, col+2)-POSITION(row, col)).length(),
													settings.kBendSpring, settings.kBendDamp,
													ArrayIndex(row, col),
													ArrayIndex(row, col+2));
					m_ForcesInt.push_back(bendForce);
				}
			}
		}
	}



	return S_OK;
}

///////////////////////////////////////////////////////


//
// Initialize device-dependent DX9 objects; called once upon startup after
// DX9 device initialization.
//

HRESULT ClothSystem::InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr;

    // Keep a local copy of the device
    m_pDevice = pDevice;

	// Since our index buffer doesn't change, we'll place it in the managed memory pool
	// to avoid dealing with restoring it.
	if(FAILED(hr = pDevice->CreateIndexBuffer( (m_iRows-1)*(m_iColumns-1)*6 * sizeof(WORD),
                                      D3DUSAGE_WRITEONLY,
                                      D3DFMT_INDEX16, D3DPOOL_MANAGED,
                                      &m_pIndexBuffer, NULL )))
		return hr;

	WORD *pIndices;
	m_pIndexBuffer->Lock( 0, 0, (void **)&pIndices, 0 );
	WORD curIndex = 0;
	for(int row = 0; row < m_iRows-1; row++) {
		for(int col = 0; col < m_iColumns-1; col++) {
			pIndices[curIndex++] = (WORD) ArrayIndex(row, col);
			pIndices[curIndex++] = (WORD) ArrayIndex(row, col+1);
			pIndices[curIndex++] = (WORD) ArrayIndex(row+1, col);

			pIndices[curIndex++] = (WORD) ArrayIndex(row+1, col);
			pIndices[curIndex++] = (WORD) ArrayIndex(row, col+1);
			pIndices[curIndex++] = (WORD) ArrayIndex(row+1, col+1);
		}
	}
	m_pIndexBuffer->Unlock();

	return S_OK;
}

//
// Release our index buffer when we're about to exit.
//
HRESULT ClothSystem::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pIndexBuffer);
	return S_OK;
}

///////////////////////////////////////////////////////


//
// Restore device objects; called upon initialization or 
// restoration of a lost device.
//
HRESULT ClothSystem::RestoreDeviceObjects()
{
	HRESULT hr;

	// Our vertex buffer is dynamic, so we place it in default driver-optimal memory 
	// for performance reasons and need to restore it if the device is lost.
	if(FAILED(hr = m_pDevice->CreateVertexBuffer( (m_iRows*m_iColumns) * sizeof(CLOTHVERTEX),
                                      D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                      D3DFVF_CLOTHVERTEX, D3DPOOL_DEFAULT,
                                      &m_pVertexBuffer, NULL )))
		return hr;

	return S_OK;
}

//
// Release our vertex buffer upon lost device.
//
HRESULT ClothSystem::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pVertexBuffer);
	return S_OK;
}


///////////////////////////////////////////////////////

// Calculates new positions, velocities, and normals.
void ClothSystem::Update(SCALAR timeStep)
{
	m_pIntegrator->Update(m_ForcesExt, m_ForcesInt, timeStep);

	bool ig, il, jg, jl;
	VECTOR3 p1, p2, p3, p4, p5, p6, p7;
	VECTOR3 n1, n2, n3, n4, n5, n6, n;

	// Calculate new normals for lighting purposes.
	// Adapted from Dean Macri, "Intel Real-Time Cloth Simulation"
	for(int row=0; row<m_iRows; row++ )
	{
		for(int col=0; col<m_iColumns; col++ )
		{

			ig = (row > 0);
			il = (row < m_iRows-1);
			jg = (col > 0);
			jl = (col < m_iColumns-1);
			if( ig )
			{
				p1 = m_Positions[ArrayIndex(row-1, col)].p; 
				if( jl )
				{
					p2 = m_Positions[ArrayIndex(row-1, col+1)].p;
				}
			}
			if( jg )
			{
				p3 = m_Positions[ArrayIndex(row, col-1)].p;
				if( il )
				{
					p6 = m_Positions[ArrayIndex(row+1, col-1)].p;
				}
			}
			p4 = m_Positions[ArrayIndex(row, col)].p;
			if( jl )
			{
				p5 = m_Positions[ArrayIndex(row, col+1)].p;
			}
			if( il )
			{
				p7 = m_Positions[ArrayIndex(row+1, col)].p;
			}

			n1 = n2 = n3 = n4 = n5 = n6 = VZERO;
			if( ig && jg ) {
				n1 = VECTOR3::cross( p3-p4,p1-p4 );
			}
			if( ig && jl )
			{
				n2 = VECTOR3::cross( p2-p1,p4-p1 );
				n3 = VECTOR3::cross( p4-p5,p2-p5 );
			}
			if( il && jg )
			{
				n4 = VECTOR3::cross( p4-p3,p6-p3 );
				n5 = VECTOR3::cross( p6-p7,p4-p7 );
			}
			if( il && jl ) {
				n6 = VECTOR3::cross( p5-p4,p7-p4 );
			}

			n = (n1+n2+n3+n4+n5+n6);
			m_Positions[ArrayIndex(row, col)].n = -n.normalize();
		}
	}

}

// Actually draws the object.
HRESULT ClothSystem::Render()
{
	CLOTHVERTEX *pVertices;


	// Copy our vertex data into our vertex buffer for rendering.
    m_pVertexBuffer->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
	memcpy(pVertices, m_Positions, sizeof(CLOTHVERTEX) * (m_iRows*m_iColumns));
	m_pVertexBuffer->Unlock();

	m_pDevice->SetFVF(D3DFVF_CLOTHVERTEX);
    m_pDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(CLOTHVERTEX) );
    m_pDevice->SetIndices( m_pIndexBuffer );
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (m_iRows-1)*(m_iColumns-1)*6, 0, 
		(m_iRows-1)*(m_iColumns-1)*2);

	return S_OK;
}