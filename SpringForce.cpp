// SpringForce.cpp: implementation of the SpringForce class.
//
//////////////////////////////////////////////////////////////////////

#include "SpringForce.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpringForce::SpringForce(const SCALAR restLength, const SCALAR springConstant, const SCALAR dampConstant,
						 const int index1, const int index2)
{
	m_restLength = restLength;
	m_springConstant = springConstant;
	m_dampConstant = dampConstant;
	m_index1 = index1;
	m_index2 = index2;
}


SpringForce::~SpringForce()
{
}

void SpringForce::Add(int nParticles, 
		const CLOTHVERTEX *inPositions, const VECTOR3 *inVelocities, const SCALAR *inMasses,
		VECTOR3 *outForces)
{
	// We don't use these parameters.  Reference them to avoid compiler warnings.
	(inMasses);
	(nParticles);

	VECTOR3 spring = inPositions[m_index1].p - inPositions[m_index2].p;
	SCALAR springLength = spring.length();
	spring *= (1.0f/springLength); // Normalize spring vector

	VECTOR3 force = (springLength - m_restLength)*m_springConstant*spring 
		+ (inVelocities[m_index1] - inVelocities[m_index2])*m_dampConstant;

	outForces[m_index2] += force;
	outForces[m_index1] -= force;
}
