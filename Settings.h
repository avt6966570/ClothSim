//
// Settings.h
// 
// Representation of all the parameters that control our simulation.
// Default settings are defined in the default constructor in Settings.cpp.
// 

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Physics.h"

enum { INT_INVALIDFIRST = -1, 
	INT_EULER, 
	INT_VERLET, 
	INT_RK,
	INT_INVALIDLAST };
typedef int IntegratorType;

struct Settings
{
	int iGridSize;

	SCALAR timeStep;

	IntegratorType integrator;

	bool bConstrainTopLeft;
	bool bConstrainTopRight;
	bool bConstrainBotLeft;
	bool bConstrainBotRight;

	SCALAR kMassPerParticle;

	SCALAR kGravityAccel;			// multiplied by (0, -1, 0) in our simulation

	bool bStretch;
	bool bShear;
	bool bBend;

	SCALAR kStretchSpring;
	SCALAR kShearSpring;	
	SCALAR kBendSpring;
	
	SCALAR kStretchDamp;
	SCALAR kShearDamp;
	SCALAR kBendDamp;

	VECTOR3 vInitialPosition;		// In world space coordinates
	VECTOR3 vInitialRotationAxis;	// In world space coordinates
	SCALAR fInitialRotationAngle;
	SCALAR fxScale;					// In world space coordinates
	SCALAR fyScale;					// In world space coordinates

	Settings();
};

#endif // #ifndef _SETTINGS_H_