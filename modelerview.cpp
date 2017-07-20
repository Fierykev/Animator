#include <GL/glew.h>
#include "modelerview.h"
#include "camera.h"
#include "bitmap.h"
#include "modelerapp.h"
#include "particleSystem.h"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <cstdio>

static const int	kMouseRotationButton = FL_LEFT_MOUSE;
static const int	kMouseTranslationButton = FL_MIDDLE_MOUSE;
static const int	kMouseZoomButton = FL_RIGHT_MOUSE;

static const char *bmp_name = NULL;

GLuint frame, depth, depthBack, render;
int w_width, w_height;

ModelerView::ModelerView(int x, int y, int w, int h, char *label)
	: Fl_Gl_Window(x, y, w, h, label), t(0), save_bmp(false)
{
	m_ctrl_camera = new Camera();
	m_curve_camera = new Camera();
	camera(CURVE_MODE);
}

ModelerView::~ModelerView()
{
	delete m_ctrl_camera;
	delete m_curve_camera;
}
int ModelerView::handle(int event)
{
	unsigned eventCoordX = Fl::event_x();
	unsigned eventCoordY = Fl::event_y();
	unsigned eventButton = Fl::event_button();
	unsigned eventState = Fl::event_state();

	switch (event)
	{
	case FL_PUSH:
	{
		switch (eventButton)
		{
		case kMouseRotationButton:
			if (!Fl::event_state(FL_ALT)) {
				m_camera->clickMouse(kActionRotate, eventCoordX, eventCoordY);
				break;
			} // ALT + LEFT = MIDDLE
		case kMouseTranslationButton:
			m_camera->clickMouse(kActionTranslate, eventCoordX, eventCoordY);
			break;
		case kMouseZoomButton:
			m_camera->clickMouse(kActionZoom, eventCoordX, eventCoordY);
			break;
		}
		// printf("push %d %d\n", eventCoordX, eventCoordY);
	}
	break;
	case FL_DRAG:
	{
		m_camera->dragMouse(eventCoordX, eventCoordY);
		//printf("drag %d %d\n", eventCoordX, eventCoordY);
	}
	break;
	case FL_RELEASE:
	{
		switch (eventButton)
		{
		case kMouseRotationButton:
		case kMouseTranslationButton:
		case kMouseZoomButton:
			m_camera->releaseMouse(eventCoordX, eventCoordY);
			break;
		}
		//  printf("release %d %d\n", eventCoordX, eventCoordY);
	}
	break;
	default:
		return Fl_Gl_Window::handle(event);
	}

	redraw();

	return 1;
}

static GLfloat lightPosition0[] = { 4, 2, -4, 0 };
static GLfloat lightDiffuse0[] = { 1, 1, 1, 1 };
static GLfloat lightPosition1[] = { -2, 1, 5, 0 };
static GLfloat lightDiffuse1[] = { 1, 1, 1, 1 };


void ModelerView::draw()
{
	if (!valid())
	{
#ifndef RENDER
		w(1920);
		h(1080);
#endif

		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_NORMALIZE);

		glGenFramebuffers(1, &frame);
		glBindFramebuffer(GL_FRAMEBUFFER, frame);

		glGenTextures(1, &render);
		glBindTexture(GL_TEXTURE_2D, render);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w(), h(), 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render, 0);

		glGenTextures(1, &depthBack);
		glBindTexture(GL_TEXTURE_2D, depthBack);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
			w(), h(), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);

		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
			w(), h(), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			depth, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", status);
		}
	}
	
	w_width = w();
	w_height = h();

	// bind frame
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	glViewport(0, 0, w(), h());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, float(w()) / float(h()), 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 1.0f, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->applyViewingTransform();

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);

	// If particle system exists, draw it
	ParticleSystem *ps = ModelerApplication::Instance()->GetParticleSystem();
	if (ps != NULL) {
		ps->computeForcesAndUpdateParticles(t);
		ps->drawParticles(t);
	}
}


/** Set the active camera **/
void ModelerView::camera(cam_mode_t mode)
{
	switch (mode) {
	case CTRL_MODE:
		m_camera = m_ctrl_camera;
		break;
	case CURVE_MODE:
		m_camera = m_curve_camera;
		break;
	}
}


/** Cleanup fxn for saving bitmaps **/
void ModelerView::endDraw()
{
	/*
	GLfloat* windowDepth = new GLfloat[w() * h()];
	unsigned char* bytes = new unsigned char[w() * h() * 3];

	glBindTexture(GL_TEXTURE_2D, depth);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, windowDepth);

	for (int i = 0; i < w() * h(); i++)
	{
		unsigned char val = windowDepth[i] * 100;

		bytes[i * 3] = val;
		bytes[i * 3 + 1] = val;
		bytes[i * 3 + 2] = val;
	}

	writeBMP("Depth.bmp", w(), h(), bytes);

	delete[] windowDepth, bytes;*/
	
#ifdef RENDER

	// display
	glDisable(GL_LIGHTING);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	{
		glClearColor(0, 1.0f, 0, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBindTexture(GL_TEXTURE_2D, render);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glColor3f(1, 1, 1);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-1, -1);
		glTexCoord2f(1, 0);
		glVertex2f(1, -1);
		glTexCoord2f(1, 1);
		glVertex2f(1, 1);
		glTexCoord2f(0, 1);
		glVertex2f(-1, 1);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}
	#endif

	if ((bmp_name == NULL) || (!save_bmp)) return;
	glFinish();
	saveBMP(bmp_name);
	save_bmp = false;
}


void ModelerView::setBMP(const char *fname)
{
	save_bmp = true;
	bmp_name = fname;
}

void ModelerView::saveBMP(const char* szFileName)
{
	int xx = x();
	int yy = y();
	int ww = w();
	int hh = h();

	make_current();

	unsigned char *imageBuffer = new unsigned char[3 * ww * hh];

	glReadBuffer(GL_BACK);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, ww);

	glReadPixels(0, 0, ww, hh,
		GL_RGB, GL_UNSIGNED_BYTE,
		imageBuffer);

	writeBMP(szFileName, ww, hh, imageBuffer);

	delete[] imageBuffer;
}
