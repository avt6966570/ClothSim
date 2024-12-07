// UniformForce.cpp: implementation of the UniformForce class.
//
//////////////////////////////////////////////////////////////////////

#include "UniformForce.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UniformForce::UniformForce(const VECTOR3 &force)
{
	m_force = force;
}

UniformForce::~UniformForce()
{

}

void UniformForce::Add(int nParticles, 
		const CLOTHVERTEX *inPositions, const VECTOR3 *inVelocities, const SCALAR *inMasses,
		VECTOR3 *outForces)
{
	// We don't use these parameters.  Reference them to avoid compiler warnings.
	(inVelocities);
	(inPositions);

	
	for(int i = 0; i < nParticles; i++) {
		outForces[i] += m_force*inMasses[i];
	}
}
