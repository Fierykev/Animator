#include <GL/glew.h>
#include <FL/gl.h>

#include "Scene 3.h"
#include "modelerview.h"
#include "batman.h"
#include "particleSystem.h"

ParticleSystem* particlesScene3;

Scene_3::Scene_3(int x, int y, int w, int h, char *label)
	: ModelerView(x, y, w, h, label)
{
	init = false;
}

Scene_3::~Scene_3()
{

}

void Scene_3::load()
{
	if (!init)
	{
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			printf("ERROR %s\n", glewGetErrorString(err));
		}

		head.Load("Batman/Head.obj");
		torso.Load("Batman/Torso.obj");
		lHand.Load("Batman/LHand.obj");
		rHand.Load("Batman/RHand.obj");
		lArm.Load("Batman/LArm.obj");
		rArm.Load("Batman/RArm.obj");
		lLeg.Load("Batman/LLeg.obj");
		rLeg.Load("Batman/RLeg.obj");
		crotch.Load("Batman/Crotch.obj");

		hagrid_head.Load("Hagrid/Head.obj");
		hagrid_torso.Load("Hagrid/Torso.obj");
		hagrid_lHand.Load("Hagrid/LHand.obj");
		hagrid_rHand.Load("Hagrid/RHand.obj");
		hagrid_lArm.Load("Hagrid/LArm.obj");
		hagrid_rArm.Load("Hagrid/RArm.obj");
		hagrid_lLeg.Load("Hagrid/LLeg.obj");
		hagrid_rLeg.Load("Hagrid/RLeg.obj");

		walls.Load("Scene 3/Walls.obj");
		stairs.Load("Scene 3/Stairs.obj");
		couch.Load("Scene 3/Couch.obj");
		ground.Load("Scene 1/Ground.obj");

		init = true;
	}
}

void Scene_3::renderHagrid(int i)
{
	float V_x = VAL(HAGRID_X);
	float V_y = VAL(HAGRID_Y);
	float V_z = VAL(HAGRID_Z);
	float Vscale = VAL(HAGRID_SCALE);
	float VlHead = VAL(HAGRID_HEAD);
	float Vtorso = VAL(HAGRID_TORSO);
	float VlArm = VAL(HAGRID_L_ARM);
	float VrArm = VAL(HAGRID_R_ARM);
	float VlHand = VAL(HAGRID_L_HAND);
	float VrHand = VAL(HAGRID_R_HAND);
	float VlLeg = VAL(HAGRID_L_LEG);
	float VrLeg = VAL(HAGRID_R_LEG);

	ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
	ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
	ObjLoader::setShadow(true);

	glPushMatrix();
	{
		// scale hagrid
		glTranslatef(0, -4.64412, 0);
		glScalef(Vscale, Vscale, Vscale);
		glTranslatef(0, 4.64412, 0);

		// set hagrid start
		glRotatef(180, 0, 1, 0);

		// move The hagrid
		glTranslatef(V_z, V_y, V_x);

		glPushMatrix();
		{
			// draw torso
			glTranslatef(0, -0.21766, 0);
			glRotated(Vtorso, 0, 0, 1);
			glTranslatef(0, 0.21766, 0);

			hagrid_torso.draw(i);
			
			glPushMatrix();
			{
				if (i == 2)
				{
					ObjLoader::applyViewMatrix();

					// scale hagrid
					glTranslatef(0, -4.64412, 0);
					glScalef(Vscale, Vscale, Vscale);
					glTranslatef(0, 4.64412, 0);

					// set hagrid start
					glRotatef(180, 0, 1, 0);

					// move The hagrid
					glTranslatef(V_z, V_y, V_x);

					// rotate torso
					glTranslatef(0, -0.21766, 0);
					glRotated(Vtorso, 0, 0, 1);
					glTranslatef(0, 0.21766, 0);
				}

				// load mouth
				char* baseFile = "Scene 3/Mouth Animation/Mouth";
				float startT = 0, endT = 2.63f;
				float delta = .1f;
				int lastFrame = 12;
				bool loop = true;

				for (int i = 0; startT <= t; i++)
				{
					int frame = i % (lastFrame + 1);

					if (endT <= t)
					{
						frame = int((endT - startT) / delta) % lastFrame;

						string name = baseFile;
						name += std::to_string(frame);
						name += ".bmp";

						hagrid_head.setImage(2, name.c_str());
						break;
					}

					string name = baseFile;
					name += std::to_string(frame);
					name += ".bmp";

					if (startT + (float)i * delta <= t
						&& ((float)t < startT + ((float)i + 1) * delta))
					{
						hagrid_head.setImage(2, name.c_str());
						break;
					}
				}

				glRotated(VlHead, 0, 1, 0);
				hagrid_head.draw(0);
			}
			glPopMatrix();

			// left arm and hand
			glPushMatrix();
			{
				glTranslatef(-0.89029, 5.82858, 2.36644);
				glRotated(-VlArm, 0, 0, 1);
				glTranslatef(0.89029, -5.82858, -2.36644);

				hagrid_lArm.draw(i);

				glTranslatef(-1.03309, 2.97531, 4.00535);
				glRotatef(-VlHand, 0, 1, 0);
				glTranslatef(1.03309, -2.97531, -4.00535);

				hagrid_lHand.draw(i);
			}
			glPopMatrix();

			ObjLoader::setGlobalDiffuseAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(true);

			// right arm and hand
			glPushMatrix();
			{
				glTranslatef(-0.89029, 5.82858, 2.36644);
				glRotated(-VrArm, 0, 0, 1);
				glTranslatef(0.89029, -5.82858, -2.36644);

				hagrid_rArm.draw(i);

				glTranslatef(-1.03309, 2.97531, -4.00535);
				glRotatef(VrHand, 0, 1, 0);
				glTranslatef(1.03309, -2.97531, 4.00535);

				hagrid_rHand.draw(i);
			}
			glPopMatrix();

			ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
			ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
			ObjLoader::setShadow(true);
		}
		glPopMatrix();

		ObjLoader::setGlobalDiffuseAmbient(Vec4f(0, 0, 0, 1));
		ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
		ObjLoader::setShadow(true);

		// left leg
		glPushMatrix();
		{
			glTranslatef(-0.32658, 0.11835, -0.7619);
			glRotated(-VlLeg, 0, 0, 1);
			glTranslatef(0.32658, -0.11835, 0.7619);
			
			hagrid_lLeg.draw(i);
		}
		glPopMatrix();

		ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
		ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
		ObjLoader::setShadow(true);

		// right leg
		glPushMatrix();
		{
			glTranslatef(-0.32658, 0.11835, 0.7619);
			glRotated(-VrLeg, 0, 0, 1);
			glTranslatef(0.32658, -0.11835, -0.7619);
			
			hagrid_rLeg.draw(i);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void Scene_3::renderBatman(int i)
{
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

	glPushMatrix();
	{
		// scale batman
		glTranslatef(0, -4.68893, 0);
		glScalef(Vscale, Vscale, Vscale);
		glTranslatef(0, 4.68893, 0);

		// set batman start
		//glRotatef(180, 0, 1, 0);
		//glTranslatef(-9.5, 7.5, -10.2);

		// move The Batman
		glTranslatef(Vbatman_z, Vbatman_y, Vbatman_x);

		glPushMatrix();
		{
			crotch.draw();

			// draw torso
			glTranslatef(0, -1.6746, 0);
			glRotated(Vtorso, 0, 0, 1);
			glTranslatef(0, 1.6746, 0);

			torso.draw(i);

			// load mouth
			char* baseFile = "Scene 1/Mouth Animation/BatmanUV";
			float startT = 4.8, endT = 7.4f;
			float delta = .1f;
			int lastFrame = 11;
			bool loop = true;

			for (int i = 0; startT <= t; i++)
			{
				int frame = i % (lastFrame + 1);

				if (endT <= t)
				{
					frame = int((endT - startT) / delta) % lastFrame;

					string name = baseFile;
					name += std::to_string(frame);
					name += ".bmp";

					head.setImage(0, name.c_str());
					break;
				}

				string name = baseFile;
				name += std::to_string(frame);
				name += ".bmp";

				if (startT + (float)i * delta <= t
					&& ((float)t < startT + ((float)i + 1) * delta))
				{
					head.setImage(0, name.c_str());
					break;
				}
			}

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
}

void Scene_3::renderHouse(int i)
{
	// extend ground

	for (int x = -2; x < 1; x++)
	{
		for (int z = -1; z < 1; z++)
		{
			glPushMatrix();
			{
				glTranslatef(-42 * x, 5, 42 * z);
				ground.draw(i);
			}
			glPopMatrix();
		}
	}

	walls.draw(i);
	stairs.draw(i);
	couch.draw(i);
}

void Scene_3::draw()
{
	load();

	// enable anti-antialiasing
	glHint(GL_SAMPLES, 10);
	glEnable(GL_MULTISAMPLE);

	// This call takes care of a lot of the nasty projection 
	// matrix stuff
	ModelerView::draw();
	glClearColor(0, 1.0f, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup lighting
	
	ObjLoader::setLightPos(Vec3f(60, 40, -40));
	ObjLoader::setLightAt(Vec3f(60, 0, 0));
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

		ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));

		float lStart = 7.5, lEnd = 7.6;
		if (lStart <= t && t < lEnd)
		{
			float val = .3f;// 1.0 - (t - lStart) / (lStart - lEnd);
			ObjLoader::setGlobalAmbient(Vec4f(val, val, val, 1));

			// render batman
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.8, .8, .8, 1));
			renderBatman(i);

			// render house
			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.2, .2, .2, 1));
			renderHouse(i);
		}
		else
		{
			ObjLoader::setShadow(true);

			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.8, .8, .8, 1));

			// render batman
			renderBatman(i);

			ObjLoader::setGlobalDiffuseAmbient(Vec4f(.2, .2, .2, 1));

			// render house
			renderHouse(i);

		}

		// render hagrid
		renderHagrid(i);


		if (i == 2)
			ObjLoader::applyViewMatrix();
		/*
		// render particles to close out the scene

		particlesScene3->setEmitRate(200);
		particlesScene3->setMaxJitter(2000);
		particlesScene3->setMinVelocity(Vec3d(30, 20, 0));
		particlesScene3->setSource(Vec3d(20, 0, 0));
		particlesScene3->forcedDraw(t);*/
	}

	endDraw();
}

ModelerView* createScene3(int x, int y, int w, int h, char *label)
{
	return new Scene_3(x, y, w, h, label);
}

int runScene3()
{
	ModelerControl controls[HAGRID_NUM];

	controls[BATMAN_X] = ModelerControl("move Batman X", -180.0, 180.0, 0.1, 11.21);
	controls[BATMAN_Y] = ModelerControl("move Batman Y", -180.0, 180.0, 0.1, 10.01764);
	controls[BATMAN_Z] = ModelerControl("move Batman Z", -180.0, 180.0, 0.1, 75.0);
	controls[BATMAN_SCALE] = ModelerControl("scale Batman", .0, 10.0, 0.1, 1.0);
	controls[HEAD] = ModelerControl("rotate head", -90.0, 90.0, 0.1, .0);
	controls[TORSO] = ModelerControl("rotate torso", -90.0, 90.0, 0.1, .0);
	controls[L_ARM] = ModelerControl("rotate left arm", 0.0, 180.0, 0.1, .0);
	controls[R_ARM] = ModelerControl("rotate right arm", 0.0, 180.0, 0.1, .0);
	controls[L_HAND] = ModelerControl("rotate left hand", -90.0, 90.0, 0.1, .0);
	controls[R_HAND] = ModelerControl("rotate right hand", -90.0, 90.0, 0.1, .0);
	controls[L_LEG] = ModelerControl("rotate left leg", -28.352, 90.0, 0.1, 0.0);
	controls[R_LEG] = ModelerControl("rotate right leg", -28.352, 90.0, 0.1, 0.0);
	controls[BAT_PARTICLE_COUNT] = ModelerControl("particle emission count (pc)", 0.0, 5.0, 0.1, 5.0);

	controls[HAGRID_X] = ModelerControl("move Hagrid X", -500.0, 500.0, 0.1, -11.21);
	controls[HAGRID_Y] = ModelerControl("move Hagrid Y", -500.0, 500.0, 0.1, 7.8);
	controls[HAGRID_Z] = ModelerControl("move Hagrid Z", -500.0, 500.0, 0.1, -60.98163);
	controls[HAGRID_SCALE] = ModelerControl("scale Hagrid", .0, 10.0, 0.1, 1.0);
	controls[HAGRID_HEAD] = ModelerControl("rotate Hagrid head", -90.0, 90.0, 0.1, .0);
	controls[HAGRID_TORSO] = ModelerControl("rotate Hagrid torso", -90.0, 90.0, 0.1, .0);
	controls[HAGRID_L_ARM] = ModelerControl("rotate Hagrid left arm", 0.0, 180.0, 0.1, .0);
	controls[HAGRID_R_ARM] = ModelerControl("rotate Hagrid right arm", 0.0, 180.0, 0.1, .0);
	controls[HAGRID_L_HAND] = ModelerControl("rotate Hagrid left hand", -90.0, 90.0, 0.1, .0);
	controls[HAGRID_R_HAND] = ModelerControl("rotate Hagrid right hand", -90.0, 90.0, 0.1, .0);
	controls[HAGRID_L_LEG] = ModelerControl("rotate Hagrid left leg", -28.352, 90.0, 0.1, 0.0);
	controls[HAGRID_R_LEG] = ModelerControl("rotate Hagrid right leg", -28.352, 90.0, 0.1, 0.0);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.
	particlesScene3 = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(particlesScene3);

	ModelerApplication::Instance()->Init(&createScene3, controls, HAGRID_NUM);
	return ModelerApplication::Instance()->Run();
}