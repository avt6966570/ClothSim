// ForceIntegrator.h: interface for the ForceIntegrator class.
//
// Abstract class representing a integrator which totals up forces acting on each
// particle and calculates new positions and velocities.
//
// Note: if you add a new derived class, be sure to update enum IntegratorType
// in Settings.h, ClothSystem::Initialize(), and the configuration UI code in SettingsDialog as well.
//////////////////////////////////////////////////////////////////////

#ifndef _FORCEINTEGRATOR_H_
#define _FORCEINTEGRATOR_H_


#ifdef _MSC_VER
#pragma warning (push, 3)
#pragma warning (disable: 4245)
#pragma warning (disable: 4100) 
#endif

#include <list>
using namespace std;

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include "Force.h"

class ForceIntegrator  
{
public:
	ForceIntegrator(const int iParticles,
					const SCALAR *inMasses, const SCALAR *inInvMasses,
					CLOTHVERTEX *outPositions, VECTOR3 *outVelocities);
	virtual ~ForceIntegrator();
	virtual void Initialize();
	virtual void Update(list<Force *> &extForces, list<Force *> &intForces, SCALAR timeStep) = 0;

protected:
	CLOTHVERTEX *m_outPositions;
	VECTOR3 *m_outVelocities;

	const SCALAR *m_inMasses;
	const SCALAR *m_inInvMasses;

	int m_iParticles;

};

#endif // #ifndef _FORCEINTEGRATOR_H_