
#include <GL/glew.h>
#include <FL/gl.h>

#include "robotarm.h"
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "particleSystem.h"

#include "batman.h"
#include "mat.h"
#include "particleSystem.h"
#include <cstdlib>

ParticleSystem* particlesBatman;

Batman::Batman(int x, int y, int w, int h, char *label)
	: ModelerView(x, y, w, h, label)
{
	init = false;
}

Batman::~Batman()
{

}

void Batman::load()
{
	if (!init)
	{
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			printf("ERROR %s\n", glewGetErrorString(err));
		}

		ground.Load("Batman/Ground.obj");
		head.Load("Batman/Head.obj");
		torso.Load("Batman/Torso.obj");
		lHand.Load("Batman/LHand.obj");
		rHand.Load("Batman/RHand.obj");
		lArm.Load("Batman/LArm.obj");
		rArm.Load("Batman/RArm.obj");
		lLeg.Load("Batman/LLeg.obj");
		rLeg.Load("Batman/RLeg.obj");
		crotch.Load("Batman/Crotch.obj");
		init = true;
	}
}

void Batman::draw()
{
	// load batman if needed
	load();

	float Vbatman_x = VAL(BATMAN_X);
	float Vbatman_y = VAL(BATMAN_Y);
	float Vbatman_z = VAL(BATMAN_Z);
	float Vscale = VAL(BATMAN_SCALE);
	float VlHead = VAL(HEAD);
	float Vtorso = VAL(TORSO);
	float VlArm = VAL(L_ARM);
	float VrArm = VAL(R_ARM);
	float VlHand = VAL(L_HAND);
	float VrHand = VAL(R_HAND);
	float VlLeg = VAL(L_LEG);
	float VrLeg = VAL(R_LEG);
	float VbPC = VAL(BAT_PARTICLE_COUNT);

	// This call takes care of a lot of the nasty projection 
	// matrix stuff
	ModelerView::draw();
	/*
	// setup lighting
	ObjLoader::setLightPos(Vec3f(0, 2, -14));
	ObjLoader::setLightAt(Vec3f(0, 0, 0));
	ObjLoader::setupLighting(30.f, 1.f, 1.f, 100.f);
	*/
	// draw twice for stencil
	int i = 0;
	//for (int i = 0; i < 3; i++)
	{
		{
			if (i == 2)
			{
				// update stencil buffer
				ObjLoader::updateStencil();

				// setup camera
				glLoadIdentity();
				m_camera->applyViewingTransform();

				// get the view matrix
				ObjLoader::setViewMatrix();

				// clear out the buffer and all colors
				glClearColor(0, 1.0f, 0, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			// because blender...
			glRotatef(-90, 0, 1, 0);

			// setup ground
			ground.draw(i);

			// scale batman
			glTranslatef(0, -4.68893, 0);
			glScalef(Vscale, Vscale, Vscale);
			glTranslatef(0, 4.68893, 0);

			// move The Batman
			glTranslatef(Vbatman_z, Vbatman_y, Vbatman_x);

			glPushMatrix();
			{
				// draw torso
				glTranslatef(0, -1.6746, 0);
				glRotated(Vtorso, 0, 0, 1);
				glTranslatef(0, 1.6746, 0);

				torso.draw(i);

				// head
				glPushMatrix();
				{
					glRotated(VlHead, 0, 1, 0);
					head.draw(i);
				}
				glPopMatrix();

				// left arm and hand
				glPushMatrix();
				{
					glTranslatef(-0.16249, 0.49033, 0.85506);
					glRotated(-VlArm, 0, 0, 1);
					glTranslatef(0.16249, -0.49033, -0.85506);

					lArm.draw(i);

					glTranslatef(-0.26823, -1.17222, 2.05062);
					glRotatef(-VlHand, 0, 1, 0);
					glTranslatef(0.26823, 1.17222, -2.05062);

					lHand.draw(i);
				}
				glPopMatrix();

				// right arm and hand
				glPushMatrix();
				{
					glTranslatef(-0.16249, 0.49033, -0.85506);
					glRotated(-VrArm, 0, 0, 1);
					glTranslatef(0.16249, -0.49033, 0.85506);

					rArm.draw(i);

					glTranslatef(-0.26823, -1.17222, -2.05062);
					glRotatef(VrHand, 0, 1, 0);
					glTranslatef(0.26823, 1.17222, 2.05062);

					rHand.draw(i);
				}
				glPopMatrix();
			}
			glPopMatrix();

			// left leg
			glPushMatrix();
			{
				glTranslatef(-0.06068, -2.40799, 0.77364);
				glRotated(-VlLeg, 0, 0, 1);
				glTranslatef(0.06068, 2.40799, -0.77364);

				lLeg.draw(i);
			}
			glPopMatrix();

			// right leg
			glPushMatrix();
			{
				glTranslatef(-0.06068, -2.40799, -0.77364);
				glRotated(-VrLeg, 0, 0, 1);
				glTranslatef(0.06068, 2.40799, 0.77364);

				rLeg.draw(i);
			}
			glPopMatrix();

		}
	}

	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();
}

ModelerView* createBatman(int x, int y, int w, int h, char *label)
{
	return new Batman(x, y, w, h, label);
}

//#ifdef BATMAN

int runBatman()
{
	ModelerControl controls[BAT_NUM];

	controls[BATMAN_X] = ModelerControl("move Batman X", -180.0, 180.0, 0.1, 0.0);
	controls[BATMAN_Y] = ModelerControl("move Batman Y", -180.0, 180.0, 0.1, 0.0);
	controls[BATMAN_Z] = ModelerControl("move Batman Z", -180.0, 180.0, 0.1, 0.0);
	controls[BATMAN_SCALE] = ModelerControl("scale Batman", .0, 10.0, 0.1, 1.0);
	controls[HEAD] = ModelerControl("rotate head", -90.0, 90.0, 0.1, .0);
	controls[TORSO] = ModelerControl("rotate torso", -90.0, 90.0, 0.1, .0);
	controls[L_ARM] = ModelerControl("rotate left arm", 0.0, 180.0, 0.1, .0);
	controls[R_ARM] = ModelerControl("rotate right arm", 0.0, 180.0, 0.1, .0);
	controls[L_HAND] = ModelerControl("rotate left hand", -90.0, 90.0, 0.1, .0);
	controls[R_HAND] = ModelerControl("rotate right hand", -90.0, 90.0, 0.1, .0);
	controls[L_LEG] = ModelerControl("rotate left leg", -28.352, 90.0, 0.1, 0);
	controls[R_LEG] = ModelerControl("rotate right leg", -28.352, 90.0, 0.1, 0);
	controls[BAT_PARTICLE_COUNT] = ModelerControl("particle emission count (pc)", 0.0, 5.0, 0.1, 5.0);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.
	particlesBatman = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(particlesBatman);

	ModelerApplication::Instance()->Init(&createBatman, controls, BAT_NUM);
	return ModelerApplication::Instance()->Run();
}

//#endif
