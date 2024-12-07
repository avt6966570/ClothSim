// VerletIntegrator.cpp: implementation of the VerletIntegrator class.
//
//////////////////////////////////////////////////////////////////////

#include "VerletIntegrator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VerletIntegrator::VerletIntegrator(const int iParticles, 
								 const SCALAR *inMasses, const SCALAR *inInvMasses,
								 CLOTHVERTEX *outPositions, VECTOR3 *outVelocities)
					: ForceIntegrator(iParticles, inMasses, inInvMasses, outPositions, outVelocities)
{
	m_Accumulator = new VECTOR3[iParticles];
	m_PosOld = new VECTOR3[iParticles];
}

VerletIntegrator::~VerletIntegrator()
{
	SAFE_DELETE_ARRAY(m_Accumulator);
	SAFE_DELETE_ARRAY(m_PosOld);
}

void VerletIntegrator::Initialize()
{
	for(int i = 0; i < m_iParticles; i++) {
		m_PosOld[i] = m_outPositions[i].p;
	}
}

void VerletIntegrator::Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep)
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
		VECTOR3 pos = m_outPositions[i].p;
		m_outPositions[i].p += m_outPositions[i].p - m_PosOld[i] + m_Accumulator[i]*m_inInvMasses[i]*timeStep*timeStep;
		m_PosOld[i] = pos;

		m_outVelocities[i] = (m_outPositions[i].p-pos)/timeStep;

	}
}