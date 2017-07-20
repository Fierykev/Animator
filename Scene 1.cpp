#include <GL/glew.h>
#include <FL/gl.h>

#include "batman.h"
#include "robotarm.h"
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "particleSystem.h"

#include "Scene 1.h"
#include "mat.h"
#include "particleSystem.h"
#include <cstdlib>

ParticleSystem* particlesScene1;

Scene_1::Scene_1(int x, int y, int w, int h, char *label)
	: ModelerView(x, y, w, h, label)
{
	init = false;
}

Scene_1::~Scene_1()
{

}

void Scene_1::train()
{
	// enable anti-antialiasing
	glHint(GL_SAMPLES, 10);
	glEnable(GL_MULTISAMPLE);

	// This call takes care of a lot of the nasty projection 
	// matrix stuff
	ModelerView::draw();

	// setup lighting
	
	ObjLoader::setLightPos(Vec3f(50, 100, 50));
	ObjLoader::setLightAt(Vec3f(20, 0, 0));
	ObjLoader::setupLighting(100.f, 1, 1, 1000.0);
	
	// draw twice for stencil
	for (int i = 1; i < 3; i++)
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
			glClearColor(0, .0f, 0, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		float speed = 40;

		glPushMatrix();
		{
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.2, .2, .2, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(true);

			// extend ground
			
			for (int x = 0; x < 5; x++)
			{
				for (int z = -2; z < 3; z++)
				{
					glPushMatrix();
					{
						glTranslatef(-40 * x, -40, 40 * z);
						ground.draw(i);
					}
					glPopMatrix();
				}
			}

			glPushMatrix();
			{
				glTranslatef(1 - (t - 3.27) * speed, -36, -12);
				headCar.draw(i);
			}
			glPopMatrix();
			
			glPushMatrix();
			{
				glTranslatef(80 - (t - 3.27) * speed, -36, -12);
				//mainCabin.draw(i);
			}
			glPopMatrix();
		}
		glPopMatrix();

		if (i == 2)
			ObjLoader::applyViewMatrix();
		
		// render particles to close out the scene
		
		particlesScene1->setEmitRate(200);
		particlesScene1->setMaxJitter(2000);
		particlesScene1->setMinVelocity(Vec3d(30, 20, 0));
		particlesScene1->setSource(Vec3d(20 - (t - 3.27) * speed, 0, 0));
		particlesScene1->forcedDraw(t);
	}
}

void Scene_1::load()
{
	if (!init)
	{
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			printf("ERROR %s\n", glewGetErrorString(err));
		}
		
		ground.Load("Scene 1/Ground.obj");
		sideWall.Load("Scene 1/Side Wall.obj");
		chair.Load("Scene 1/chair.obj");
		tv.Load("Scene 1/TV.obj");
		tvPlane.Load("Scene 1/TV Plane.obj");
		backWall.Load("Scene 1/Back Wall.obj");

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


		// TMP
		//headCar.Load("Scene 2/Head Car.obj");//"Scene 2/Head Car.obj");
		//mainCabin.Load("Scene 2/Main Cabin.obj");
	}
}

void Scene_1::draw()
{
	// load batman if needed
	load();

	// switch scenes
	if (t >= 3.27 || secondScene)
	//if (false)
	{
		if (!secondScene)
		{
			// delete all objects
			//ground.reset();
			sideWall.reset();
			chair.reset();
			tv.reset();
			tvPlane.reset();
			backWall.reset();

			head.reset();
			torso.reset();
			lHand.reset();
			rHand.reset();
			lArm.reset();
			rArm.reset();
			lLeg.reset();
			rLeg.reset();
			crotch.reset();

			// load train
			headCar.Load("Scene 2/Head Car.obj");
			//mainCabin.Load("Scene 2/Main Cabin.obj");
			//coalCar.Load("Scene 2/Coal Car.obj");

			secondScene = true;
		}

		train();

		endDraw();

		return;
	}

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

	// enable anti-antialiasing
	glHint(GL_SAMPLES, 10);
	glEnable(GL_MULTISAMPLE);

	// This call takes care of a lot of the nasty projection 
	// matrix stuff
	ModelerView::draw();

	// setup lighting
	
	ObjLoader::setLightPos(Vec3f(32, 20, 10));
	ObjLoader::setLightAt(Vec3f(0, 0, 10));
	ObjLoader::setupLighting(50.f, 1, .1, 100.0);
	
	// draw twice for stencil
	for (int i = 1; i < 3; i++)
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
			glClearColor(0, .0f, 0, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		
		glPushMatrix();
		{
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.2, .2, .2, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(true);

			// because blender...
			//glRotatef(-90, 0, 1, 0);

			// scene 1
			chair.draw(i);
			tv.draw(i);
			backWall.draw(i);

			// extend side wall
			sideWall.draw(i);

			glPushMatrix();
			{
				glScalef(-1, 1, 1);
				glTranslatef(-1.2, 0, 0);
				sideWall.draw(i);
			}
			glPopMatrix();

			glPushMatrix();
			{
				glTranslatef(-60, 0, 0);
				sideWall.draw(i);
			}
			glPopMatrix();

			// extend ground
			ground.draw(i);

			glPushMatrix();
			{
				glTranslatef(-40, 0, 0);
				ground.draw(i);
			}
			glPopMatrix();

			// draw tv set
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(false);

			// change tv when needed
			char* baseFile = "Scene 1/Movie/Pokemon";
			float startT = 0;
			float delta = .01f;
			int lastFrame = 60;

			for (int i = 0; i <= lastFrame; i++)
			{
				string name = baseFile;
				name += std::to_string(i);
				name += ".bmp";

				if (startT + (float)i * delta <= t
					&& ((float)t < startT + ((float)i + 1) * delta
						|| (float)i == lastFrame))
				{
					tvPlane.setImage(0, name.c_str());
					break;
				}
			}

			tvPlane.draw(i);

			// BATMAN
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.5, .5, .5, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(false);

			// change mouth when needed
			baseFile = "Scene 1/Mouth Animation/BatmanUV";
			startT = 2.14;
			delta = .1;
			lastFrame = 11;

			for (int i = 0; i <= lastFrame; i++)
			{
				string name = baseFile;
				name += std::to_string(i);
				name += ".bmp";

				if (startT + (float)i * delta <= t
					&& ((float)t < startT + ((float)i + 1) * delta
						|| (float)i == lastFrame))
				{
					head.setImage(0, name.c_str());
					break;
				}
			}

			// scale batman
			glTranslatef(0, -4.68893, 0);
			glScalef(Vscale, Vscale, Vscale);
			glTranslatef(0, 4.68893, 0);

			// set batman start
			glRotatef(180, 0, 1, 0);
			glTranslatef(-9.5, 7.5, -10.2);

			// move The Batman
			glTranslatef(Vbatman_z, Vbatman_y, Vbatman_x);

			glPushMatrix();
			{
				crotch.draw(i);

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
		glPopMatrix();
		
		if (i == 2)
			ObjLoader::applyViewMatrix();

		// render particles to close out the scene
		if (t >= 2.0)
		{
			particlesScene1->setEmitRate(200);
			particlesScene1->setMaxJitter(2000);
			particlesScene1->setMinVelocity(Vec3d(-10, 3, -2));
			particlesScene1->setSource(Vec3d(20, 0, 0));
			particlesScene1->forcedDraw(t);
		}
	}

	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();
}

ModelerView* createScene1(int x, int y, int w, int h, char *label)
{
	return new Scene_1(x, y, w, h, label);
}

//#ifdef BATMAN

int runScene1()
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
	controls[L_LEG] = ModelerControl("rotate left leg", -28.352, 90.0, 0.1, 90.0);
	controls[R_LEG] = ModelerControl("rotate right leg", -28.352, 90.0, 0.1, 90.0);
	controls[BAT_PARTICLE_COUNT] = ModelerControl("particle emission count (pc)", 0.0, 5.0, 0.1, 5.0);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.
	particlesScene1 = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(particlesScene1);

	ModelerApplication::Instance()->Init(&createScene1, controls, BAT_NUM);
	return ModelerApplication::Instance()->Run();
}

//#endif
