#include "Settings.h"

// Default settings to use on startup.

Settings::Settings() {
	iGridSize = 16;

	timeStep = 0.002f;

	integrator = INT_RK;

	bConstrainTopLeft = true;
	bConstrainTopRight = true;
	bConstrainBotLeft = false;
	bConstrainBotRight = false;

	kMassPerParticle = 0.05f;

	kGravityAccel = 9.8f;

	bStretch = true;
	bShear = true;
	bBend = true;

	kStretchSpring = 10000.0f;
	kShearSpring = 1000.0f;
	kBendSpring = 100.0f;

	kStretchDamp = 0.01f;
	kShearDamp = 0.01f;
	kBendDamp = 0.01f;

	vInitialPosition = VECTOR3( 0.0f, 0.0f, 0.0f );
	vInitialRotationAxis = VECTOR3( 1.0f, 0.0f, 0.0f );
	fInitialRotationAngle = (3.141592654f/4.0f);
	fxScale = 2.0f;
	fyScale = 2.0f;
}