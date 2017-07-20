#ifndef OBJECT_FILE_LOADER_H
#define OBJECT_FILE_LOADER_H

// loading files

#include <fstream>

// get line in file

#include <sstream>

// Need these headers to support the array types I want

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <string>
#include <stdint.h>

#include <iostream>
#include <fstream>

#include "Helper.h"
#include "vec.h"
#include "mat.h"
#include "bitmap.h"

// assignment specific vars
#include "modelerview.h"

// namespace time

using namespace std;// load all std:: things

namespace std
{
	template <>
	struct equal_to<Vec3f> : public unary_function<Vec3f, bool>
	{
		bool operator() (const Vec3f &a, const Vec3f &b) const
		{
			return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
		}
	};

	template<>
	struct hash<Vec3f> : public unary_function<Vec3f, size_t>
	{
		std::size_t operator() (const Vec3f& a) const
		{
			return std::hash<float>{}(a[0]) ^ std::hash<float>{}(a[1]) ^ std::hash<float>{}(a[2]);
		}
	};
};

struct Material
{
	std::string name;
	
	Vec4f ambient;

	Vec4f diffuse;

	Vec4f specular;

	float shininess;

	float opticalDensity;

	float alpha;

	bool specularb;

	std::string texture_path;

	GLuint texID = 0;

	int texWidth;

	int texHeight;
	
	unsigned char* texture = nullptr;
};

struct MaterialUpload
{
	Vec4f ambient;

	Vec4f diffuse;

	Vec4f specular;

	float shininess;

	float opticalDensity;

	float alpha;

	bool specularb;

	GLuint texID;
};

struct Vertex
{
	Vec3f position;
	Vec3f normal;
	Vec2f texcoord;
};

struct VertexDataforMap
{
	Vec3f normal;
	Vec2f texcoord;
	unsigned int index;
};

class ObjLoader
{
public:

	ObjLoader();

	~ObjLoader(); // destruction method

	void Load(char *filename); // Load the object with its materials
								// get the number of materials used

	void reset();

	static void setLightAt(Vec3f lightAt);

	static void setLightPos(Vec3f lightPos);

	static void setupLighting(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

	static void setGlobalDiffuseAmbient(Vec4f diffuseAmbient);

	static void setGlobalAmbient(Vec4f globalAmbient);

	static void setShadow(bool b);

	static void setViewMatrix();

	static void applyViewMatrix();

	void setImage(unsigned int mat_num, const char* filename);

	void draw(int type); // choose which draw type

	void draw(); // regular draw

	static void updateStencil(); // update stored stencil buffer

	void drawStencil(); // draw for the stencil buffer

	void drawShadow(); // shadow map draw

	const size_t getMat_Num()
	{
		return material.size();
	}

	// get the material pointer

	Material* getMaterials()
	{
		return &material.at(0);
	}

	// get the number of vertices in the object

	const unsigned int getNumVertices()
	{
		return numVerts;
	}

	// get the number of indices in the object

	const size_t getNumIndices()
	{
		return vx_array_i.size();
	}

	// get the number of material indices in the object

	const size_t getNumMaterialIndices()
	{
		return attributes.size();
	}

	// get the number of materials in the object

	const size_t getNumMaterials()
	{
		return material.size();
	}

	// get a pointer to the verticies

	const Vertex* getVertices()
	{
		return vertex_final_array;
	}

	// get the vertex stride

	unsigned int getVertexStride()
	{
		return sizeof(Vertex);
	}

	// get a pointer to the indices

	const unsigned int* getIndices(unsigned int mat_num)
	{
		return &vx_array_i[mat_num].at(0);
	}

	// get the number of meshes used to draw the object

	const unsigned int getNumMesh()
	{
		return mesh_num;
	}

	// get a pointer to a certain material

	Material* getMaterial(unsigned int mat_num)
	{
		return &material.at(mat_num);
	}

private:

	// Create a vector to store the verticies

	void Load_Geometry(char *filename); // load the verticies and indices

	void Material_File(string filename, string matfile, unsigned long* tex_num); // load the material file

	void Base_Mat(Material *mat); // the basic material

	unordered_map <unsigned int, vector<unsigned int>> vx_array_i; // store the indecies for the vertex

	vector <float> vx_array; // store the verticies in the mesh

	Vertex* vertex_final_array = nullptr; // the final verticies organized for Direct3D to draw

	vector <Material> material; // the materials used on the object

	vector <unsigned int> attributes;

	unordered_map <Vec3f, vector<VertexDataforMap>> vertexmap; // map for removing doubles

	unsigned int numVerts;

	unsigned int mesh_num; // the number of meshes

	static GLuint shadowProgram;
	static GLuint shadowVS;
	static GLuint shadowFS;

	// lighting
	static Vec3f lightPos, lightAt, lightUp;
	static Mat4f lightViewProj;
	static Mat4f viewMatrix;
	static GLfloat viewMatrixGL[16];
	static Vec4f diffuseAmbient, globalAmbient;
	static GLuint shadowOn;
};

#endif
