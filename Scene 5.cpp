#include <GL/glew.h>
#include <FL/gl.h>

#include "Scene 5.h"
#include "modelerview.h"
#include "particleSystem.h"

ParticleSystem* particlesScene5;

Scene_5::Scene_5(int x, int y, int w, int h, char *label)
	: ModelerView(x, y, w, h, label)
{
	init = false;
}

Scene_5::~Scene_5()
{

}

void Scene_5::load()
{
	if (!init)
	{
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			printf("ERROR %s\n", glewGetErrorString(err));
		}

		head.Load("Batman Beauty/Head.obj");
		lowerHalf.Load("Batman Beauty ALT/Lower Half.obj");

		hagrid_head.Load("Hagrid/Head.obj");
		hagrid_torso.Load("Hagrid/Torso.obj");
		hagrid_lHand.Load("Hagrid ALT/LHand.obj");
		hagrid_rHand.Load("Hagrid ALT/RHand.obj");
		hagrid_lArm.Load("Hagrid ALT/LArm.obj");
		hagrid_rArm.Load("Hagrid ALT/RArm.obj");
		hagrid_lLeg.Load("Hagrid/LLeg.obj");
		hagrid_rLeg.Load("Hagrid/RLeg.obj");

		room.Load("Final Scene/Final Scene.obj");

		init = true;
	}
}

void Scene_5::renderHagrid(int i)
{
	float V_x = VAL(S5_HAGRID_X);
	float V_y = VAL(S5_HAGRID_Y);
	float V_z = VAL(S5_HAGRID_Z);
	float Vscale = VAL(S5_HAGRID_SCALE);
	float Vrot = VAL(S5_HAGRID_ROT);
	float VlHead = VAL(S5_HAGRID_HEAD);
	float Vtorso = VAL(S5_HAGRID_TORSO);
	float VlArm = VAL(S5_HAGRID_L_ARM);
	float VrArm = VAL(S5_HAGRID_R_ARM);
	float VlHand = VAL(S5_HAGRID_L_HAND);
	float VrHand = VAL(S5_HAGRID_R_HAND);
	float VlLeg = VAL(S5_HAGRID_L_LEG);
	float VrLeg = VAL(S5_HAGRID_R_LEG);

	ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
	ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
	ObjLoader::setShadow(false);

	glPushMatrix();
	{
		// scale hagrid
		glTranslatef(0, -4.64412, 0);
		glScalef(Vscale, Vscale, Vscale);
		glTranslatef(0, 4.64412, 0);

		// move The hagrid
		glTranslatef(V_z, V_y, V_x);

		// set hagrid start
		glRotatef(Vrot, 0, 1, 0);

		glPushMatrix();
		{
			// draw torso
			glTranslatef(0, -0.21766, 0);
			glRotated(Vtorso, 0, 0, 1);
			glTranslatef(0, 0.21766, 0);

			hagrid_torso.draw(i);

			glPushMatrix();
			{
				glRotated(VlHead, 0, 1, 0);
				hagrid_head.draw(i);
			}
			glPopMatrix();

			// left arm and hand
			glPushMatrix();
			{
				glTranslatef(-0.89029, 5.82858, 2.36644);
				//glRotated(-VlArm, 0, 0, 1);
				glTranslatef(0.89029, -5.82858, -2.36644);

				hagrid_lArm.draw(i);

				glTranslatef(-1.03309, 2.97531, 4.00535);
				//glRotatef(-VlHand, 0, 1, 0);
				glTranslatef(1.03309, -2.97531, -4.00535);

				hagrid_lHand.draw(i);
			}
			glPopMatrix();

			// right arm and hand
			glPushMatrix();
			{
				glTranslatef(-0.89029, 5.82858, 2.36644);
				//glRotated(-VrArm, 0, 0, 1);
				glTranslatef(0.89029, -5.82858, -2.36644);

				hagrid_rArm.draw(i);

				glTranslatef(-1.03309, 2.97531, -4.00535);
				//glRotatef(VrHand, 0, 1, 0);
				glTranslatef(1.03309, -2.97531, 4.00535);

				hagrid_rHand.draw(i);
			}
			glPopMatrix();
		}
		glPopMatrix();

		// left leg
		glPushMatrix();
		{
			glTranslatef(-0.32658, 0.11835, -0.7619);
			glRotated(-VlLeg, 0, 0, 1);
			glTranslatef(0.32658, -0.11835, 0.7619);

			hagrid_lLeg.draw(i);
		}
		glPopMatrix();

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

void Scene_5::renderBatman(int i)
{
	float Vbatman_x = VAL(S5_BATMAN_X);
	float Vbatman_y = VAL(S5_BATMAN_Y);
	float Vbatman_z = VAL(S5_BATMAN_Z);
	float VlbatmanRot = VAL(S5_BATMAN_ROT);
	float VlHeadY = VAL(S5_HEAD_ROT_Y);
	float VlHeadZ = VAL(S5_HEAD_ROT_Z);

	ObjLoader::setGlobalDiffuseAmbient(Vec4f(1, 1, 1, 1));
	ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
	ObjLoader::setShadow(false);

	glPushMatrix();
	{
		// set batman start
		glRotatef(180, 0, 1, 0);

		// move The Batman
		glTranslatef(Vbatman_z, Vbatman_y, Vbatman_x);

		glRotatef(VlbatmanRot, 0, 1, 0);

		// load mouth
		char* baseFile = "Scene 1/Mouth Animation/BatmanUV";
		float startT = 6.76, endT = 7.4f;
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

		glPushMatrix();
		{
			glTranslatef(0, -1.31984, 0);
			glRotatef(VlHeadZ, 0, 0, 1);
			glTranslatef(0, 1.31984, 0);

			glRotatef(VlHeadY, 0, 1, 0);

			head.draw(i);
		}
		glPopMatrix();

		lowerHalf.draw(i);
	}
	glPopMatrix();
}

void Scene_5::renderRoom(int i)
{
	ObjLoader::setGlobalDiffuseAmbient(Vec4f(.1, .1, .1, 1));
	ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));
	ObjLoader::setShadow(true);

	glPushMatrix();
	{

		//glScalef(2.f, 2.f, 2.f);

		room.draw(i);
	}
	glPopMatrix();
}

void Scene_5::draw()
{
	load();

	// enable anti-antialiasing
	glHint(GL_SAMPLES, 10);
	glEnable(GL_MULTISAMPLE);

	// This call takes care of a lot of the nasty projection 
	// matrix stuff
	ModelerView::draw();
	glClearColor(.05f, .05f, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup lighting
	
	ObjLoader::setLightPos(Vec3f(-15, 10, 0));
	ObjLoader::setLightAt(Vec3f(0, 0, -30));
	ObjLoader::setupLighting(200.f, 1, 1, 300.0);
	
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
			glClearColor(1, .0f, 0, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		ObjLoader::setGlobalAmbient(Vec4f(0, 0, 0, 1));

		// render room
		renderRoom(i);

		if (i == 2)
			ObjLoader::applyViewMatrix();

		// render batman
		if (i != 2)
		renderBatman(0);

		// render hagrid
		if (i != 2)
		renderHagrid(0);

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

ModelerView* createScene5(int x, int y, int w, int h, char *label)
{
	return new Scene_5(x, y, w, h, label);
}

int runScene5()
{
	ModelerControl controls[S5_HAGRID_NUM];

	controls[S5_BATMAN_X] = ModelerControl("move Batman X", -180.0, 180.0, 0.1, 0);
	controls[S5_BATMAN_Y] = ModelerControl("move Batman Y", -180.0, 180.0, 0.1, 0);
	controls[S5_BATMAN_Z] = ModelerControl("move Batman Z", -180.0, 180.0, 0.1, 0);
	controls[S5_BATMAN_ROT] = ModelerControl("rotate batman", -1000.0, 1000.0, 0.1, .0);
	controls[S5_HEAD_ROT_Y] = ModelerControl("rotate head Y", -90.0, 90.0, 0.1, .0);
	controls[S5_HEAD_ROT_Z] = ModelerControl("rotate head Z", -90.0, 90.0, 0.1, .0);

	controls[S5_HAGRID_X] = ModelerControl("move Hagrid X", -500.0, 500.0, 0.1, 0);
	controls[S5_HAGRID_Y] = ModelerControl("move Hagrid Y", -500.0, 500.0, 0.1, 0);
	controls[S5_HAGRID_Z] = ModelerControl("move Hagrid Z", -500.0, 500.0, 0.1, 0);
	controls[S5_HAGRID_SCALE] = ModelerControl("scale Hagrid", .0, 10.0, 0.1, 1.0);
	controls[S5_HAGRID_ROT] = ModelerControl("rot Hagrid", -1000.0, 1000.0, 0.1, 180.0);
	controls[S5_HAGRID_HEAD] = ModelerControl("rotate Hagrid head", -90.0, 90.0, 0.1, .0);
	controls[S5_HAGRID_TORSO] = ModelerControl("rotate Hagrid torso", -90.0, 90.0, 0.1, .0);
	controls[S5_HAGRID_L_ARM] = ModelerControl("rotate Hagrid left arm", 0.0, 180.0, 0.1, .0);
	controls[S5_HAGRID_R_ARM] = ModelerControl("rotate Hagrid right arm", 0.0, 180.0, 0.1, .0);
	controls[S5_HAGRID_L_HAND] = ModelerControl("rotate Hagrid left hand", -90.0, 90.0, 0.1, .0);
	controls[S5_HAGRID_R_HAND] = ModelerControl("rotate Hagrid right hand", -90.0, 90.0, 0.1, .0);
	controls[S5_HAGRID_L_LEG] = ModelerControl("rotate Hagrid left leg", -90.0, 90.0, 0.1, 0.0);
	controls[S5_HAGRID_R_LEG] = ModelerControl("rotate Hagrid right leg", -90.0, 90.0, 0.1, 0.0);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.
	particlesScene5 = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(particlesScene5);

	ModelerApplication::Instance()->Init(&createScene5, controls, S5_HAGRID_NUM);
	return ModelerApplication::Instance()->Run();
}