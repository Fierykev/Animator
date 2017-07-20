#ifndef __SCENE_4_H__
#define __SCENE_4_H__

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"
#include "batman.h"

enum Scene4Controls
{
	S4_BATMAN_X = 0, S4_BATMAN_Y, S4_BATMAN_Z,
	S4_BATMAN_ROT,
	S4_HEAD_ROT_Y,
	S4_HEAD_ROT_Z,
	S4_HAGRID_X, S4_HAGRID_Y, S4_HAGRID_Z,
	S4_HAGRID_SCALE, S4_HAGRID_ROT,
	S4_HAGRID_HEAD, S4_HAGRID_TORSO,
	S4_HAGRID_L_ARM, S4_HAGRID_R_ARM, S4_HAGRID_L_HAND, S4_HAGRID_R_HAND,
	S4_HAGRID_L_LEG, S4_HAGRID_R_LEG,
	S4_HAGRID_NUM
};

class Scene_4 : public ModelerView
{
public:
	Scene_4(int x, int y, int w, int h, char *label);
	~Scene_4();

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

extern int runScene4();

#endif
