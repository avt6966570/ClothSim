// RKIntegrator.h: interface for the EulerIntegrator class.
//
// Fourth-order Runge-Kutta explicit integrator.
//
//////////////////////////////////////////////////////////////////////


#ifndef _RKINTEGRATOR_H_
#define _RKINTEGRATOR_H_


#include "ForceIntegrator.h"

class RKIntegrator : public ForceIntegrator  
{
public:
	RKIntegrator(const int iParticles,
					const SCALAR *inMasses, const SCALAR *inInvMasses,
					CLOTHVERTEX *outPositions, VECTOR3 *outVelocities);
	virtual ~RKIntegrator();

	virtual void Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep);

protected:
	void ZeroAccumulatorAndSumForces(const CLOTHVERTEX *inPositions,
											   const VECTOR3 *inVelocities,
											   list<Force *> &extForces, 
											   list<Force *> &intForces);
	

	CLOTHVERTEX *m_tmpP;
	VECTOR3 *m_tmpV;

	VECTOR3 *m_tmpDeltaV1;
	VECTOR3 *m_tmpDeltaV2;
	VECTOR3 *m_tmpDeltaV3;
	VECTOR3 *m_tmpDeltaV4;
	VECTOR3 *m_tmpDeltaX1;
	VECTOR3 *m_tmpDeltaX2;
	VECTOR3 *m_tmpDeltaX3;
	VECTOR3 *m_tmpDeltaX4;


	VECTOR3 *m_Accumulator;	// Stores the vector sum of all forces acting on each particle
};

#endif // #ifndef _RKINTEGRATOR_H_
