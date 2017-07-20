#ifndef __SCENE_3_H__
#define __SCENE_3_H__

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"
#include "batman.h"

enum HagridControls
{
	HAGRID_X = BAT_NUM, HAGRID_Y, HAGRID_Z,
	HAGRID_SCALE, HAGRID_HEAD, HAGRID_TORSO,
	HAGRID_L_ARM, HAGRID_R_ARM, HAGRID_L_HAND, HAGRID_R_HAND,
	HAGRID_L_LEG, HAGRID_R_LEG,
	HAGRID_NUM,
};


class Scene_3 : public ModelerView
{
public:
	Scene_3(int x, int y, int w, int h, char *label);
	~Scene_3();

	void load();
	void draw();

private:
	ObjLoader head, torso, lHand, rHand, lArm, rArm, lLeg, rLeg, crotch;

	ObjLoader hagrid_head, hagrid_torso,
		hagrid_lHand, hagrid_rHand,
		hagrid_lArm, hagrid_rArm,
		hagrid_lLeg, hagrid_rLeg;

	ObjLoader walls, stairs, couch, ground;

	void renderHagrid(int i);

	void renderBatman(int i);

	void renderHouse(int i);

	bool init, secondScene;
};

extern int runScene3();

#endif