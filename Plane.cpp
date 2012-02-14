#include "Plane.hpp"

Plane::Plane( const Vector3f & normal, const Vector3f & point ) :
	mNormal( normal )
{
	mPlaneConstant = normal.dot( point );
}
