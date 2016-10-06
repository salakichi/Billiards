#include "XLoader.h"

//
// global Variables
//
static char* Pointer;
static char Token[1024];


//-------------------------------------------------------------------------------------------------
// Name : CheckToken()
// Desc : �w�肳�ꂽ������ƃg�[�N�����`�F�b�N����
//-------------------------------------------------------------------------------------------------
static bool CheckToken(const char* str)
{
	if (strcmp(Token, str) == 0)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : �g�[�N�����擾����
//-------------------------------------------------------------------------------------------------
static void GetToken()
{
	char *p = Pointer;
	char *q = Token;

	while (*p != '\0' && strchr(" \t\r\n,;\"", *p))
		p++;

	if (*p == '{' || *p == '}')
		(*q++) = (*p++);
	else
		while (*p != '\0' && !strchr(" \t\r\n,;\"{}", *p))
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

static void GetToken(const char ch)
{
	char *p = Pointer;
	char *q = Token;

	while (*p != '\0' && strchr(" \t\r\n,;\"", *p))
		p++;

	if (*p == '{' || *p == '}')
		(*q++) = (*p++);
	else
		while (*p != '\0' && *p != ch)
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

//--------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : �g�[�N�����擾���w�肳�ꂽ��������r���s��
//--------------------------------------------------------------------------------------------------
static bool GetToken(const char* token)
{
	GetToken();
	if (strcmp(Token, token) != 0)
	{
		cout << "Error : �z��g�[�N���ƓǍ��g�[�N������v���܂���\n";
		cout << "�z��g�[�N���F" << token << endl;
		cout << "�Ǎ��g�[�N���F" << Token << endl;
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------
// Name : GetFloatToken()
// Desc : �g�[�N����float�^�ɕϊ����擾����
//--------------------------------------------------------------------------------------------------
static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

//--------------------------------------------------------------------------------------------------
// Name : GetIntToken()
// Desc : �g�[�N����int�^�ɕϊ����擾����
//--------------------------------------------------------------------------------------------------
static int GetIntToken()
{
	GetToken();
	return (int)atoi(Token);
}

//-------------------------------------------------------------------------------------------------
// Name : SkipNode()
// Desc : �m�[�h���΂�
//-------------------------------------------------------------------------------------------------
static void SkipNode()
{
	while (*Pointer != '\0')
	{
		GetToken();
		if (strchr(Token, '{')) break;
	}
	int count = 1;
	while (*Pointer != '\0' && count > 0)
	{
		GetToken();
		if (strchr(Token, '{')) count++;
		else if (strchr(Token, '}')) count--;
	}
	if (count > 0)
	{
		cout << "Error : �J�b�R����v���Ă��܂���\n";
		return;
	}
}

static int substring(char *src, char* dest, const char *str1, const char *str2)
{
	char *from, *to;
	int len;

	if ((from = strstr(src, str1)) == NULL)
		return 0;
	if ((to = strstr(from, str2)) == NULL)
		return 0;
	len = strlen(from) - strlen(to) + strlen(str2);
	strncpy_s(dest, 1024, from, len);
	dest[len] = '\0';

	return 1;
}

static void strdelete(char *buf, const char *str1)
{
	char *p;
	int strLen, bufLen;

	strLen = strlen(str1);
	while ((p = strstr(buf, str1)) != NULL) {
		bufLen = strlen(p);
		for (int i = 0; i<bufLen - strLen; i++) {
			*(p + i) = *(p + i + strLen);
		}
		*(p + bufLen - strLen) = '\0';
	}
}

static void CommentDelete(char* buf)
{
	char str[1024];

	while ((substring(buf, str, "//", "\n")) != NULL) {
		strdelete(buf, str);
	}
}

////////////////////////////////////////////////////////////////////////
// XFace class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Name : XFace()
// Desc : �R���X�g���N�^
//-------------------------------------------------------------------------------------------------
XFace::XFace()
{
	element = 0;
	indexMaterial = -1;
	for (int i = 0; i<4; i++)
	{
		indexVertices[i] = -1;
		indexNormals[i] = -1;
		indexTexCoords[i] = -1;
	}
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XFace& XFace::operator =(XFace &ob)
{
	element = ob.element;
	indexMaterial = ob.indexMaterial;
	for (int i = 0; i<4; i++)
	{
		indexVertices[i] = ob.indexVertices[i];
		indexNormals[i] = ob.indexNormals[i];
		indexTexCoords[i] = ob.indexTexCoords[i];
	}
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetVertexIndex()
// Desc : ���_�C���f�b�N�X���Z�b�g����
//--------------------------------------------------------------------------------------------------
void XFace::SetVertexIndex(int index[])
{
	for (int i = 0; i<4; i++) indexVertices[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetNormalIndex()
// Desc : �@���C���f�b�N�X���Z�b�g����
//--------------------------------------------------------------------------------------------------
void XFace::SetNormalIndex(int index[])
{
	for (int i = 0; i<4; i++) indexNormals[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetTexCoordIndex()
// Desc : �e�N�X�`�����W�C���f�b�N�X���Z�b�g����
//--------------------------------------------------------------------------------------------------
void XFace::SetTexCoordIndex(int index[])
{
	for (int i = 0; i<4; i++) indexTexCoords[i] = index[i];
}

////////////////////////////////////////////////////////////////////////
// XMaterial class
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Name : XMaterial()
// Desc : �R���X�g���N�^
//--------------------------------------------------------------------------------------------------
XMaterial::XMaterial()
{
	strcpy_s(name, "null");
	index = -1;
	ambient.r = 0.0f;	ambient.g = 0.0f;	ambient.b = 0.0f;	ambient.a = 0.0f;
	diffuse.r = 0.0f;	diffuse.g = 0.0f;	diffuse.b = 0.0f;	diffuse.a = 0.0f;
	specular.r = 0.0f;	specular.g = 0.0f;	specular.b = 0.0f;	specular.a = 0.0f;
	emissive.r = 0.0f;	emissive.g = 0.0f;	emissive.g = 0.0f;	emissive.a = 0.0f;
	power = 0.0f;
	strcpy_s(textureFileName, "null");
	imageTex = 0;
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMaterial& XMaterial::operator =(XMaterial &ob)
{
	strcpy_s(name, ob.name);
	ambient = ob.ambient;
	diffuse = ob.diffuse;
	specular = ob.specular;
	emissive = ob.emissive;
	power = ob.power;
	strcpy_s(textureFileName, ob.textureFileName);
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : ���O���Z�b�g����
//--------------------------------------------------------------------------------------------------
void XMaterial::SetName(char *str)
{
	strcpy_s(name, str);
}

//--------------------------------------------------------------------------------------------------
// Name : SetTextureFileName()
// Desc : �e�N�X�`���t�@�C�������Z�b�g����
//--------------------------------------------------------------------------------------------------
void XMaterial::SetTextureFileName(char *fileName, char* dirName)
{
	strcpy_s(textureFileName, dirName);
	strcat_s(textureFileName, fileName);
}

void XMaterial::LoadImageTex()
{
	Texture* tex = new Texture();
	if (!tex->Load(textureFileName))
	{
		cout << "XLoader Error : failed to load texture file: " << textureFileName << endl;
		imageTex = -1;
	}
	else
	{
		tex->CreateTexture((GLuint*)&imageTex);
	}
	delete tex;
}

////////////////////////////////////////////////////////////////////////
// XMesh class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XMesh()
// Desc : �R���X�g���N�^
//---------------------------------------------------------------------------------------------------
XMesh::XMesh()
{
	strcpy_s(name, "null");
	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
	vertex = (XVector3*)malloc(1 * sizeof(XVector3));
	normal = (XVector3*)malloc(1 * sizeof(XVector3));
	texcoord = (XVector2*)malloc(1 * sizeof(XVector2));
	face = (XFace*)malloc(1 * sizeof(XFace));
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMesh& XMesh::operator= (XMesh &ob)
{
	strcpy_s(name, ob.name);
	numVertices = ob.numVertices;
	numNormals = ob.numNormals;
	numTexCoords = ob.numTexCoords;
	numFaces = ob.numFaces;

	vertex = (XVector3*)malloc(numVertices * sizeof(XVector3));
	normal = (XVector3*)malloc(numNormals * sizeof(XVector3));
	texcoord = (XVector2*)malloc(numTexCoords * sizeof(XVector2));
	face = (XFace*)malloc(numFaces * sizeof(XFace));

	for (int i = 0; i<numVertices; i++)		vertex[i] = ob.vertex[i];
	for (int i = 0; i<numNormals; i++)		normal[i] = ob.normal[i];
	for (int i = 0; i<numTexCoords; i++)		texcoord[i] = ob.texcoord[i];
	for (int i = 0; i<numFaces; i++)		face[i] = ob.face[i];

	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : AddVertex()
// Desc : ���_��ǉ�����
//--------------------------------------------------------------------------------------------------
int XMesh::AddVertex(XVector3 &ob)
{
	numVertices++;
	vertex = (XVector3*)realloc(vertex, numVertices * sizeof(XVector3));
	vertex[numVertices - 1] = ob;
	return numVertices;
}

//--------------------------------------------------------------------------------------------------
// Name : AddNormal()
// Desc : �@����ǉ�����
//--------------------------------------------------------------------------------------------------
int XMesh::AddNormal(XVector3 &ob)
{
	numNormals++;
	normal = (XVector3*)realloc(normal, numNormals * sizeof(XVector3));
	normal[numNormals - 1] = ob;
	return numNormals;
}

//--------------------------------------------------------------------------------------------------
// Name : AddTexCoord()
// Desc : �e�N�X�`�����W��ǉ�����
//--------------------------------------------------------------------------------------------------
int XMesh::AddTexCoord(XVector2 &ob)
{
	numTexCoords++;
	texcoord = (XVector2*)realloc(texcoord, numTexCoords * sizeof(XVector2));
	texcoord[numTexCoords - 1] = ob;
	return numTexCoords;
}

//--------------------------------------------------------------------------------------------------
// Name : AddFace()
// Desc : �ʂ�ǉ�����
//--------------------------------------------------------------------------------------------------
int XMesh::AddFace(XFace &ob)
{
	numFaces++;
	face = (XFace*)realloc(face, numFaces * sizeof(XFace));
	face[numFaces - 1] = ob;
	return numFaces;
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : �m�ۂ������������������
//--------------------------------------------------------------------------------------------------
void XMesh::Release()
{
	if (vertex)
	{
		free((XVector3*)vertex);
		vertex = NULL;
	}

	if (normal)
	{
		free((XVector3*)normal);
		normal = NULL;
	}

	if (texcoord)
	{
		free((XVector2*)texcoord);
		texcoord = NULL;
	}

	if (face)
	{
		free((XFace*)face);
		face = NULL;
	}

	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
}

//---------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : ���O���Z�b�g����
//---------------------------------------------------------------------------------------------------
void XMesh::SetName(char *str)
{
	strcpy_s(name, str);
}

////////////////////////////////////////////////////////////////////////
// XModel class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XModel()
// Desc : �R���X�g���N�^
//---------------------------------------------------------------------------------------------------
XModel::XModel()
{
	numMeshes = 0;
	numMaterials = 0;
	mesh = (XMesh*)malloc(1 * sizeof(XMesh));
}

//--------------------------------------------------------------------------------------------------
// Name : AddMesh()
// Desc : ���b�V����ǉ�����
//--------------------------------------------------------------------------------------------------
int XModel::AddMesh(XMesh ob)
{
	numMeshes++;
	mesh = (XMesh*)realloc(mesh, numMeshes * sizeof(XMesh));
	mesh[numMeshes - 1] = ob;
	return numMeshes;
}

//--------------------------------------------------------------------------------------------------
// Name : AddMaterial()
// Desc : �}�e���A����ǉ�����
//--------------------------------------------------------------------------------------------------
int XModel::AddMaterial(XMaterial ob)
{
	numMaterials++;
	ob.index = numMaterials - 1;
	material.push_back(ob);
	return numMaterials;
}

void XModel::ChangeMaterialIndex(XMaterial ob, int index)
{
	list<XMaterial>::iterator it = material.begin();
	while (it != material.end())  // list�̖����܂�
	{
		if (strcmp(it->textureFileName, ob.textureFileName) == 0) {
			it->index = index;
			break;
		}
		++it;  // �C�e���[�^���P�i�߂�
	}
}

void XModel::DeleteMaterial(XMaterial ob)
{
	numMaterials--;
	list<XMaterial>::iterator it = material.begin();
	while (it != material.end())  // list�̖����܂�
	{
		if (strcmp(it->textureFileName, ob.textureFileName) == 0) {
			material.erase(it);
			break;
		}
		++it;  // �C�e���[�^���P�i�߂�
	}
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : �m�ۂ������������������
//--------------------------------------------------------------------------------------------------
void XModel::Release()
{
	material.clear();
	if (mesh)
	{
		free((XMesh*)mesh);
		mesh = NULL;
	}
	if (matArr)
	{
		delete[] matArr;
		matArr = NULL;
	}
	if (matList)
	{
		delete[] matList;
		matList = NULL;
	}
	if (vbo)
	{
		glDeleteBuffers(numMeshes * 4, vbo);
		delete[] vbo;
		vbo = NULL;
	}
	numMaterials = 0;
	numMeshes = 0;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingSphere()
// Desc : �o�E���f�B���O�X�t�B�A���v�Z����
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingSphere()
{
	int count = 0;
	float tempRadius = 0.0f;
	XVector3 tempCenter;
	tempCenter.x = 0.0f;
	tempCenter.y = 0.0f;
	tempCenter.z = 0.0f;

	//�@���S���W�̎Z�o
	for (int i = 0; i<numMeshes; i++)
	{
		for (int j = 0; j<mesh[i].numVertices; j++)
		{
			tempCenter.x += mesh[i].vertex[j].x;
			tempCenter.y += mesh[i].vertex[j].y;
			tempCenter.z += mesh[i].vertex[j].z;
			count++;
		}
	}
	tempCenter.x /= (float)count;
	tempCenter.y /= (float)count;
	tempCenter.z /= (float)count;

	//�@���a�̎Z�o
	for (int i = 0; i<numMeshes; i++)
	{
		for (int j = 0; j<mesh[i].numVertices; j++)
		{
			float d = 0.0f;
			XVector3 temp;
			temp.x = mesh[i].vertex[j].x - tempCenter.x;
			temp.y = mesh[i].vertex[j].y - tempCenter.y;
			temp.z = mesh[i].vertex[j].z - tempCenter.z;
			d = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
			tempRadius = (tempRadius < d) ? d : tempRadius;
		}
	}

	//�@���ʂ��i�[
	sphere.center = tempCenter;
	sphere.radius = tempRadius;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingBox()
// Desc : �o�E���f�B���O�{�b�N�X���v�Z����
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingBox()
{
	XVector3 tempMin, tempMax;

	//�@�ŏ��̒��_�ŏ�����
	tempMin = tempMax = mesh[0].vertex[0];

	for (int i = 0; i<numMeshes; i++)
	{
		for (int j = 0; j<mesh[i].numVertices; j++)
		{
			//�@x����
			if (tempMin.x > mesh[i].vertex[j].x) tempMin.x = mesh[i].vertex[j].x;
			if (tempMax.x < mesh[i].vertex[j].x) tempMax.x = mesh[i].vertex[j].x;

			//�@y����
			if (tempMin.y > mesh[i].vertex[j].y) tempMin.y = mesh[i].vertex[j].y;
			if (tempMax.y < mesh[i].vertex[j].y) tempMax.y = mesh[i].vertex[j].y;

			//�@z����
			if (tempMin.z > mesh[i].vertex[j].z) tempMin.z = mesh[i].vertex[j].z;
			if (tempMax.z < mesh[i].vertex[j].z) tempMax.z = mesh[i].vertex[j].z;
		}
	}

	//�@���ʂ��i�[
	box.min = tempMin;
	box.max = tempMax;
}

//--------------------------------------------------------------------------------------------------
// Name : Load()
// Desc : �e�L�X�g�t�H�[�}�b�g��X�t�@�C�������[�h����
//--------------------------------------------------------------------------------------------------
bool XModel::Load(char *filename, float scale)
{
	//�@�J�E���g�p�ϐ��Ȃ�	
	bool b_Mesh = false;
	int matCount = 0;
	int vertCount = 0;
	int faceCount = 0;
	int normCount = 0;
	int uvCount = 0;
	int meshCount = 0;

	XFace* tempFace = NULL;
	XMesh tempMesh;

	// get relative directory path
	char* dirName;
	char buff[1024];
	strcpy_s(buff, filename);
	char* p = strrchr(buff, '\\');
	if (p)
	{
		while (strrchr(p + 1, '\\'))
			p = strrchr(buff, '\\');
		int dirNameSize = p - buff + 1;
		dirName = new char[dirNameSize + 1];
		strncpy_s(dirName, dirNameSize + 1, buff, dirNameSize);
		dirName[dirNameSize] = '\0';
	}
	else {
		dirName = new char[1];
		*dirName = '\0';
	}

	//�@�t�@�C����ǂݍ���
	FILE *fp;
	//�@�`�F�b�N
	if (fopen_s(&fp, filename, "rb"))
	{
		cout << "XLoader Error : failed to open file: " << filename << endl;
		return false;
	}

	//�@�t�@�C���T�C�Y���擾
	struct stat stat_buf;
	fstat(_fileno(fp), &stat_buf);
	int fileSize = stat_buf.st_size;

	char* buffer = new char[fileSize + 1];
	buffer[fileSize] = '\0';

	//�@�o�b�t�@�Ɋi�[
	int read_size = fread_s(buffer, fileSize, fileSize, 1, fp);
	fclose(fp);

	//�@�T�C�Y�`�F�b�N
	if (read_size != 1)
	{
		cout << "Error : �ǂݍ��݃T�C�Y�ƃT�C�Y����v���Ă��܂���\n";
		return false;
	}

	CommentDelete(buffer);

	//�@Pointer�ɓǂݍ��񂾃o�b�t�@���Z�b�g
	Pointer = buffer;

	matCount = 0;

	//�@���[�v
	while (*Pointer != '\0')
	{
		//�@�g�[�N�����擾
		GetToken();

		//�@template�̏ꍇ
		if (CheckToken("template"))
		{
			//�@�X�L�b�v����
			SkipNode();
		}

		//�@Mesh�̏ꍇ
		else if (CheckToken("Mesh"))
		{
			//�@�g�[�N�����擾
			GetToken();

			//�@�g�[�N����" { " �łȂ��ꍇ
			if (!CheckToken("{"))
			{
				//�@���O���Z�b�g
				tempMesh.SetName(Token);

				//�@�g�[�N�����擾
				GetToken("{");
			}

			//�@�g�[�N����" { "�̏ꍇ
			else
			{
				//�@���O�����߂�
				char tempName[MAX_PATH] = "obj";
				sprintf_s(tempName, "%s%d", tempName, meshCount + 1);

				//�@���O���Z�b�g
				tempMesh.SetName(tempName);
			}

			//�@���b�V�������J�E���g����
			meshCount++;

			//�@�J�E���g����1���傫���ꍇ
			if (meshCount > 1)
			{
				//�@�ʃf�[�^��ǉ�
				for (int i = 0; i<faceCount; i++)
					tempMesh.AddFace(tempFace[i]);

				//�@���b�V����ǉ�
				AddMesh(tempMesh);

				//�@�m�ۂ��������������
				if (tempFace)
				{
					delete[] tempFace;
					tempFace = NULL;
				}

				//�@�[���ɖ߂�
				ZeroMemory(&tempMesh, sizeof(tempMesh));
			}

			//�@���_���̃J�E���^�[��0�ɖ߂�
			vertCount = 0;

			//�@�g�[�N�����璸�_�����擾
			vertCount = GetIntToken();

			for (int i = 0; i<vertCount; i++)
			{
				//�@�g�[�N�����璸�_�f�[�^���擾
				XVector3 tempVertex;
				tempVertex.x = GetFloatToken();
				tempVertex.y = GetFloatToken();
				tempVertex.z = GetFloatToken();

				//�@���_�f�[�^��ǉ�
				tempMesh.AddVertex(tempVertex);
			}

			//�@���_�����`�F�b�N
			if (tempMesh.numVertices != vertCount)
			{
				cout << "Error : ���_������v���Ă��܂���\n";
				return false;
			}

			//�@�ʐ��̃J�E���^�[��0�ɖ߂�
			faceCount = 0;

			//�@�g�[�N������ʐ����擾
			faceCount = GetIntToken();

			//�@���������m��
			tempFace = new XFace[faceCount];

			//�@�g�[�N������ʃf�[�^���擾
			for (int i = 0; i<faceCount; i++)
			{
				int tempIndex[4] = { -1, -1, -1, -1 };
				tempFace[i].element = 0;
				tempFace[i].indexMaterial = -1;

				//�@�g�[�N�����擾
				GetToken();

				//�@�O�p�`�̏ꍇ
				if (CheckToken("3"))
				{
					//�@�v�f����3�ɂ���
					tempFace[i].element = 3;

					//�@�g�[�N�����璸�_�C���f�b�N�X���擾
					for (int j = 0; j<3; j++)
					{
						tempIndex[j] = GetIntToken();
					}

					//�@4�Ԗڂ̃C���f�b�N�X�ɂ�-1���i�[
					tempIndex[3] = -1;

					//�@���_�C���f�b�N�X���Z�b�g
					tempFace[i].SetVertexIndex(tempIndex);

					//�@�e�N�X�`�����W�C���f�b�N�X���Z�b�g
					tempFace[i].SetTexCoordIndex(tempIndex);
				}

				//�@�l�p�`�̏ꍇ
				else if (CheckToken("4"))
				{
					//�@�v�f����4�ɂ���
					tempFace[i].element = 4;

					//�@�g�[�N�����璸�_�C���f�b�N�X���擾
					for (int j = 0; j<4; j++)
					{
						tempIndex[j] = GetIntToken();
					}

					//�@���_�C���f�b�N�X���Z�b�g
					tempFace[i].SetVertexIndex(tempIndex);

					//�@�e�N�X�`�����W�C���f�b�N�X���Z�b�g
					tempFace[i].SetTexCoordIndex(tempIndex);
				}
			}
		}

		//�@MeshNormals�̏ꍇ
		else if (CheckToken("MeshNormals"))
		{
			//�@�g�[�N�����擾
			GetToken("{");

			//�@�@�����̃J�E���^�[��0�ɖ߂�
			normCount = 0;

			//�@�g�[�N������@�������擾
			normCount = GetIntToken();

			//�@�g�[�N������@���f�[�^���擾
			for (int i = 0; i<normCount; i++)
			{
				XVector3 tempNormal;
				tempNormal.x = GetFloatToken();
				tempNormal.y = GetFloatToken();
				tempNormal.z = GetFloatToken();

				//�@�@���f�[�^��ǉ�
				tempMesh.AddNormal(tempNormal);
			}

			//�@�@�������`�F�b�N
			if (tempMesh.numNormals != normCount)
			{
				cout << "Error : �@��������v���Ă��܂���\n";
				return false;
			}

			//�@�@���C���f�b�N�X�����`�F�b�N
			if (GetIntToken() != faceCount)
			{
				cout << "Error : �ʐ��Ɩ@���C���f�b�N�X������v���Ă��܂���\n";
				return false;
			}

			for (int i = 0; i<faceCount; i++)
			{
				int tempIndex[4] = { -1, -1, -1, -1 };

				//�@�g�[�N�����擾
				GetToken();

				//�@�O�p�`�̏ꍇ
				if (CheckToken("3"))
				{
					//�@�g�[�N������@���C���f�b�N�X���擾
					for (int j = 0; j<3; j++)
					{
						tempIndex[j] = GetIntToken();
					}

					//�@4�Ԗڂ̃C���f�b�N�X�ɂ�-1���Z�b�g
					tempIndex[3] = -1;

					//�@�@���C���f�b�N�X���Z�b�g
					tempFace[i].SetNormalIndex(tempIndex);
				}

				//�@�l�p�`�̏ꍇ
				else if (CheckToken("4"))
				{
					//�@�@���C���f�b�N�X���擾
					for (int j = 0; j<4; j++)
					{
						tempIndex[j] = GetIntToken();
					}

					//�@�@���C���f�b�N�X���Z�b�g
					tempFace[i].SetNormalIndex(tempIndex);
				}
			}
		}

		//�@MeshTextureCoords�̏ꍇ
		else if (CheckToken("MeshTextureCoords"))
		{
			//�@�g�[�N�����擾
			GetToken("{");

			//�@�e�N�X�`�����W���̃J�E���^�[��0�ɖ߂�
			uvCount = 0;

			//�@�g�[�N������e�N�X�`�����W�����擾
			uvCount = GetIntToken();

			for (int i = 0; i<uvCount; i++)
			{
				//�@�g�[�N������e�N�X�`�����W�f�[�^���擾
				XVector2 tempUV;
				tempUV.x = GetFloatToken();
				tempUV.y = GetFloatToken();

				//�@�e�N�X�`�����W�f�[�^��ǉ�
				tempMesh.AddTexCoord(tempUV);
			}
		}

		//�@MeshMaterialList�̏ꍇ
		else if (CheckToken("MeshMaterialList"))
		{
			//�@�g�[�N�����擾
			GetToken("{");

			//�@�}�e���A�����̃J�E���^�[��0�ɖ߂�
			matCount = 0;

			//�@�g�[�N������}�e���A�������擾
			matCount = GetIntToken();

			//�@�}�e���A�������`�F�b�N
			if (GetIntToken() != faceCount)
			{
				cout << "Error : �ʐ��ƃ}�e���A�����X�g������v���܂���\n";
				return false;
			}

			//�@�g�[�N������}�e���A���C���f�b�N�X���擾
			for (int i = 0; i<faceCount; i++)
			{
				tempFace[i].indexMaterial = GetIntToken();
			}

			for (int i = 0; i<matCount; i++)
			{
				XMaterial tempMaterial;

				//�@�g�[�N�����擾
				GetToken();
				if (CheckToken("{"))
				{
					GetToken();

					list<XMaterial>::iterator it = material.begin();
					while (it != material.end())  // list�̖����܂�
					{
						if (strcmp(it->textureFileName, Token) == 0) {
							break;
						}
						++it;  // �C�e���[�^���P�i�߂�
					}
					ChangeMaterialIndex(*it, i);

					GetToken("}");
				}
				else {
					GetToken();

					//�@�g�[�N����" { "�łȂ��ꍇ
					if (!CheckToken("{"))
					{
						//�@���O���Z�b�g
						tempMaterial.SetName(Token);

						//�@�g�[�N�����擾
						GetToken("{");
					}

					//�@�g�[�N����" { "�̏ꍇ
					else
					{
						//�@���O�����߂�
						char tempName[MAX_PATH] = "mat";
						sprintf_s(tempName, "%s%d", tempName, i + 1);

						//�@���O���Z�b�g
						tempMaterial.SetName(tempName);
					}

					//�@Ambient Color
					tempMaterial.ambient.r = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.a = 1.0f;

					//�@Diffuse Color
					tempMaterial.diffuse.r = GetFloatToken();
					tempMaterial.diffuse.g = GetFloatToken();
					tempMaterial.diffuse.b = GetFloatToken();
					tempMaterial.diffuse.a = GetFloatToken();

					//�@Shiness
					tempMaterial.power = GetFloatToken();

					//�@Specular Color
					tempMaterial.specular.r = GetFloatToken();
					tempMaterial.specular.g = GetFloatToken();
					tempMaterial.specular.b = GetFloatToken();
					tempMaterial.specular.a = 1.0f;

					//�@Emissive Color
					tempMaterial.emissive.r = GetFloatToken();
					tempMaterial.emissive.g = GetFloatToken();
					tempMaterial.emissive.b = GetFloatToken();
					tempMaterial.emissive.a = 1.0f;

					//�@�g�[�N�����擾
					GetToken();

					//�@TextureFileName�̏ꍇ
					if (CheckToken("TextureFilename"))
					{
						//�@�g�[�N�����擾
						GetToken("{");

						//�@�g�[�N�����擾
						GetToken('\"');

						//�@�t�@�C�������Z�b�g
						tempMaterial.SetTextureFileName(Token, dirName);

						// �e�N�X�`����Ǎ���
						tempMaterial.LoadImageTex();

						//�@�g�[�N�����擾
						GetToken("}");

						//�@�g�[�N�����擾
						GetToken();
					}

					//�@�g�[�N����" } "�̏ꍇ
					if (CheckToken("}"))
					{
						//�@�}�e���A���f�[�^��ǉ�
						AddMaterial(tempMaterial);
					}
				}
			}
		}
		else if (CheckToken("Material"))
		{
			XMaterial tempMaterial;

			//�@�g�[�N�����擾
			GetToken();

			//�@�g�[�N����" { "�łȂ��ꍇ
			if (!CheckToken("{"))
			{
				//�@���O���Z�b�g
				tempMaterial.SetName(Token);

				//�@�g�[�N�����擾
				GetToken("{");
			}

			//�@�g�[�N����" { "�̏ꍇ
			else
			{
				//�@���O�����߂�
				char tempName[MAX_PATH] = "mat";
				sprintf_s(tempName, "%s%d", tempName, matCount + 1);

				//�@���O���Z�b�g
				tempMaterial.SetName(tempName);
			}

			//�@Ambient Color
			tempMaterial.ambient.r = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.a = 1.0f;

			//�@Diffuse Color
			tempMaterial.diffuse.r = GetFloatToken();
			tempMaterial.diffuse.g = GetFloatToken();
			tempMaterial.diffuse.b = GetFloatToken();
			tempMaterial.diffuse.a = GetFloatToken();

			//�@Shiness
			tempMaterial.power = GetFloatToken();

			//�@Specular Color
			tempMaterial.specular.r = GetFloatToken();
			tempMaterial.specular.g = GetFloatToken();
			tempMaterial.specular.b = GetFloatToken();
			tempMaterial.specular.a = 1.0f;

			//�@Emissive Color
			tempMaterial.emissive.r = GetFloatToken();
			tempMaterial.emissive.g = GetFloatToken();
			tempMaterial.emissive.b = GetFloatToken();
			tempMaterial.emissive.a = 1.0f;

			//�@�g�[�N�����擾
			GetToken();

			//�@TextureFileName�̏ꍇ
			if (CheckToken("TextureFilename"))
			{
				//�@�g�[�N�����擾
				GetToken("{");

				//�@�g�[�N�����擾
				GetToken('\"');

				//�@�t�@�C�������Z�b�g
				tempMaterial.SetTextureFileName(Token, dirName);

				// �e�N�X�`����Ǎ���
				tempMaterial.LoadImageTex();

				//�@�g�[�N�����擾
				GetToken("}");

				//�@�g�[�N�����擾
				GetToken();
			}

			//�@�g�[�N����" } "�̏ꍇ
			if (CheckToken("}"))
			{
				//�@�}�e���A���f�[�^��ǉ�
				AddMaterial(tempMaterial);

				matCount++;
			}
		}
	}

	//�@���b�V������1�ȏ�̏ꍇ
	if (meshCount >= 1)
	{
		//�@�ʃf�[�^��ǉ�
		for (int i = 0; i<faceCount; i++)
			tempMesh.AddFace(tempFace[i]);

		//�@���b�V���f�[�^��ǉ�
		AddMesh(tempMesh);

		//�@�m�ۂ��������������
		if (tempFace)
		{
			delete[] tempFace;
			tempFace = NULL;
		}
	}
	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}

	//�@�o�E���f�B���O�X�t�B�A���v�Z
	ComputeBoundingSphere();

	//�@�o�E���f�B���O�{�b�N�X���v�Z
	ComputeBoundingBox();

	//  �������疂����
	int i, j, k;
	if (numMaterials)
	{
		matArr = new MaterialStruct[numMaterials];
		for (i = 0; i<numMaterials; i++)
		{
			list<XMaterial>::iterator it = material.begin();
			while (it != material.end())
			{
				if (it->index == i) {
					matArr[i].ambient = it->ambient;
					matArr[i].diffuse = it->diffuse;
					matArr[i].specular = it->specular;
					matArr[i].emissive = it->emissive;
					matArr[i].imageTex = it->imageTex;
					break;
				}
				++it;
			}
		}
	}
	material.clear();

	if (numMeshes && numMaterials)
	{
		matList = new MaterialList*[numMeshes];
		vbo = new GLuint[numMeshes * 4];
	}

	for (i = 0; i<numMeshes; i++)
	{
		if (mesh[i].numVertices < mesh[i].numTexCoords)
		{
			cout << "���Ή�" << endl;
			getchar();
			return false;
		}

		matList[i] = new MaterialList[numMaterials * 2];

		int indexCount = 0;
		for (j = 0; j<mesh[i].numFaces; j++)
			indexCount += mesh[i].face[j].element;

		float* vertexArr;
		float* normalArr;
		float* texcoordArr;
		uint* indexArr;
		vertexArr = new float[mesh[i].numVertices * 3];
		normalArr = new float[mesh[i].numVertices * 3];
		texcoordArr = new float[mesh[i].numVertices * 2];
		indexArr = new uint[indexCount];

		for (j = 0; j<mesh[i].numVertices; j++)
		{
			vertexArr[j * 3 + 0] = mesh[i].vertex[j].x*scale;
			vertexArr[j * 3 + 1] = mesh[i].vertex[j].y*scale;
			vertexArr[j * 3 + 2] = mesh[i].vertex[j].z*scale;
		}
		if (mesh[i].numTexCoords == mesh[i].numVertices)
		{
			for (j = 0; j<mesh[i].numVertices; j++)
			{
				texcoordArr[j * 2 + 0] = mesh[i].texcoord[j].x;
				texcoordArr[j * 2 + 1] = mesh[i].texcoord[j].y;
			}
		}
		else
		{
			for (j = 0; j<mesh[i].numFaces; j++)
				for (k = 0; k<mesh[i].face[j].element; k++)
				{
					texcoordArr[mesh[i].face[j].indexVertices[k] * 2 + 0] = mesh[i].texcoord[mesh[i].face[j].indexTexCoords[k]].x;
					texcoordArr[mesh[i].face[j].indexVertices[k] * 2 + 1] = mesh[i].texcoord[mesh[i].face[j].indexTexCoords[k]].y;
				}
		}
		if (mesh[i].numVertices == mesh[i].numNormals)
		{
			for (j = 0; j<mesh[i].numVertices; j++)
			{
				normalArr[j * 3 + 0] = mesh[i].normal[j].x;
				normalArr[j * 3 + 1] = mesh[i].normal[j].y;
				normalArr[j * 3 + 2] = mesh[i].normal[j].z;
			}
		}
		else
		{
			// ���_�ɍ����悤�ɖ@���f�[�^���쐬
			for (j = 0; j<mesh[i].numFaces; j++)
				for (k = 0; k<mesh[i].face[j].element; k++)
				{
					normalArr[mesh[i].face[j].indexVertices[k] * 3 + 0] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].x;
					normalArr[mesh[i].face[j].indexVertices[k] * 3 + 1] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].y;
					normalArr[mesh[i].face[j].indexVertices[k] * 3 + 2] = mesh[i].normal[mesh[i].face[j].indexNormals[k]].z;
				}
		}

		int index = 0;
		for (j = 0; j<numMaterials; j++)
		{
			int faceNum = 0;
			for (k = 0; k<mesh[i].numFaces; k++)
			{
				if (mesh[i].face[k].element == 3 &&
					mesh[i].face[k].indexMaterial == j)
				{
					indexArr[index + 0] = mesh[i].face[k].indexVertices[0];
					indexArr[index + 1] = mesh[i].face[k].indexVertices[1];
					indexArr[index + 2] = mesh[i].face[k].indexVertices[2];
					index += 3;
					faceNum++;
				}
			}
			matList[i][j * 2].elementNum = 3;
			matList[i][j * 2].faceNum = faceNum;
			matList[i][j * 2].materialIndex = j;

			faceNum = 0;
			for (k = 0; k<mesh[i].numFaces; k++)
			{
				if (mesh[i].face[k].element == 4 &&
					mesh[i].face[k].indexMaterial == j)
				{
					indexArr[index + 0] = mesh[i].face[k].indexVertices[0];
					indexArr[index + 1] = mesh[i].face[k].indexVertices[1];
					indexArr[index + 2] = mesh[i].face[k].indexVertices[2];
					indexArr[index + 3] = mesh[i].face[k].indexVertices[3];
					index += 4;
					faceNum++;
				}
			}
			matList[i][j * 2 + 1].elementNum = 4;
			matList[i][j * 2 + 1].faceNum = faceNum;
			matList[i][j * 2 + 1].materialIndex = j;
		}

		int bufferSize = 0;

		glGenBuffers(4, &vbo[i * 4 + 0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i * 4 + 0]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices * 3 * sizeof(float), vertexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (mesh[i].numVertices * 3 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(1, &vbo[i * 4 + 0]);
			cout << "Model Error : VBO�̍쐬�Ɏ��s���܂����B" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i * 4 + 1]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices * 3 * sizeof(float), normalArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (mesh[i].numVertices * 3 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(2, &vbo[i * 4 + 0]);
			cout << "Model Error : VBO�̍쐬�Ɏ��s���܂����B" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i * 4 + 2]);
		glBufferData(GL_ARRAY_BUFFER, mesh[i].numVertices * 2 * sizeof(float), texcoordArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (mesh[i].numVertices * 2 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(3, &vbo[i * 4 + 0]);
			cout << "Model Error : VBO�̍쐬�Ɏ��s���܂����B" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[i * 4 + 3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (indexCount * sizeof(uint) != bufferSize)
		{
			glDeleteBuffers(4, &vbo[i * 4 + 0]);
			cout << "Model Error : VBO�̍쐬�Ɏ��s���܂����B" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] vertexArr;
		delete[] normalArr;
		delete[] texcoordArr;
		delete[] indexArr;
	}
	if (mesh)
	{
		delete[] mesh;
		mesh = NULL;
	}
	delete[] dirName;

	return true;
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : ���b�V���̃C���f�b�N�X���w�肵�ĕ`��
//---------------------------------------------------------------------------------------------------
void XModel::RenderMesh(int index)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[index * 4 + 0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[index * 4 + 1]);
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[index * 4 + 2]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[index * 4 + 3]);

	int i;
	int indexCount = 0;
	for (i = 0; i<numMaterials * 2; i++)
	{
		int matIndex = matList[index][i].materialIndex;
		if (matArr[matIndex].imageTex != -1)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, matArr[matIndex].imageTex);
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matArr[matIndex].ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matArr[matIndex].diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matArr[matIndex].specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matArr[matIndex].emissive);

		if (matList[index][i].elementNum == 3)
			glDrawElements(GL_TRIANGLES, matList[index][i].faceNum * matList[index][i].elementNum,
				GL_UNSIGNED_INT, BUFFER_OFFSET(indexCount * sizeof(uint)));
		else
			glDrawElements(GL_QUADS, matList[index][i].faceNum * matList[index][i].elementNum,
				GL_UNSIGNED_INT, BUFFER_OFFSET(indexCount * sizeof(uint)));
		indexCount += matList[index][i].faceNum * matList[index][i].elementNum;

		if (matArr[matIndex].imageTex != -1)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : �`�揈��
//---------------------------------------------------------------------------------------------------
void XModel::Render()
{
	for (int i = 0; i<numMeshes; i++)
	{
		RenderMesh(i);
	}
}