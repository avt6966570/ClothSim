// ForceIntegrator.cpp: implementation of the ForceIntegrator class.
//
//////////////////////////////////////////////////////////////////////

#include "ForceIntegrator.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ForceIntegrator::ForceIntegrator(const int iParticles, 
								 const SCALAR *inMasses, const SCALAR *inInvMasses,
								 CLOTHVERTEX *outPositions, VECTOR3 *outVelocities)
{
	m_outPositions = outPositions;
	m_outVelocities = outVelocities;
	m_inMasses = inMasses;
	m_inInvMasses = inInvMasses;
	m_iParticles = iParticles;
}

ForceIntegrator::~ForceIntegrator()
{

}

void ForceIntegrator::Initialize()
{

}