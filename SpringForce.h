// SpringForce.h: interface for the SpringForce class.
//
// A force that acts mutually on two particles, proportional to displacement
// with damping proportional to relative velocity between the two.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SPRINGFORCE_H_
#define _SPRINGFORCE_H_

#include "Force.h"

class SpringForce : public Force  
{
public:
	SpringForce(const SCALAR restLength, const SCALAR springConstant, const SCALAR dampConstant, const int index1, const int index2);
	virtual ~SpringForce();

	virtual void Add(int nParticles, 
		const CLOTHVERTEX *inPositions, const VECTOR3 *inVelocities, const SCALAR *inMasses,
		VECTOR3 *outForces);

protected:
	SCALAR m_restLength;
	SCALAR m_springConstant;
	SCALAR m_dampConstant;
	int m_index1;
	int m_index2;
};

#endif // #ifndef _SPRINGFORCE_H_
