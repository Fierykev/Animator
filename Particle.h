#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#define CACHE

//#include <GL/glew.h>
//#include <GL/glut.h>

#include <algorithm>
#include <climits>
#include <map>
#include <fstream>
#include "modelerdraw.h"
#include "modelerview.h"
#include "Force.h"
#include "bitmap.h"
#include "robotarm.h"
#include "camera.h"

#define GRAVITY 10.f

using namespace std;

struct ParticleUpload
{
	Vec4f loc;
	Vec4f pos;
	float size;
	Vec2f texCoord;
	float life;
	Vec3f origin;
};

struct C4
{
	char x, y, z, w;
};

class Particle
{
public:

	float life;
	float maxLife;
	Vec3d vel, pos;
	float mass;
	float radius;
	Vec3d source;

	void draw()
	{
		setDiffuseColor(1.f, 1.f, 1.f);
		setAmbientColor(1.f, 1.f, 1.f);

		glPushMatrix();
		{
			glTranslatef(pos[0], pos[1], pos[2]);
			drawSphere(radius);
		}
		glPopMatrix();
	}
};

static bool ParticleSort(Particle a, Particle b)
{
	Vec3f tmp = Camera::mLookAt - Camera::mPosition;
	Vec3d dir = { tmp[0], tmp[1], tmp[2] };
	dir.normalize();
	Vec3d eye = { Camera::mPosition[0], Camera::mPosition[1], Camera::mPosition[2] };

	auto toObjA = a.pos - eye;
	float depthA = dir * toObjA;

	auto toObjB = b.pos - eye;
	float depthB = dir * toObjB;

	return depthA > depthB;
}

inline int ran(int low, int high)
{
	return low + rand() % (high - low + 1);
}

class PSystem
{
public:
	PSystem();

	void clear();

	bool empty();

	void addForce(Force* f);

	void easyBake(float t);

	void update(float delta);

	void draw(float t);

	void setSource(Vec3d s);

	void setEmitRate(float er);

	void setLaunchDir(Vec3d dir);

	void setMinVelocity(Vec3d vel);

	void setMaxJitter(int jit);

	Force* getForce(size_t i);

	void createStencil();

	void copyStencil();

	void loadShaders();

	void loadTextures();

	void noiseVol(unsigned int volSize);
	
private:

	std::vector<Particle> particles;

	std::map<float, std::vector<Particle>> particleCache;

	std::vector<Force*> force;

	Vec3d source = { INT_MAX, INT_MAX, INT_MAX };

	int maxLife = 200, minLife = 100;
	int maxSpawn = 100, minSpawn = 10;
	int maxJitter = 100, minJitter = 0;
	int maxMass = 10, minMass = 10;
	int maxRadius = 50, minRadius = 50;
	Vec3d minVelocity = { .4f, 2.f, .4f };
	Vec3d minVel = { .0f, 2.f, .0f };

	float runningDelta = 0;

	bool started = false;

	GLuint cStencil;
	GLuint smokeShader, vs, fs;
	GLuint densityTex;
	GLuint smoke;
};

#endif
