//
// A simple two-sided directional lighting vertex shader
// Adapted from Greg James, "Lighting a Single Surface Object", Direct3D ShaderX p. 209
//

#include "ShaderConstants.h"

#define V_POSITION v0
#define V_NORMAL v1
#define V_DIFFUSE v2

#define RV_VERT_TO_EYE	r0
#define RV_COR_NORMAL r1
#define R_ALIGNED r2

vs.1.1

dcl_position v0
dcl_normal v1
dcl_color v2

// Transform position data from world to clip space
m4x4 oPos, V_POSITION, c[CM_VIEWPROJMATRIX] 

// Vertex -> eye
sub RV_VERT_TO_EYE, c[CV_EYE_WORLD], V_POSITION

// Is our normal facing away from us?  If so, flip it.
dp3 R_ALIGNED, V_NORMAL, RV_VERT_TO_EYE
sge R_ALIGNED.x, R_ALIGNED, c[CV_ZERO]
mad RV_COR_NORMAL, V_NORMAL, R_ALIGNED.x, -V_NORMAL
mad RV_COR_NORMAL, V_NORMAL, R_ALIGNED.x, RV_COR_NORMAL

// Simple directional diffuse lighting calcuation
dp3 r4, -c[CV_LIGHT_DIRECTION], RV_COR_NORMAL
mul oD0, V_DIFFUSE, r4
