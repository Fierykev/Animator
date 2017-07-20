#ifndef __FORCE__H__
#define __FORCE__H__

#include "vec.h"

class Force
{
public:
	Force()
	{

	}

	virtual Vec3d relForce(Vec3d pos, float mass) = 0;
};

class Gravity : public Force
{
public:

	Vec3d relForce(Vec3d pos, float mass)
	{
		return Vec3d(0.f, -10.f, 0.f) * mass;
	}
};

class Direction : public Force
{
public:

	Vec3d relForce(Vec3d pos, float mass)
	{
		return force * mass;
	}

	void setForce(Vec3d f)
	{
		force = f;
	}

private:

	Vec3d force = { 0, 0, 0 };
};

#endif