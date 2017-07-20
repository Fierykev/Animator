#include <GL/glew.h>
#include "Particle.h"
#include "bitmap.h"
#include "Shader_Helper.h"

PSystem::PSystem()
{

}

void PSystem::clear()
{
	particles.clear();
	particleCache.clear();

	for (auto f : force)
		delete f;

	force.clear();
}

bool PSystem::empty()
{
	return particleCache.empty();
}

void PSystem::addForce(Force* f)
{
	force.push_back(f);
}

void PSystem::easyBake(float t)
{
#ifdef CACHE
	particleCache[t] = particles;
#endif
}

void PSystem::update(float delta)
{
	// spawn particles
	int spawn = ran(minSpawn, maxSpawn);

#ifndef CACHE
	runningDelta += delta;

	if (runningDelta >= 1.f / 30.f)
	{
		runningDelta = 0;
#endif

		for (int i = 0; i < spawn && source[0] != INT_MAX && source[1] != INT_MAX && source[2] != INT_MAX; i++)
		{
			float life = (float)ran(minLife, maxLife);
			particles.push_back(
				Particle{
				life,
				life,
				minVel + Vec3d(float(ran(minJitter, maxJitter)) / 100.f, float(ran(minJitter, maxJitter)) / 100.f, float(ran(minJitter, maxJitter)) / 100.f),
				source,
				(float)ran(minMass, maxMass) / 100.f,
				(float)ran(maxRadius, minRadius) / 50.f,
				source
			}
			);
		}


#ifndef CACHE
	}
#endif

	// update particles
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].life -= delta;

		for (int j = 0; j < force.size(); j++)
		{
			particles[i].vel +=
				force[j]->relForce(particles[i].pos, particles[i].mass) * delta;
		}

		particles[i].pos += particles[i].vel * delta;

		particles[i].life--;
	}

	// remove dead particles
	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i].life < 0)
		{
			particles.erase(particles.begin() + i);
			i--;
		}
	}
}

void PSystem::draw(float t)
{
	if (!started)
	{		
		loadShaders();

		noiseVol(32);

		loadTextures();

		started = true;
	}
	// TODO: remove
#ifdef CACHE
	if (particleCache.empty())
		return;
#endif

	std::vector<Particle> tPart;

#ifdef CACHE
	// search for best match
	for (auto i = particleCache.begin(); i != particleCache.end(); i++)
	{
		// perfect match
		if (t == i->first)
		{
			tPart = i->second;
			break;
		}
		else if (t < i->first)
		{
			tPart = (i--)->second;
			break;
		}
	}
#else
	tPart = particles;
#endif
	
#ifdef NO_SMOKE
	for (size_t i = 0; i < tPart.size(); i++)
		tPart[i].draw();
#else
	Vec4f offset[6] =
	{
		Vec4f(-1, 1, 0, 1),
		Vec4f(1, 1, 0, 1),
		Vec4f(-1, -1, 0, 1),

		Vec4f(1, -1, 0, 1),
		Vec4f(-1, -1, 0, 1),
		Vec4f(1, 1, 0, 1)
	};

	Vec2f texCoord[4] =
	{
		Vec2f(0, 0),
		Vec2f(1, 0),
		Vec2f(0, 1),
		Vec2f(1, 1)
	};

	// sort particles first
	std::sort(tPart.begin(), tPart.end(), ParticleSort);

	vector<ParticleUpload> upload;

	// setup for render
	for (auto p : tPart)
	{
		for (int i = 0; i < 6; i++)
		{
			ParticleUpload pu;
			pu.loc = offset[i];
			pu.pos = Vec4f(p.pos[0], p.pos[1], p.pos[2], 1);
			pu.size = p.radius;
			pu.texCoord = texCoord[i];
			pu.life = p.life / p.maxLife;

			pu.origin = Vec3f(p.source[0], p.source[1], p.source[2]);

			upload.push_back(pu);
		}
	}

	if (upload.size() == 0)
		return;

	glUseProgram(smokeShader);
	
	Mat4f invView = glGetMatrix(GL_MODELVIEW_MATRIX);
	invView = invView.inverse();
	
	glUniformMatrix4fv(
		glGetUniformLocation(smokeShader, "invView"),
		1, GL_TRUE, &invView[0][0]
	);

	Mat4f invProjection = glGetMatrix(GL_PROJECTION_MATRIX);
	invProjection = invProjection.inverse();

	glUniformMatrix4fv(
		glGetUniformLocation(smokeShader, "invProj"),
		1, GL_TRUE, &invProjection[0][0]
	);

	Mat4f modelView =
		glGetMatrix(GL_MODELVIEW_MATRIX);

	glUniformMatrix4fv(
		glGetUniformLocation(smokeShader, "modelView"),
		1, GL_TRUE, &modelView[0][0]
	);

	Mat4f worldViewProjection =
		glGetMatrix(GL_PROJECTION_MATRIX)
		* glGetMatrix(GL_MODELVIEW_MATRIX);

	glUniformMatrix4fv(
		glGetUniformLocation(smokeShader, "worldViewProjection"),
		1, GL_TRUE, &worldViewProjection[0][0]
	);

	glUniform1i(
		glGetUniformLocation(smokeShader, "colorTex"), 0
	);
	
	glUniform1i(
		glGetUniformLocation(smokeShader, "depthTex"), 1
	);

	glUniform1i(
		glGetUniformLocation(smokeShader, "volume"), 2
	);

	glUniform2f(
		glGetUniformLocation(smokeShader, "screenDim"), w_width, w_height
	);

	Vec3f eyeVec = Camera::mLookAt - Camera::mPosition;
	eyeVec.normalize();

	glUniform3f(
		glGetUniformLocation(smokeShader, "eyeDir"),
		eyeVec[0], eyeVec[1], eyeVec[2]
	);

	Vec4f octaveOff[4];

	for (int i = 0; i < 4; i++)
	{
		octaveOff[i][0] = -t * .05f;
		octaveOff[i][1] = 0.f;
		octaveOff[i][2] = 0.f;
		octaveOff[i][3] = 0.f;
	}

	glUniform4fv(
		glGetUniformLocation(smokeShader, "octaveOff"),
		4, (float*)&octaveOff
	);

	GLuint vBuffer;

	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, smoke);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, densityTex);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ADD);

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		upload.size() * sizeof(ParticleUpload),
		&upload.at(0), GL_STATIC_DRAW);

	for (int i = 0; i < 5; i++)
		glEnableVertexAttribArray(i);

	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	
	glVertexAttribPointer(glGetAttribLocation(smokeShader, "loc"), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(0));		
	glVertexAttribPointer(glGetAttribLocation(smokeShader, "pos"), 4, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(sizeof(Vec4f)));
	glVertexAttribPointer(glGetAttribLocation(smokeShader, "size"), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(sizeof(Vec4f) * 2));		
	glVertexAttribPointer(glGetAttribLocation(smokeShader, "texCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(sizeof(Vec4f) * 2 + sizeof(float)));

	//printf("E %i %i\n", glGetError(), glGetAttribLocation(smokeShader, "texCoord"));

	glVertexAttribPointer(glGetAttribLocation(smokeShader, "life"), 1, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(sizeof(Vec4f) * 2 + sizeof(float) + sizeof(Vec2f)));
	glVertexAttribPointer(glGetAttribLocation(smokeShader, "source"), 3, GL_FLOAT, GL_FALSE, sizeof(ParticleUpload), BUFFER_OFFSET(sizeof(Vec4f) * 2 + 2 * sizeof(float) + sizeof(Vec2f)));
	
	glDrawArrays(GL_TRIANGLES, 0, upload.size());
	
	for (int i = 0; i < 5; i++)
		glDisableVertexAttribArray(i);
	
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
	glDeleteBuffers(1, &vBuffer);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glDepthMask(true);
#endif
}

void PSystem::setSource(Vec3d s)
{
	source = s;
}

void PSystem::setEmitRate(float er)
{
	maxSpawn = minSpawn = er;
}

void PSystem::setLaunchDir(Vec3d dir)
{
	minVel = minVelocity + dir;
}

void PSystem::setMinVelocity(Vec3d vel)
{
	minVelocity = vel;
}

void PSystem::setMaxJitter(int jit)
{
	maxJitter = jit;
}

Force* PSystem::getForce(size_t i)
{
	return force.at(i);
}

void PSystem::createStencil()
{
	glGenTextures(1, &cStencil);
}

void PSystem::copyStencil()
{
	glEnable(GL_TEXTURE_2D);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,
		0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,
		0, GL_TEXTURE_HEIGHT, &h);
	
	// TODO: fix
	
	GLfloat* pixels = new GLfloat[w * h];
	GLfloat* pixelsRGB = new GLfloat[w * h * 3];

	glBindTexture(GL_TEXTURE_2D, depth);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	
	for (int i = 0; i < w_width * w_height; i++)
	{
		pixels[i] = .5;
		pixelsRGB[i * 3] = pixels[i];
		pixelsRGB[i * 3 + 1] = pixels[i];
		pixelsRGB[i * 3 + 2] = pixels[i];
	}
	
	glBindTexture(GL_TEXTURE_2D, cStencil);
	
	// set wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w_width, w_height,
		0, GL_RGB, GL_FLOAT, pixelsRGB);

	delete[] pixels, pixelsRGB;

	glDisable(GL_TEXTURE_2D);
}

void PSystem::loadShaders()
{
	createProgram(smokeShader, vs, fs, "Smoke.vs", "Smoke.fs");
}

void PSystem::loadTextures()
{
	// load smoke texture
	int width, height;
	unsigned char* data = readBMP("Smoke/Smoke.bmp", width, height);

	glEnable(GL_TEXTURE_1D);

	glGenTextures(1, &smoke);
	glBindTexture(GL_TEXTURE_1D, smoke);
	
	// set wrap
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	
	glTexImage1D(GL_TEXTURE_1D, 0, 3, width,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);

	delete[] data;

	glDisable(GL_TEXTURE_1D);
}

float densityC(Vec3i pos, C4* pixel, unsigned int volSize)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		if (pos[i] < 0)
			pos[i] += volSize;

		pos[i] %= volSize;
	}

	int index = pos[0] + pos[1] * volSize + pos[2] * volSize * volSize;

	return (float)pixel[index].w / 128.0f;
}

float rp()
{
	float ret = (float)((rand() % 20000) - 10000);
	return ret / 10000.0f;
}

void PSystem::noiseVol(unsigned int volSize)
{
	Vec3f density;

	float height = .5f;

	C4* data = new C4[volSize * volSize * volSize];
	for (unsigned int i = 0; i < volSize * volSize * volSize; i++)
		data[i].w = (char)(rp() * 128.0f);

	for (unsigned int z = 0; z < volSize; z++)
	{
		for (unsigned int y = 0; y < volSize; y++)
		{
			for (unsigned int x = 0; x < volSize; x++)
			{
				density[0] = densityC(Vec3i(x + 1, y, z), data, volSize) - densityC(Vec3i(x - 1, y, z), data, volSize) / height;
				density[1] = densityC(Vec3i(x, y + 1, z), data, volSize) - densityC(Vec3i(x, y - 1, z), data, volSize) / height;
				density[2] = densityC(Vec3i(x, y, z + 1), data, volSize) - densityC(Vec3i(x, y, z - 1), data, volSize) / height;

				density.normalize();

				Vec3i pos = Vec3i(x, y, z);

				// set norm
				for (unsigned int i = 0; i < 3; i++)
				{
					if (pos[i] < 0)
						pos[i] += volSize;

					pos[i] %= volSize;
				}

				int index = pos[0] + pos[1] * volSize + pos[2] * volSize * volSize;

				data[index].x = (char)(density[0] * 128.0f);
				data[index].y = (char)(density[1] * 128.0f);
				data[index].z = (char)(density[2] * 128.0f);
			}
		}
	}

	glGenTextures(1, &densityTex);
	glBindTexture(GL_TEXTURE_3D, densityTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, volSize, volSize, volSize,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	delete[] data;
}
