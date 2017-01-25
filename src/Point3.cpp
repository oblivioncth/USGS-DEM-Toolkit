#include "Point3.h"

Point3::Point3()
{
	x = 0;
	y = 0;
	z = -32767;//point disabled by default
}

Point3::Point3(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
}

Point3::~Point3()
{
}


