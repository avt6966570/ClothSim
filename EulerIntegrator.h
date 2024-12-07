// EulerIntegrator.h: interface for the EulerIntegrator class.
//
// Explicit forward Euler integrator.
//
//////////////////////////////////////////////////////////////////////


#ifndef _EULERINTEGRATOR_H_
#define _EULERINTEGRATOR_H_


#include "ForceIntegrator.h"

class EulerIntegrator : public ForceIntegrator  
{
public:
	EulerIntegrator(const int iParticles,
					const SCALAR *inMasses, const SCALAR *inInvMasses,
					CLOTHVERTEX *outPositions, VECTOR3 *outVelocities);
	virtual ~EulerIntegrator();

	virtual void Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep);

protected:
	VECTOR3 *m_Accumulator;	// Stores the vector sum of all forces acting on each particle
};

#endif 
