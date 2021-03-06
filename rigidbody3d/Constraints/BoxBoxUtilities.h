// BoxBoxUtilities.h
//
// Adapted from ODE
// Last updated: 01/05/2016

#ifndef BOX_BOX_UTILITIES_H
#define BOX_BOX_UTILITIES_H

#include "scisim/Math/MathDefines.h"

// TODO: Complete cleanup of this code

namespace BoxBoxUtilities
{

void isActive( const Vector3s& cm0, const Matrix33sr& R0, const Vector3s& side0, const Vector3s& cm1, const Matrix33sr& R1, const Vector3s& side1, Vector3s& n, std::vector<Vector3s>& points );

}

#endif
