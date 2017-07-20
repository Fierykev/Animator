#ifndef __BATMAN_H__
#define __BATMAN_H__

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"

enum BatmanControls
{
	BATMAN_X = 0, BATMAN_Y, BATMAN_Z, BATMAN_SCALE, HEAD, TORSO, L_ARM, R_ARM, L_HAND, R_HAND, L_LEG, R_LEG, BAT_PARTICLE_COUNT, BAT_NUM,
};

class Batman : public ModelerView
{
public:
	Batman(int x, int y, int w, int h, char *label);

	~Batman();

	void load();

	void draw();

private:
	ObjLoader ground, head, torso, lHand, rHand, lArm, rArm, lLeg, rLeg, crotch;

	bool init;
};


extern int runBatman();

#endif
