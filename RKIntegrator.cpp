// RKIntegrator.cpp: implementation of the RKIntegrator class.
//
//////////////////////////////////////////////////////////////////////

#include "RKIntegrator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RKIntegrator::RKIntegrator(const int iParticles, 
								 const SCALAR *inMasses, const SCALAR *inInvMasses,
								 CLOTHVERTEX *outPositions, VECTOR3 *outVelocities)
					: ForceIntegrator(iParticles, inMasses, inInvMasses, outPositions, outVelocities)
{
	m_Accumulator = new VECTOR3[iParticles];

	m_tmpP = new CLOTHVERTEX[iParticles];
	m_tmpV = new VECTOR3[iParticles];
	m_tmpDeltaV1 = new VECTOR3[iParticles];
	m_tmpDeltaV2 = new VECTOR3[iParticles];
	m_tmpDeltaV3 = new VECTOR3[iParticles];
	m_tmpDeltaV4 = new VECTOR3[iParticles];
	m_tmpDeltaX1 = new VECTOR3[iParticles];
	m_tmpDeltaX2 = new VECTOR3[iParticles];
	m_tmpDeltaX3 = new VECTOR3[iParticles];
	m_tmpDeltaX4 = new VECTOR3[iParticles];
}

RKIntegrator::~RKIntegrator()
{
	SAFE_DELETE_ARRAY(m_tmpP);
	SAFE_DELETE_ARRAY(m_tmpV);

	SAFE_DELETE_ARRAY(m_tmpDeltaV1);
	SAFE_DELETE_ARRAY(m_tmpDeltaV2);
	SAFE_DELETE_ARRAY(m_tmpDeltaV3);
	SAFE_DELETE_ARRAY(m_tmpDeltaV4);
	SAFE_DELETE_ARRAY(m_tmpDeltaX1);
	SAFE_DELETE_ARRAY(m_tmpDeltaX2);
	SAFE_DELETE_ARRAY(m_tmpDeltaX3);
	SAFE_DELETE_ARRAY(m_tmpDeltaX4);

	SAFE_DELETE_ARRAY(m_Accumulator);
}

void RKIntegrator::ZeroAccumulatorAndSumForces(const CLOTHVERTEX *inPositions,
											   const VECTOR3 *inVelocities,
											   list<Force *> &extForces, 
											   list<Force *> &intForces)
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
		force->Add(m_iParticles, inPositions, inVelocities, m_inMasses, m_Accumulator);
	}

	for(li = intForces.begin(); li != intForces.end(); li++) {
		Force *force = *li;
		force->Add(m_iParticles, inPositions, inVelocities, m_inMasses, m_Accumulator);
	}
}

void RKIntegrator::Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep)
{
	VECTOR3 deltaV, V, deltaX;
	ZeroAccumulatorAndSumForces(m_outPositions, m_outVelocities, extForces, intForces);
	// Calculate incremental change in velocity and positions
	for(int i = 0; i < m_iParticles; i++) {
		deltaV = m_Accumulator[i]*m_inInvMasses[i]*timeStep;
		deltaX =  m_outVelocities[i]*timeStep;

		m_tmpDeltaV1[i] = deltaV;
		m_tmpDeltaX1[i] = deltaX;

		m_tmpV[i] = m_outVelocities[i] + (0.5f*deltaV);
		m_tmpP[i].p = m_outPositions[i].p + (0.5f*deltaX);
	}

// ==============================
	ZeroAccumulatorAndSumForces(m_tmpP, m_tmpV, extForces, intForces);
	// Calculate incremental change in velocity and positions
	for( i = 0; i < m_iParticles; i++) {
		deltaV = m_Accumulator[i]*m_inInvMasses[i]*timeStep;
		deltaX = (m_tmpV[i])*timeStep;
		 
		m_tmpDeltaV2[i] = deltaV;
		m_tmpDeltaX2[i] = deltaX;

		m_tmpV[i] = m_outVelocities[i] + (0.5f*deltaV);
		m_tmpP[i].p = m_outPositions[i].p + (0.5f*deltaX);
	}

// ==============================
	ZeroAccumulatorAndSumForces(m_tmpP, m_tmpV, extForces, intForces);
	// Calculate incremental change in velocity and positions
	for( i = 0; i < m_iParticles; i++) {
		deltaV = m_Accumulator[i]*m_inInvMasses[i]*timeStep;
		deltaX = m_tmpV[i]*timeStep;

		m_tmpDeltaV3[i] = deltaV;
		m_tmpDeltaX3[i] = deltaX;

		m_tmpV[i] = m_outVelocities[i] + (deltaV);
		m_tmpP[i].p = m_outPositions[i].p + (deltaX);
	}	
	

// ==============================
	ZeroAccumulatorAndSumForces(m_tmpP, m_tmpV, extForces, intForces);
	// Calculate incremental change in velocity and positions
	for( i = 0; i < m_iParticles; i++) {
		deltaV = m_Accumulator[i]*m_inInvMasses[i]*timeStep;
		deltaX = m_tmpV[i]*timeStep;

		m_tmpDeltaV4[i] = deltaV;
		m_tmpDeltaX4[i] = deltaX;
	}
	
// ==============================

	for(i = 0; i < m_iParticles; i++) {
		deltaV = (1.0f/6.0f)*(m_tmpDeltaV1[i] + 2.0f*m_tmpDeltaV2[i] + 2.0f*m_tmpDeltaV3[i] + m_tmpDeltaV4[i]);
		deltaX = (1.0f/6.0f)*(m_tmpDeltaX1[i] + 2.0f*m_tmpDeltaX2[i] + 2.0f*m_tmpDeltaX3[i] + m_tmpDeltaX4[i]);

		m_outVelocities[i] += deltaV;
		m_outPositions[i].p += deltaX;
	}
}

