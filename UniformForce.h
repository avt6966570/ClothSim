// UniformForce.h: interface for the UniformForce class.
//
// A force proportional to mass that acts uniformly on all particles.
//
//////////////////////////////////////////////////////////////////////

#ifndef _UNIFORMFORCE_H_
#define _UNIFORMFORCE_H_

#include "Force.h"

class UniformForce : public Force  
{
public:
	UniformForce(const VECTOR3 &force);
	virtual ~UniformForce();

	virtual void Add(int nParticles, 
		const CLOTHVERTEX *inPositions, const VECTOR3 *inVelocities, const SCALAR *inMasses,
		VECTOR3 *outForces);

protected:

	VECTOR3 m_force;

};

#endif // #ifndef _UNIFORMFORCE_H_

