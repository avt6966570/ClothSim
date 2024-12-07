// VerletIntegrator.h: interface for the EulerIntegrator class.
//
// Explicit Verlet integrator.
//
//////////////////////////////////////////////////////////////////////


#ifndef _VERLETINTEGRATOR_H_
#define _VERLETINTEGRATOR_H_


#include "ForceIntegrator.h"

class VerletIntegrator : public ForceIntegrator  
{
public:
	VerletIntegrator(const int iParticles,
					const SCALAR *inMasses, const SCALAR *inInvMasses,
					CLOTHVERTEX *outPositions, VECTOR3 *outVelocities);
	virtual ~VerletIntegrator();
	virtual void Initialize();
	virtual void Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep);

protected:
	VECTOR3 *m_Accumulator;	// Stores the vector sum of all forces acting on each particle
	VECTOR3 *m_PosOld;	// Old positions
};

#endif 