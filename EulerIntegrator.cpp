// EulerIntegrator.cpp: implementation of the EulerIntegrator class.
//
//////////////////////////////////////////////////////////////////////

#include "EulerIntegrator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EulerIntegrator::EulerIntegrator(const int iParticles, 
								 const SCALAR *inMasses, const SCALAR *inInvMasses,
								 CLOTHVERTEX *outPositions, VECTOR3 *outVelocities)
					: ForceIntegrator(iParticles, inMasses, inInvMasses, outPositions, outVelocities)
{
	m_Accumulator = new VECTOR3[iParticles];
}

EulerIntegrator::~EulerIntegrator()
{
	SAFE_DELETE_ARRAY(m_Accumulator);
}

void EulerIntegrator::Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep)
{
	// Zero out forces.
	int i;
	for(i = 0; i < m_iParticles; i++) {
		m_Accumulator[i] = VZERO;
	}

	// Sum up all the internal and external forces
	list<Force *>::iterator li;
	for(li = extForces.begin(); li != extForces.end(); li++) {
		Force *force = *li;
		force->Add(m_iParticles, m_outPositions, m_outVelocities, m_inMasses, m_Accumulator);
	}

	for(li = intForces.begin(); li != intForces.end(); li++) {
		Force *force = *li;
		force->Add(m_iParticles, m_outPositions, m_outVelocities, m_inMasses, m_Accumulator);
	}

	// Calculate incremental change in velocity and positions
	for(i = 0; i < m_iParticles; i++) {
		VECTOR3 deltaV = m_Accumulator[i]*m_inInvMasses[i]*timeStep;
		VECTOR3 V = m_outVelocities[i] + deltaV;
		VECTOR3 deltaX = V*timeStep;

		m_outPositions[i].p += deltaX;
		m_outVelocities[i] = V;
	}
}