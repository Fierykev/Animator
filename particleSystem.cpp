#pragma warning(disable : 4786)

#include "particleSystem.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

using namespace std;

static float prevT;

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	pSystem.addForce(new Gravity());
	bake_end_time = -1;
	prevT = -1;
	lastSource = Vec3d(INT_MAX, INT_MAX, INT_MAX);
}




/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
	// TODO

	// These values are used by the UI ...
	// negative bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	bake_start_time = t;
	simulate = true;
	dirty = true;
	pSystem.clear();
	pSystem.addForce(new Gravity());
	lastSource = { INT_MAX, INT_MAX, INT_MAX };
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	// TODO

	// These values are used by the UI
	simulate = false;
	dirty = true;
	
	bakeParticles(t);
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	// TODO
	// These values are used by the UI
	bake_end_time = -1;
	simulate = false;
	dirty = true;
	pSystem.clear();
	pSystem.addForce(new Gravity());
	prevT = -1;
	lastSource = { INT_MAX, INT_MAX, INT_MAX };
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	float delta = t - prevT;

	if (t <= bake_end_time)
		return;

	// TODO
	if (simulate)
	{
		// update particles
		pSystem.update(delta);

		// store them
		bakeParticles(t);

		// Debugging info
		if (t - prevT > .04)
			printf("(!!) Dropped Frame %lf (!!)\n", t - prevT);
		prevT = t;
	}
}

void ParticleSystem::forcedDraw(float t)
{
	if (bake_start_time > t || bake_end_time < t)
		return;

	// TODO
	pSystem.draw(t);
}

void ParticleSystem::setMinVelocity(Vec3d vel)
{
	pSystem.setMinVelocity(vel);
}

void ParticleSystem::setMaxJitter(int jit)
{
	pSystem.setMaxJitter(jit);
}

/** Render particles */
void ParticleSystem::drawParticles(float t)
{

}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	bake_end_time = t;

	if (pSystem.empty())
		bake_start_time = t;

	pSystem.easyBake(t);
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	resetSimulation(0);
}

void ParticleSystem::setSource(Vec3d origin)
{
	// TODO UNCOMMENT
	//if (lastSource[0] != INT_MAX && lastSource[1] != INT_MAX && lastSource[2] != INT_MAX)
		//pSystem.setLaunchDir(-(origin - lastSource) * 150.f);
	pSystem.setLaunchDir(Vec3d(0, 0, 0));
	lastSource = origin;

	pSystem.setSource(origin);
}

void ParticleSystem::setEmitRate(float er)
{
	pSystem.setEmitRate(er);
}


