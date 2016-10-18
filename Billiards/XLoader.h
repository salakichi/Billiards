#pragma once

// PROJECT ASURAさんのXファイルレンダラーを改良したもの
// 元ソース： http://asura.iaigiri.com/OpenGL/gl44.html
//
// 改良内容
// ・ロード時のバグ修正
// ・VBOでレンダリング
// ・様々な画像形式のサポート
//
// 諸注意
// 一部効率悪いことしているので要改良


#include <iostream>
#include <cmath>
#include <ctype.h>
#include <list>
#include <sys/stat.h>
#include "MyGlew.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Texture.h"
using namespace std;

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

typedef unsigned int uint;

//
// forward declarations
//
class XFace;
class XMesh;
class XMaterial;
class XModel;

//
// XColor struct
//
struct XColor
{
	float r, g, b, a;
	operator float*() { return (float*)&r; }
	operator const float*() const { return (const float*)&r; }
	XColor& operator=(const XColor& c) {
		r = c.r; g = c.g; b = c.b; a = c.a;
		return *this;
	}
};

//
// XBoundingSphere struct
//
struct XBoundingSphere
{
	glm::vec3 center;
	float radius;
};

//
// XBoundingBox struct
//
struct XBoundingBox
{
	glm::vec3 min;
	glm::vec3 max;
};

//
// XFace class
//
class XFace
{
public:
	int element;
	int indexMaterial;
	glm::uvec4 indexVertices;
	glm::uvec4 indexNormals;
	glm::uvec4 indexTexCoords;
	XFace();
};

//
// XMaterial class
//
class XMaterial
{
public:
	char name[MAX_PATH];
	int index;
	XColor ambient;
	XColor diffuse;
	XColor specular;
	XColor emissive;
	float power;
	char textureFileName[MAX_PATH];
	int imageTex;
	XMaterial &operator = (XMaterial &ob);
	XMaterial();
	void SetName(char *str);
	void SetTextureFileName(char *fileName, char* dirName);
	void LoadImageTex();
};

//
// XMesh class
//
class XMesh
{
public:
	char name[MAX_PATH];
	int numVertices;
	int numNormals;
	int numTexCoords;
	int numFaces;
	glm::vec3* vertex;
	glm::vec3* normal;
	glm::vec2* texcoord;
	XFace* face;
	XMesh();
	void Release();
	void SetName(char *str);
};

struct MaterialStruct
{
	XColor ambient;
	XColor diffuse;
	XColor specular;
	XColor emissive;
	int imageTex;
};

struct MaterialList
{
	int materialIndex;
	int elementNum;
	int faceNum;
};

//
// XModel class
//
class XModel
{
private:
	void ComputeBoundingSphere(vector<XMesh*> meshList);
	void ComputeBoundingBox(vector<XMesh*> meshList);
	int numMeshes;
	int numMaterials;
	list<XMaterial> material;

	MaterialStruct* matArr;
	MaterialList** matList;
	GLuint* vbo;

	int AddMaterial(XMaterial ob);
	void ChangeMaterialIndex(XMaterial ob, int index);
	void DeleteMaterial(XMaterial ob);

	bool rotationFlag;

public:
	XBoundingSphere sphere;
	XBoundingBox box;

	glm::vec3 position;
	glm::mat4 rotation;

	bool Load(char *filename, float scale = 1.0f);
	void Release();
	void RenderMesh(int index);
	void Render();
	void EnableRotate()
	{
		rotationFlag = true;
	}
	XModel();
};