#ifndef __SCENE_1_H__
#define __SCENE_1_H__

#include "robotarm.h"
#include "modelerview.h"
#include "ObjectFileLoader.h"

class Scene_1 : public ModelerView
{
public:
	Scene_1(int x, int y, int w, int h, char *label);
	~Scene_1();

	void train();
	void load();
	void draw();

private:
	ObjLoader head, torso, lHand, rHand, lArm, rArm, lLeg, rLeg, crotch;

	ObjLoader ground, sideWall, chair, tv, tvPlane, backWall;

	// train
	ObjLoader headCar, mainCabin, coalCar;

	bool init, secondScene;
};

extern int runScene1();

#endif