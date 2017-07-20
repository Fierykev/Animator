#ifndef __SCENE_5_H__
#define __SCENE_5_H__

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"
#include "batman.h"

enum Scene5Controls
{
	S5_BATMAN_X = 0, S5_BATMAN_Y, S5_BATMAN_Z,
	S5_BATMAN_ROT,
	S5_HEAD_ROT_Y,
	S5_HEAD_ROT_Z,
	S5_HAGRID_X, S5_HAGRID_Y, S5_HAGRID_Z,
	S5_HAGRID_SCALE, S5_HAGRID_ROT,
	S5_HAGRID_HEAD, S5_HAGRID_TORSO,
	S5_HAGRID_L_ARM, S5_HAGRID_R_ARM, S5_HAGRID_L_HAND, S5_HAGRID_R_HAND,
	S5_HAGRID_L_LEG, S5_HAGRID_R_LEG,
	S5_HAGRID_NUM
};

class Scene_5 : public ModelerView
{
public:
	Scene_5(int x, int y, int w, int h, char *label);
	~Scene_5();

	void load();
	void draw();

private:
	ObjLoader head, lowerHalf;

	ObjLoader hagrid_head, hagrid_torso,
		hagrid_lHand, hagrid_rHand,
		hagrid_lArm, hagrid_rArm,
		hagrid_lLeg, hagrid_rLeg;

	ObjLoader room;

	void renderHagrid(int i);

	void renderBatman(int i);

	void renderRoom(int i);

	bool init, secondScene;
};

extern int runScene5();

#endif
