// Force.h: interface for the Force class.
//
// Abstract class representing a "force" which acts on one or more (or all) particles
// in a system, and may or may not depend on position, velocity, and mass.
// 
//////////////////////////////////////////////////////////////////////

#ifndef _FORCE_H_
#define _FORCE_H_

#include "Physics.h"

class Force  
{
public:
	Force() {}
	virtual ~Force() {}

	virtual void Add(int nParticles, 
		const CLOTHVERTEX *inPositions, const VECTOR3 *inVelocities, const SCALAR *inMasses,
		VECTOR3 *outForces) = 0;

	// TODO: add another method to output derivatives for implicit integration.

protected:

};

#endif // #ifdef _FORCE_H_