#ifndef __SCENE_6_H_
#define __SCENE_6_H_

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"
#include "batman.h"

class Scene_6 : public ModelerView
{
public:
	Scene_6(int x, int y, int w, int h, char *label);
	~Scene_6();

	void load();
	void draw();

private:
	ObjLoader head, lowerHalf;

	ObjLoader hagrid_head, hagrid_torso,
		hagrid_lHand, hagrid_rHand,
		hagrid_lArm, hagrid_rArm,
		hagrid_lLeg, hagrid_rLeg;

	ObjLoader room;

	void renderBatman(int i);

	void renderRoom(int i);

	bool init, secondScene;
};

extern int runScene6();

#endif