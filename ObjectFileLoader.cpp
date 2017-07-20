#include <GL/glew.h>
#include "ObjectFileLoader.h" // link to the header
#include "Shader_Helper.h"
#include "robotarm.h"
#include "camera.h"
#include <climits>

GLuint ObjLoader::shadowProgram = 0;
GLuint ObjLoader::shadowVS = 0;
GLuint ObjLoader::shadowFS = 0;
Vec3f ObjLoader::lightPos, ObjLoader::lightAt, ObjLoader::lightUp = Vec3f(0, 1, 0);
Mat4f ObjLoader::lightViewProj;
Mat4f ObjLoader::viewMatrix;
GLfloat ObjLoader::viewMatrixGL[16];
Vec4f ObjLoader::diffuseAmbient, ObjLoader::globalAmbient;
GLuint ObjLoader::shadowOn = false;

/***************************************************************************
OBJ Loading
***************************************************************************/

ObjLoader::ObjLoader()
{

}

ObjLoader::~ObjLoader()
{
	// delete all data

	reset();
}

void ObjLoader::reset()
{
	delete[] vertex_final_array;

	for (auto i : material)
	{
		if (i.texID != 0)
		{
			glDeleteTextures(1, &i.texID);
			delete[] i.texture;
		}
	}
}

void ObjLoader::Base_Mat(Material *mat)
{
	mat->ambient = Vec4f(0.2f, 0.2f, 0.2f, 1.f);
	mat->diffuse = Vec4f(0.8f, 0.8f, 0.8f, 1.f);
	mat->specular = Vec4f(1.0f, 1.0f, 1.0f, 1.f);
	mat->shininess = 0;
	mat->opticalDensity = 0;
	mat->alpha = 1.0f;
	mat->specularb = false;
}

void ObjLoader::Material_File(string filename, string matfile, unsigned long* tex_num)
{
	// find the directory to the material file

	string directory = filename.substr(0, filename.find_last_of('/') + 1);

	matfile = directory + matfile; // the location of the material file to the program

								   // open the file

	ifstream matFile_2(matfile);

	if (matFile_2.is_open()) // If obj file is open, continue
	{
		string line_material;// store each line of the file here

		while (!matFile_2.eof()) // Start reading file data as long as we have not reached the end
		{
			getline(matFile_2, line_material); // Get a line from file

											   // convert to a char to do pointer arithmetics

			char* ptr = (char*)line_material.c_str();

			if (ptr[0] == '	')// this is tab not space (3ds max uses tabs which would otherwise confuse this program without this line)
			{
				ptr++;// move address up
			}

			// This program is for standard Wavefront Objects that are triangulated and have normals stored in the file.  This reader has been tested with 3ds Max and Blender.

			if (ptr[0] == 'n' && ptr[1] == 'e' && ptr[2] == 'w' && ptr[3] == 'm'
				&& ptr[4] == 't' && ptr[5] == 'l') // new material
			{
				ptr += 6 + 1;// move address up

				Material mat; // allocate memory to create a new material

				Base_Mat(&mat); // init the material

				mat.name = ptr; // set the name of the material

				material.push_back(mat); // add to the vector

				*tex_num = material.size() - 1;
			}
			else if (ptr[0] == 'K' && ptr[1] == 'a') // ambient
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f %f %f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).ambient[0],
					&material.at(*tex_num).ambient[1],
					&material.at(*tex_num).ambient[2]);

				material.at(*tex_num).ambient[3] = 1.f;
			}
			else if (ptr[0] == 'K' && ptr[1] == 'd') // diffuse
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f %f %f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).diffuse[0],
					&material.at(*tex_num).diffuse[1],
					&material.at(*tex_num).diffuse[2]);

				material.at(*tex_num).diffuse[3] = 1.f;
			}
			else if (ptr[0] == 'K' && ptr[1] == 's') // specular
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f %f %f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).specular[0],
					&material.at(*tex_num).specular[1],
					&material.at(*tex_num).specular[2]);

				material.at(*tex_num).specular[3] = 1.f;
			}
			else if (ptr[0] == 'N' && ptr[1] == 's') // shininess
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).shininess);
			}
			else if (ptr[0] == 'N' && ptr[1] == 'i') // refraction
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).opticalDensity);
			}
			else if (ptr[0] == 'd') // transparency
			{
				ptr++;// move address up

				sscanf(ptr, "%f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).alpha);
			}
			else if (ptr[0] == 'T' && ptr[0] == 'r') // another way to store transparency
			{
				ptr += 2;// move address up

				sscanf(ptr, "%f ",							// Read floats from the line: v X Y Z
					&material.at(*tex_num).alpha);
			}
			else if (ptr[0] == 'm' && ptr[1] == 'a' && ptr[2] == 'p' && ptr[3] == '_'
				&& ptr[4] == 'K' && ptr[5] == 'd') // image texture
			{
				ptr += 7;// move address up

				material.at(*tex_num).texture_path = ptr; // the material file
																	  // load the file
																	  // convert to a LPWSTR

				string filename;
				filename = directory + material.at(*tex_num).texture_path;

				// load bmp file

				material.at(*tex_num).texture = readBMP(filename.c_str(), material.at(*tex_num).texWidth, material.at(*tex_num).texHeight);

				glEnable(GL_TEXTURE_2D);

				glGenTextures(1, &material.at(*tex_num).texID);

				glBindTexture(GL_TEXTURE_2D, material.at(*tex_num).texID);

				// set wrap
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				glTexImage2D(GL_TEXTURE_2D, 0, 3, material.at(*tex_num).texWidth, material.at(*tex_num).texHeight,
					0, GL_RGB, GL_UNSIGNED_BYTE, material.at(*tex_num).texture);

				glBindTexture(GL_TEXTURE_2D, 0);

				glDisable(GL_TEXTURE_2D);
			}
		}

		matFile_2.close(); // close the file
	}
	else
	{
		cout << "Error (OBJECT LOADER): Cannot Find Material File- " << matfile.c_str() << endl;
	}
}

void ObjLoader::Load_Geometry(char *filename)
{
	// delete past memory

	if (vertex_final_array != nullptr)
		delete vertex_final_array;

	// allocate memory to the vectors on the heap

	vx_array_i.clear();

	vx_array.clear();

	material.clear();

	attributes.clear();

	mesh_num = 0;

	// create maps to store the lighting values for the material

	ifstream objFile(filename); // open the object file

	if (objFile.is_open()) // If the obj file is open, continue
	{
		// initialize the strings needed to read the file

		string line;

		string mat;

		// the material that is used

		unsigned long material_num = 0;

		unsigned long tex_num = 0;

		numVerts = 0;

		// Store the coordinates

		vector <float> vn_array;

		vector <float> vt_array;

		while (!objFile.eof()) // start reading file data
		{
			getline(objFile, line);	// get line from file

									// convert to a char to do pointers

			const char* ptr = line.c_str();

			if (ptr[0] == 'm' && ptr[1] == 't' && ptr[2] == 'l' && ptr[3] == 'l'  && ptr[4] == 'i' && ptr[5] == 'b' && ptr[6] == ' ') // load the material file
			{
				ptr += 7; // move the address up

				const string material_file = ptr;// the material file

				Material_File(filename, material_file, &tex_num); // read the material file and update the number of materials
			}
			if (ptr[0] == 'v' && ptr[1] == ' ') // the first character is a v: on this line is a vertex stored.
			{
				ptr += 2; // move address up

						  // store the three tmp's into the verticies

				float tmp[3];

				sscanf(ptr, "%f %f %f ", // read floats from the line: X Y Z
					&tmp[0],
					&tmp[1],
					&tmp[2]);

				vx_array.push_back(tmp[0]);
				vx_array.push_back(tmp[1]);
				vx_array.push_back(tmp[2]);
			}

			else if (ptr[0] == 'v' && ptr[1] == 'n') // the vertex normal
			{
				ptr += 2;

				// store the three tmp's into the verticies

				float tmp[3];

				sscanf(ptr, "%f %f %f ", // read floats from the line: X Y Z
					&tmp[0],
					&tmp[1],
					&tmp[2]);

				vn_array.push_back(tmp[0]);
				vn_array.push_back(tmp[1]);
				vn_array.push_back(tmp[2]);
			}

			else if (ptr[0] == 'v' && ptr[1] == 't') // texture coordinate for a vertex
			{
				ptr += 2;

				// store the two tmp's into the verticies

				float tmp[2];

				sscanf(ptr, "%f %f ",	// read floats from the line: X Y Z
					&tmp[0],
					&tmp[1]);

				vt_array.push_back(tmp[0]);
				vt_array.push_back(tmp[1]);
			}
			else if (ptr[0] == 'u' && ptr[1] == 's' && ptr[2] == 'e' && ptr[3] == 'm' && ptr[4] == 't' && ptr[5] == 'l') // which material is being used
			{
				mat = line.substr(6 + 1, line.length());// save so the comparison will work

														// add new to the material name so that it matches the names of the materials in the mtl file

				for (unsigned long num = 0; num < tex_num + 1; num++)// find the material
				{
					if (mat == material.at(num).name)// matches material in mtl file
					{
						material_num = num;
					}
				}
			}
			else if (ptr[0] == 'f') // store the faces in the object
			{
				ptr++;

				int vertexNumber[3] = { 0, 0, 0 };
				int normalNumber[3] = { 0, 0, 0 };
				int textureNumber[3] = { 0, 0, 0 };

				// no texture
				if (string(ptr).find("//") != -1)
				{
					sscanf(ptr, "%i//%i %i//%i %i//%i ",
						&vertexNumber[0],
						&normalNumber[0],
						&vertexNumber[1],
						&normalNumber[1],
						&vertexNumber[2],
						&normalNumber[2]
					);

					textureNumber[0] = INT_MAX;
					textureNumber[1] = INT_MAX;
					textureNumber[2] = INT_MAX;
				}
				else
				{
					sscanf(ptr, "%i/%i/%i %i/%i/%i %i/%i/%i ",
						&vertexNumber[0],
						&textureNumber[0],
						&normalNumber[0],
						&vertexNumber[1],
						&textureNumber[1],
						&normalNumber[1],
						&vertexNumber[2],
						&textureNumber[2],
						&normalNumber[2]
					); // each point represents an X,Y,Z.
				}

				// create a vertex for this area

				for (int i = 0; i < 3; i++) // loop for each triangle
				{
					Vertex vert;

					vert.position = Vec3f(vx_array.at((vertexNumber[i] - 1) * 3), vx_array.at((vertexNumber[i] - 1) * 3 + 1), vx_array.at((vertexNumber[i] - 1) * 3 + 2));

					vert.normal = Vec3f(vn_array[(normalNumber[i] - 1) * 3], vn_array[(normalNumber[i] - 1) * 3 + 1], vn_array[(normalNumber[i] - 1) * 3 + 2]);

					if (textureNumber[i] != INT_MAX)
						vert.texcoord = Vec2f(vt_array[(textureNumber[i] - 1) * 2], vt_array[(textureNumber[i] - 1) * 2 + 1]);
					else
						vert.texcoord = Vec2f(0, 0);

					unsigned int index = 0;

					bool indexupdate = false;

					if (vertexmap.find(vert.position) != vertexmap.end())
						for (VertexDataforMap vdm : vertexmap[vert.position])
						{
							if (vert.normal == vdm.normal && vert.texcoord == vdm.texcoord) // found the index
							{
								index = vdm.index;

								indexupdate = true;
								break;
							}
						}

					// nothing found

					if (!indexupdate)
					{
						VertexDataforMap tmp;

						index = numVerts;
						tmp.normal = vert.normal;

						tmp.texcoord = vert.texcoord;

						tmp.index = index;

						vertexmap[vert.position].push_back(tmp);

						numVerts++;
					}

					vx_array_i[material_num].push_back(index);
				}
			}
		}

		// create the final verts

		Vertex vert;

		vertex_final_array = new Vertex[numVerts];

		for (unordered_map<Vec3f, vector<VertexDataforMap>>::iterator i = vertexmap.begin(); i != vertexmap.end(); i++)
		{
			for (VertexDataforMap vdm : i->second)
			{
				vertex_final_array[vdm.index].position = i->first;

				vertex_final_array[vdm.index].normal = vdm.normal;

				vertex_final_array[vdm.index].texcoord = vdm.texcoord;
			}
		}
	}
	else
	{
		printf("Error (OBJECT LOADER):  Cannot Find Object File- %s\n", filename);
	}
}

void ObjLoader::Load(char *filename)
{
	if (shadowProgram == 0)
	{
		createProgram(shadowProgram,
			shadowVS, shadowFS, "Shadow.vs", "Shadow.fs");
	}

	Load_Geometry(filename);
}

void ObjLoader::setLightAt(Vec3f lightAt)
{
	ObjLoader::lightAt = lightAt;
}

void ObjLoader::setLightPos(Vec3f lightPos)
{
	ObjLoader::lightPos = lightPos;
}

void ObjLoader::setupLighting(
	GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	// set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);

	// set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// create lookat
	gluLookAt(lightPos[0], lightPos[1], lightPos[2],
		lightAt[0], lightAt[1], lightAt[2],
		lightUp[0], lightUp[1], lightUp[2]);

	// calculate lightViewProjection
	lightViewProj =
		glGetMatrix(GL_PROJECTION_MATRIX) *
		glGetMatrix(GL_MODELVIEW_MATRIX);
}

void ObjLoader::setGlobalDiffuseAmbient(Vec4f diffuseAmbient)
{
	ObjLoader::diffuseAmbient = diffuseAmbient;
}

void ObjLoader::setGlobalAmbient(Vec4f globalAmbient)
{
	ObjLoader::globalAmbient = globalAmbient;
}

void ObjLoader::setShadow(bool b)
{
	ObjLoader::shadowOn = b;
}

void ObjLoader::setImage(unsigned int mat_num, const char* filename)
{
	if (material.at(mat_num).texID != 0)
		delete[] material.at(mat_num).texture;

	material.at(mat_num).texture = readBMP(filename, material.at(mat_num).texWidth, material.at(mat_num).texHeight);

	glEnable(GL_TEXTURE_2D);

	if (material.at(mat_num).texID == 0)
		glGenTextures(1, &material.at(mat_num).texID);

	glBindTexture(GL_TEXTURE_2D, material.at(mat_num).texID);

	// set wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, material.at(mat_num).texWidth, material.at(mat_num).texHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, material.at(mat_num).texture);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

void ObjLoader::setViewMatrix()
{
	viewMatrix = glGetMatrix(GL_MODELVIEW_MATRIX);
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrixGL);

	glLoadIdentity();
}

void ObjLoader::applyViewMatrix()
{
	glLoadMatrixf(viewMatrixGL);
}

void ObjLoader::draw(int type)
{
	switch(type)
	{
	case  0:
		draw();
		break;
	case 1:
		drawStencil();
		break;
	case 2:
		drawShadow();
		break;
	}
}

void ObjLoader::draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_AMBIENT);
	glEnable(GL_DIFFUSE);
	glEnable(GL_SPECULAR);
	glEnable(GL_SHININESS);

	// setup normals, verts, and texcroods
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertex_final_array[0].position);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &vertex_final_array[0].normal);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertex_final_array[0].texcoord);

	for (int i = 0; i < getNumMaterials(); i++)
	{
		// set texture
		glBindTexture(GL_TEXTURE_2D, material[i].texID);

		// setup mat
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &material[i].ambient[0]);

		if (material[i].texID == 0)
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &material[i].diffuse[0]);
		else
		{
			Vec4f white = { 1, 1, 1, 1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*)&white);
		}
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &material[i].specular[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, material[i].shininess);

		glDrawElements(GL_TRIANGLES, vx_array_i[i].size(), GL_UNSIGNED_INT, &vx_array_i[i].at(0));
	}

	glDisable(GL_SHININESS);
	glDisable(GL_SPECULAR);
	glDisable(GL_DIFFUSE);
	glDisable(GL_AMBIENT);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ObjLoader::drawStencil()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	// setup normals, verts, and texcroods
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertex_final_array[0].position);

	for (int i = 0; i < getNumMaterials(); i++)
		glDrawElements(GL_TRIANGLES, vx_array_i[i].size(), GL_UNSIGNED_INT, &vx_array_i[i].at(0));

	glDisableClientState(GL_VERTEX_ARRAY);

	Mat4f worldViewProjection =
		glGetMatrix(GL_MODELVIEW_MATRIX);
}

void ObjLoader::updateStencil()
{
	// swap back buffers
	GLuint tmp = depthBack;
	depthBack = depth;
	depth = tmp;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		depth, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}
}

void ObjLoader::drawShadow()
{
	glUseProgram(shadowProgram);

	glUniform1i(
		glGetUniformLocation(shadowProgram, "shadowOn"),
		shadowOn
	);

	glUniform4fv(
		glGetUniformLocation(shadowProgram, "diffuseAmbient"),
		1, &diffuseAmbient[0]
	);

	glUniform4fv(
		glGetUniformLocation(shadowProgram, "globalAmbient"),
		1, &globalAmbient[0]
	);

	// get world matrix
	Mat4f worldMatrix = glGetMatrix(GL_MODELVIEW_MATRIX);

	// bind world view projection
	Mat4f worldViewProjection =
		glGetMatrix(GL_PROJECTION_MATRIX)
		* viewMatrix
		* worldMatrix;

	glUniformMatrix4fv(
		glGetUniformLocation(shadowProgram, "worldViewProjection"),
		1, GL_TRUE, &worldViewProjection[0][0]
	);

	// bind light projection
	Mat4f modelLightViewProjection = lightViewProj * worldMatrix;
	
	glUniformMatrix4fv(
		glGetUniformLocation(shadowProgram, "modelLightViewProjection"),
		1, GL_TRUE, &modelLightViewProjection[0][0]
	);

	// bind light pos
	glUniform3fv(
		glGetUniformLocation(shadowProgram, "lightpos"),
		1, &lightPos[0]
	);

	Vec3f lightDir = lightAt - lightPos;

	glUniform3fv(
		glGetUniformLocation(shadowProgram, "lightdir"),
		1, &lightDir[0]
	);

	Vec3f eyeVec = Camera::mLookAt - Camera::mPosition;

	glUniform3fv(
		glGetUniformLocation(shadowProgram, "eyeVec"),
		1, &eyeVec[0]
	);

	// bind shadow map
	glUniform1i(
		glGetUniformLocation(shadowProgram, "shadowmap"), 0
	);

	// bind dim
	glUniform2f(
		glGetUniformLocation(shadowProgram, "dim"),
		w_width, w_height
	);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthBack);

	// setup texture
	glUniform1i(
		glGetUniformLocation(shadowProgram, "tex"), 1
	);

	// go to texture 1 for rendering
	glActiveTexture(GL_TEXTURE1);

	for (int i = 0; i < 3; i++)
		glEnableVertexAttribArray(i);

	for (int i = 0; i < getNumMaterials(); i++)
	{
		// setup color vals
		
		glUniform4fv(
			glGetUniformLocation(shadowProgram, "ambient"),
			1, (float*)&material[i].ambient[0]
		);
		
		glUniform4fv(
			glGetUniformLocation(shadowProgram, "diffuse"),
			1, (float*)&material[i].diffuse[0]
		);

		glUniform4fv(
			glGetUniformLocation(shadowProgram, "specular"),
			1, (float*)&material[i].specular[0]
		);

		glUniform1f(
			glGetUniformLocation(shadowProgram, "shininess"),
			material[i].shininess
		);

		// bind texture

		if (material[i].texID != 0)
		{
			glBindTexture(GL_TEXTURE_2D, material[i].texID);

			glUniform1i(
				glGetUniformLocation(shadowProgram, "usingTex"),
				true
			);
		}
		else
		{
			glUniform1i(
				glGetUniformLocation(shadowProgram, "usingTex"),
				false
			);
		}

		// create upload buffer
		// TODO: store
		GLuint vBuffer;
		glGenBuffers(1, &vBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			getNumVertices() * sizeof(Vertex),
			vertex_final_array, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glVertexAttribPointer(glGetAttribLocation(shadowProgram, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(glGetAttribLocation(shadowProgram, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(Vec3f)));
		glVertexAttribPointer(glGetAttribLocation(shadowProgram, "texcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(2 * sizeof(Vec3f)));

		glDrawElements(GL_TRIANGLES, vx_array_i[i].size(), GL_UNSIGNED_INT, &vx_array_i[i].at(0));

		glDeleteBuffers(1, &vBuffer);
	}

	for (int i = 0; i < 3; i++)
		glDisableVertexAttribArray(i);

	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}