#include "Vector3D.h"
#include <stdio.h>

Vector3D::Vector3D()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Vector3D::Vector3D(float inX, float inY, float inZ)
{
	x = inX;
	y = inY;
	z = inZ;
}