#include "stdafx.h"
#include "XLoader.h"

//
// global Variables
//
static char* Pointer;
static char Token[1024];


//-------------------------------------------------------------------------------------------------
// Name : CheckToken()
// Desc : 指定された文字列とトークンをチェックする
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
// Desc : トークンを取得する
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
// Desc : トークンを取得し指定された文字列を比較を行う
//--------------------------------------------------------------------------------------------------
static bool GetToken(const char* token)
{
	GetToken();
	if (strcmp(Token, token) != 0)
	{
		cout << "Error : 想定トークンと読込トークンが一致しません\n";
		cout << "想定トークン：" << token << endl;
		cout << "読込トークン：" << Token << endl;
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------
// Name : GetFloatToken()
// Desc : トークンをfloat型に変換し取得する
//--------------------------------------------------------------------------------------------------
static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

//--------------------------------------------------------------------------------------------------
// Name : GetIntToken()
// Desc : トークンをint型に変換し取得する
//--------------------------------------------------------------------------------------------------
static int GetIntToken()
{
	GetToken();
	return (int)atoi(Token);
}

//-------------------------------------------------------------------------------------------------
// Name : SkipNode()
// Desc : ノードを飛ばす
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
		cout << "Error : カッコが一致していません\n";
		return;
	}
}

////////////////////////////////////////////////////////////////////////
// XFace class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Name : XFace()
// Desc : コンストラクタ
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

////////////////////////////////////////////////////////////////////////
// XMaterial class
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Name : XMaterial()
// Desc : コンストラクタ
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
// Desc : 名前をセットする
//--------------------------------------------------------------------------------------------------
void XMaterial::SetName(char *str)
{
	strcpy_s(name, str);
}

//--------------------------------------------------------------------------------------------------
// Name : SetTextureFileName()
// Desc : テクスチャファイル名をセットする
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
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XMesh::XMesh()
{
	strcpy_s(name, "null");
	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
	vertex = nullptr;
	normal = nullptr;
	texcoord = nullptr;
	face = nullptr;
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XMesh::Release()
{
	if (vertex)
	{
		free((glm::vec3*)vertex);
		vertex = nullptr;
	}

	if (normal)
	{
		free((glm::vec3*)normal);
		normal = nullptr;
	}

	if (texcoord)
	{
		free((glm::vec2*)texcoord);
		texcoord = nullptr;
	}

	if (face)
	{
		free((XFace*)face);
		face = nullptr;
	}

	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
}

//---------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : 名前をセットする
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
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XModel::XModel()
{
	numMeshes = 0;
	numMaterials = 0;
	position = glm::vec3(0.f);
	rotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0, 0.0, 0.0));
	box.min = glm::vec3(0.f);
	box.max = glm::vec3(0.f);
	rotationFlag = false;
	matArr = nullptr;
	matList = nullptr;
	vbo = nullptr;
}

//--------------------------------------------------------------------------------------------------
// Name : AddMaterial()
// Desc : マテリアルを追加する
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
	while (it != material.end())  // listの末尾まで
	{
		if (strcmp(it->textureFileName, ob.textureFileName) == 0) {
			it->index = index;
			break;
		}
		++it;  // イテレータを１つ進める
	}
}

void XModel::DeleteMaterial(XMaterial ob)
{
	numMaterials--;
	list<XMaterial>::iterator it = material.begin();
	while (it != material.end())  // listの末尾まで
	{
		if (strcmp(it->textureFileName, ob.textureFileName) == 0) {
			material.erase(it);
			break;
		}
		++it;  // イテレータを１つ進める
	}
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XModel::Release()
{
	material.clear();
	if (matArr)
	{
		delete[] matArr;
		matArr = nullptr;
	}
	if (matList)
	{
		delete[] matList;
		matList = nullptr;
	}
	if (vbo)
	{
		glDeleteBuffers(numMeshes * 4, vbo);
		delete[] vbo;
		vbo = nullptr;
	}
	numMaterials = 0;
	numMeshes = 0;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingSphere()
// Desc : バウンディングスフィアを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingSphere(vector<XMesh*> &meshList, float scale)
{
	int count = 0;
	float tempRadius = 0.0f;
	glm::vec3 tempCenter;
	tempCenter.x = 0.0f;
	tempCenter.y = 0.0f;
	tempCenter.z = 0.0f;

	//　中心座標の算出
	for (int i = 0; i<numMeshes; i++)
	{
		for (int j = 0; j<meshList[i]->numVertices; j++)
		{
			tempCenter.x += meshList[i]->vertex[j].x;
			tempCenter.y += meshList[i]->vertex[j].y;
			tempCenter.z += meshList[i]->vertex[j].z;
			count++;
		}
	}
	tempCenter.x /= (float)count;
	tempCenter.y /= (float)count;
	tempCenter.z /= (float)count;

	//　半径の算出
	for (int i = 0; i<numMeshes; i++)
	{
		for (int j = 0; j<meshList[i]->numVertices; j++)
		{
			float d = 0.0f;
			glm::vec3 temp;
			temp.x = meshList[i]->vertex[j].x - tempCenter.x;
			temp.y = meshList[i]->vertex[j].y - tempCenter.y;
			temp.z = meshList[i]->vertex[j].z - tempCenter.z;
			d = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
			tempRadius = (tempRadius < d) ? d : tempRadius;
		}
	}

	//　結果を格納
	sphere.center = tempCenter * scale;
	sphere.radius = tempRadius * scale;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingBox()
// Desc : バウンディングボックスを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingBox(vector<XMesh*> &meshList, float scale)
{
	glm::vec3 tempMin = glm::vec3();
	glm::vec3 tempMax = glm::vec3();

	if (numMeshes > 0)
	{
		//　最初の頂点で初期化
		tempMin = tempMax = meshList[0]->vertex[0];

		for (int i = 0; i<numMeshes; i++)
		{
			for (int j = 0; j<meshList[i]->numVertices; j++)
			{
				//　x成分
				if (tempMin.x > meshList[i]->vertex[j].x) tempMin.x = meshList[i]->vertex[j].x;
				if (tempMax.x < meshList[i]->vertex[j].x) tempMax.x = meshList[i]->vertex[j].x;

				//　y成分
				if (tempMin.y > meshList[i]->vertex[j].y) tempMin.y = meshList[i]->vertex[j].y;
				if (tempMax.y < meshList[i]->vertex[j].y) tempMax.y = meshList[i]->vertex[j].y;

				//　z成分
				if (tempMin.z > meshList[i]->vertex[j].z) tempMin.z = meshList[i]->vertex[j].z;
				if (tempMax.z < meshList[i]->vertex[j].z) tempMax.z = meshList[i]->vertex[j].z;
			}
		}
	}

	//　結果を格納
	box.min = tempMin * scale;
	box.max = tempMax * scale;
}

//--------------------------------------------------------------------------------------------------
// Name : Load()
// Desc : テキストフォーマットのXファイルをロードする
//--------------------------------------------------------------------------------------------------
bool XModel::Load(char *filename, float scale)
{
	//　カウント用変数など
	int matCount = 0;
	int meshCount = 0;

	XMesh* mesh = nullptr;
	vector<XMesh*> meshList;

	// get relative directory path
	char* dirName;
	char buff[1024];
	strcpy_s(buff, filename);
	char* p = strrchr(buff, '\\');
	if (p)
	{
		while (strrchr(p + 1, '\\'))
			p = strrchr(buff, '\\');
		__int64 dirNameSize = p - buff + 1;
		dirName = new char[dirNameSize + 1];
		strncpy_s(dirName, dirNameSize + 1, buff, dirNameSize);
		dirName[dirNameSize] = '\0';
	}
	else {
		dirName = new char[1];
		*dirName = '\0';
	}

	//　ファイルを読み込む
	FILE *fp;
	//　チェック
	if (fopen_s(&fp, filename, "rb"))
	{
		cout << "XLoader Error : failed to open file: " << filename << endl;
		return false;
	}

	//　ファイルサイズを取得
	struct stat stat_buf;
	fstat(_fileno(fp), &stat_buf);
	int fileSize = stat_buf.st_size;

	char* buffer = new char[fileSize + 1];
	buffer[fileSize] = '\0';

	//　バッファに格納
	size_t read_size = fread_s(buffer, fileSize+1, fileSize, 1, fp);
	fclose(fp);

	//　サイズチェック
	if (read_size != 1)
	{
		cout << "XLoader Error : 読み込みサイズとサイズが一致していません\n";
		return false;
	}

	//　Pointerに読み込んだバッファをセット
	Pointer = buffer;

	matCount = 0;

	//　ループ
	while (*Pointer != '\0')
	{
		//　トークンを取得
		GetToken();

		//　templateの場合
		if (CheckToken("template"))
		{
			//　スキップする
			SkipNode();
		}

		//　Meshの場合
		else if (CheckToken("Mesh"))
		{
			//　カウント数が1より大きい場合
			if (mesh != nullptr)
			{
				//　メッシュを追加
				meshList.push_back(mesh);
				mesh = nullptr;
			}

			mesh = new XMesh();

			//　トークンを取得
			GetToken();

			//　トークンが" { " でない場合
			if (!CheckToken("{"))
			{
				//　名前をセット
				mesh->SetName(Token);

				//　トークンを取得
				GetToken("{");
			}

			//　トークンが" { "の場合
			else
			{
				//　名前を決める
				char tempName[MAX_PATH] = "obj";
				sprintf_s(tempName, "%s%d", tempName, meshCount + 1);

				//　名前をセット
				mesh->SetName(tempName);
			}

			//　トークンから頂点数を取得
			mesh->numVertices = GetIntToken();

			// メモリを確保
			mesh->vertex = new glm::vec3[mesh->numVertices];

			for (int i = 0; i<mesh->numVertices; i++)
			{
				//　トークンから頂点データを取得
				mesh->vertex[i].x = GetFloatToken();
				mesh->vertex[i].y = GetFloatToken();
				mesh->vertex[i].z = GetFloatToken();
			}

			//　トークンから面数を取得
			mesh->numFaces = GetIntToken();

			//　メモリを確保
			mesh->face = new XFace[mesh->numFaces];

			//　トークンから面データを取得
			for (int i = 0; i<mesh->numFaces; i++)
			{
				mesh->face[i].element = 0;
				mesh->face[i].indexMaterial = -1;

				//　トークンを取得
				GetToken();

				//　三角形の場合
				if (CheckToken("3"))
				{
					//　要素数は3にする
					mesh->face[i].element = 3;

					//　トークンから頂点インデックスを取得
					for (int j = 0; j<3; j++)
					{
						int index = GetIntToken();
						mesh->face[i].indexVertices[j] = index;
						mesh->face[i].indexTexCoords[j] = index;
					}

					//　4番目のインデックスには-1を格納
					mesh->face[i].indexVertices[3] = -1;
					mesh->face[i].indexTexCoords[3] = -1;
				}

				//　四角形の場合
				else if (CheckToken("4"))
				{
					//　要素数は4にする
					mesh->face[i].element = 4;

					//　トークンから頂点インデックスを取得
					for (int j = 0; j<4; j++)
					{
						int index = GetIntToken();
						mesh->face[i].indexVertices[j] = index;
						mesh->face[i].indexTexCoords[j] = index;
					}
				}
			}
		}

		//　MeshNormalsの場合
		else if (CheckToken("MeshNormals"))
		{
			//　トークンを取得
			GetToken("{");

			//　トークンから法線数を取得
			mesh->numNormals = GetIntToken();

			mesh->normal = new glm::vec3[mesh->numNormals];

			//　トークンから法線データを取得
			for (int i = 0; i<mesh->numNormals; i++)
			{
				mesh->normal[i].x = GetFloatToken();
				mesh->normal[i].y = GetFloatToken();
				mesh->normal[i].z = GetFloatToken();
			}

			//　法線インデックス数をチェック
			if (GetIntToken() != mesh->numFaces)
			{
				cout << "Error : 面数と法線インデックス数が一致していません\n";
				return false;
			}

			for (int i = 0; i<mesh->numFaces; i++)
			{
				//　トークンを取得
				GetToken();

				//　三角形の場合
				if (CheckToken("3"))
				{
					//　トークンから法線インデックスを取得
					for (int j = 0; j<3; j++)
					{
						mesh->face[i].indexNormals[j] = GetIntToken();
					}

					//　4番目のインデックスには-1をセット
					mesh->face[i].indexNormals[3] = -1;
				}

				//　四角形の場合
				else if (CheckToken("4"))
				{
					//　法線インデックスを取得
					for (int j = 0; j<4; j++)
					{
						mesh->face[i].indexNormals[j] = GetIntToken();
					}
				}
			}
		}

		//　MeshTextureCoordsの場合
		else if (CheckToken("MeshTextureCoords"))
		{
			//　トークンを取得
			GetToken("{");

			//　トークンからテクスチャ座標数を取得
			mesh->numTexCoords = GetIntToken();

			mesh->texcoord = new glm::vec2[mesh->numTexCoords];

			for (int i = 0; i<mesh->numTexCoords; i++)
			{
				mesh->texcoord[i].x = GetFloatToken();
				mesh->texcoord[i].y = GetFloatToken();
			}
		}

		//　MeshMaterialListの場合
		else if (CheckToken("MeshMaterialList"))
		{
			//　トークンを取得
			GetToken("{");

			//　マテリアル数のカウンターを0に戻す
			matCount = 0;

			//　トークンからマテリアル数を取得
			matCount = GetIntToken();

			//　マテリアル数をチェック
			if (GetIntToken() != mesh->numFaces)
			{
				cout << "Error : 面数とマテリアルリスト数が一致しません\n";
				return false;
			}

			//　トークンからマテリアルインデックスを取得
			for (int i = 0; i<mesh->numFaces; i++)
			{
				mesh->face[i].indexMaterial = GetIntToken();
			}

			for (int i = 0; i<matCount; i++)
			{
				XMaterial tempMaterial;

				//　トークンを取得
				GetToken();
				if (CheckToken("{"))
				{
					GetToken();

					list<XMaterial>::iterator it = material.begin();
					while (it != material.end())  // listの末尾まで
					{
						if (strcmp(it->textureFileName, Token) == 0) {
							break;
						}
						++it;  // イテレータを１つ進める
					}
					ChangeMaterialIndex(*it, i);

					GetToken("}");
				}
				else {
					GetToken();

					//　トークンが" { "でない場合
					if (!CheckToken("{"))
					{
						//　名前をセット
						tempMaterial.SetName(Token);

						//　トークンを取得
						GetToken("{");
					}

					//　トークンが" { "の場合
					else
					{
						//　名前を決める
						char tempName[MAX_PATH] = "mat";
						sprintf_s(tempName, "%s%d", tempName, i + 1);

						//　名前をセット
						tempMaterial.SetName(tempName);
					}

					//　Ambient Color
					tempMaterial.ambient.r = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.g = 0.15f;
					tempMaterial.ambient.a = 1.0f;

					//　Diffuse Color
					tempMaterial.diffuse.r = GetFloatToken();
					tempMaterial.diffuse.g = GetFloatToken();
					tempMaterial.diffuse.b = GetFloatToken();
					tempMaterial.diffuse.a = GetFloatToken();

					//　Shiness
					tempMaterial.power = GetFloatToken();

					//　Specular Color
					tempMaterial.specular.r = GetFloatToken();
					tempMaterial.specular.g = GetFloatToken();
					tempMaterial.specular.b = GetFloatToken();
					tempMaterial.specular.a = 1.0f;

					//　Emissive Color
					tempMaterial.emissive.r = GetFloatToken();
					tempMaterial.emissive.g = GetFloatToken();
					tempMaterial.emissive.b = GetFloatToken();
					tempMaterial.emissive.a = 1.0f;

					//　トークンを取得
					GetToken();

					//　TextureFileNameの場合
					if (CheckToken("TextureFilename"))
					{
						//　トークンを取得
						GetToken("{");

						//　トークンを取得
						GetToken('\"');

						//　ファイル名をセット
						tempMaterial.SetTextureFileName(Token, dirName);

						// テクスチャを読込む
						tempMaterial.LoadImageTex();

						//　トークンを取得
						GetToken("}");

						//　トークンを取得
						GetToken();
					}

					//　トークンが" } "の場合
					if (CheckToken("}"))
					{
						//　マテリアルデータを追加
						AddMaterial(tempMaterial);
					}
				}
			}
		}
		else if (CheckToken("Material"))
		{
			XMaterial tempMaterial;

			//　トークンを取得
			GetToken();

			//　トークンが" { "でない場合
			if (!CheckToken("{"))
			{
				//　名前をセット
				tempMaterial.SetName(Token);

				//　トークンを取得
				GetToken("{");
			}

			//　トークンが" { "の場合
			else
			{
				//　名前を決める
				char tempName[MAX_PATH] = "mat";
				sprintf_s(tempName, "%s%d", tempName, matCount + 1);

				//　名前をセット
				tempMaterial.SetName(tempName);
			}

			//　Ambient Color
			tempMaterial.ambient.r = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.g = 0.15f;
			tempMaterial.ambient.a = 1.0f;

			//　Diffuse Color
			tempMaterial.diffuse.r = GetFloatToken();
			tempMaterial.diffuse.g = GetFloatToken();
			tempMaterial.diffuse.b = GetFloatToken();
			tempMaterial.diffuse.a = GetFloatToken();

			//　Shiness
			tempMaterial.power = GetFloatToken();

			//　Specular Color
			tempMaterial.specular.r = GetFloatToken();
			tempMaterial.specular.g = GetFloatToken();
			tempMaterial.specular.b = GetFloatToken();
			tempMaterial.specular.a = 1.0f;

			//　Emissive Color
			tempMaterial.emissive.r = GetFloatToken();
			tempMaterial.emissive.g = GetFloatToken();
			tempMaterial.emissive.b = GetFloatToken();
			tempMaterial.emissive.a = 1.0f;

			//　トークンを取得
			GetToken();

			//　TextureFileNameの場合
			if (CheckToken("TextureFilename"))
			{
				//　トークンを取得
				GetToken("{");

				//　トークンを取得
				GetToken('\"');

				//　ファイル名をセット
				tempMaterial.SetTextureFileName(Token, dirName);

				// テクスチャを読込む
				tempMaterial.LoadImageTex();

				//　トークンを取得
				GetToken("}");

				//　トークンを取得
				GetToken();
			}

			//　トークンが" } "の場合
			if (CheckToken("}"))
			{
				//　マテリアルデータを追加
				AddMaterial(tempMaterial);

				matCount++;
			}
		}
	}

	if (mesh != nullptr)
	{
		//　メッシュデータを追加
		meshList.push_back(mesh);
		mesh = nullptr;
	}
	numMeshes = (int)meshList.size();

	if (buffer)
	{
		delete[] buffer;
		buffer = nullptr;
	}

	//　バウンディングスフィアを計算
	ComputeBoundingSphere(meshList, scale);

	//　バウンディングボックスを計算
	ComputeBoundingBox(meshList, scale);

	//  ここから魔改造
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

	// マテリアル別に並べかえる
	for (i = 0; i<numMeshes; i++)
	{
		if (meshList[i]->numVertices < meshList[i]->numTexCoords)
		{
			cout << "未対応" << endl;
			getchar();
			return false;
		}

		matList[i] = new MaterialList[numMaterials * 2];

		int indexCount = 0;
		for (j = 0; j<meshList[i]->numFaces; j++)
			indexCount += meshList[i]->face[j].element;

		float* vertexArr;
		float* normalArr;
		float* texcoordArr;
		uint* indexArr;
		vertexArr = new float[meshList[i]->numVertices * 3];
		normalArr = new float[meshList[i]->numVertices * 3];
		texcoordArr = new float[meshList[i]->numVertices * 2];
		indexArr = new uint[indexCount];

		for (j = 0; j<meshList[i]->numVertices; j++)
		{
			vertexArr[j * 3 + 0] = meshList[i]->vertex[j].x*scale;
			vertexArr[j * 3 + 1] = meshList[i]->vertex[j].y*scale;
			vertexArr[j * 3 + 2] = meshList[i]->vertex[j].z*scale;
		}
		if (meshList[i]->numTexCoords == meshList[i]->numVertices)
		{
			for (j = 0; j<meshList[i]->numVertices; j++)
			{
				texcoordArr[j * 2 + 0] = meshList[i]->texcoord[j].x;
				texcoordArr[j * 2 + 1] = meshList[i]->texcoord[j].y;
			}
		}
		else
		{
			for (j = 0; j<meshList[i]->numFaces; j++)
				for (k = 0; k<meshList[i]->face[j].element; k++)
				{
					texcoordArr[meshList[i]->face[j].indexVertices[k] * 2 + 0] = meshList[i]->texcoord[meshList[i]->face[j].indexTexCoords[k]].x;
					texcoordArr[meshList[i]->face[j].indexVertices[k] * 2 + 1] = meshList[i]->texcoord[meshList[i]->face[j].indexTexCoords[k]].y;
				}
		}
		if (meshList[i]->numVertices == meshList[i]->numNormals)
		{
			for (j = 0; j<meshList[i]->numVertices; j++)
			{
				normalArr[j * 3 + 0] = meshList[i]->normal[j].x;
				normalArr[j * 3 + 1] = meshList[i]->normal[j].y;
				normalArr[j * 3 + 2] = meshList[i]->normal[j].z;
			}
		}
		else
		{
			// 頂点に合うように法線データを作成
			for (j = 0; j<meshList[i]->numFaces; j++)
				for (k = 0; k<meshList[i]->face[j].element; k++)
				{
					normalArr[meshList[i]->face[j].indexVertices[k] * 3 + 0] = meshList[i]->normal[meshList[i]->face[j].indexNormals[k]].x;
					normalArr[meshList[i]->face[j].indexVertices[k] * 3 + 1] = meshList[i]->normal[meshList[i]->face[j].indexNormals[k]].y;
					normalArr[meshList[i]->face[j].indexVertices[k] * 3 + 2] = meshList[i]->normal[meshList[i]->face[j].indexNormals[k]].z;
				}
		}

		int index = 0;
		for (j = 0; j<numMaterials; j++)
		{
			int faceNum = 0;
			for (k = 0; k<meshList[i]->numFaces; k++)
			{
				if (meshList[i]->face[k].element == 3 &&
					meshList[i]->face[k].indexMaterial == j)
				{
					indexArr[index + 0] = meshList[i]->face[k].indexVertices[0];
					indexArr[index + 1] = meshList[i]->face[k].indexVertices[1];
					indexArr[index + 2] = meshList[i]->face[k].indexVertices[2];
					index += 3;
					faceNum++;
				}
			}
			matList[i][j * 2].elementNum = 3;
			matList[i][j * 2].faceNum = faceNum;
			matList[i][j * 2].materialIndex = j;

			faceNum = 0;
			for (k = 0; k<meshList[i]->numFaces; k++)
			{
				if (meshList[i]->face[k].element == 4 &&
					meshList[i]->face[k].indexMaterial == j)
				{
					indexArr[index + 0] = meshList[i]->face[k].indexVertices[0];
					indexArr[index + 1] = meshList[i]->face[k].indexVertices[1];
					indexArr[index + 2] = meshList[i]->face[k].indexVertices[2];
					indexArr[index + 3] = meshList[i]->face[k].indexVertices[3];
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
		glBufferData(GL_ARRAY_BUFFER, meshList[i]->numVertices * 3 * sizeof(float), vertexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (meshList[i]->numVertices * 3 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(1, &vbo[i * 4 + 0]);
			cout << "Model Error : VBOの作成に失敗しました。" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i * 4 + 1]);
		glBufferData(GL_ARRAY_BUFFER, meshList[i]->numVertices * 3 * sizeof(float), normalArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (meshList[i]->numVertices * 3 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(2, &vbo[i * 4 + 0]);
			cout << "Model Error : VBOの作成に失敗しました。" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i * 4 + 2]);
		glBufferData(GL_ARRAY_BUFFER, meshList[i]->numVertices * 2 * sizeof(float), texcoordArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (meshList[i]->numVertices * 2 * sizeof(float) != bufferSize)
		{
			glDeleteBuffers(3, &vbo[i * 4 + 0]);
			cout << "Model Error : VBOの作成に失敗しました。" << endl;
			Release();
			return false;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[i * 4 + 3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indexArr, GL_STATIC_DRAW);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if (indexCount * sizeof(uint) != bufferSize)
		{
			glDeleteBuffers(4, &vbo[i * 4 + 0]);
			cout << "Model Error : VBOの作成に失敗しました。" << endl;
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
	if (meshList.size() > 0)
	{
		for (auto mesh : meshList)
		{
			delete[] mesh->face;
			delete[] mesh->vertex;
			delete[] mesh->normal;
			delete[] mesh->texcoord;
			delete mesh;
		}
		meshList.clear();
	}
	delete[] dirName;

	return true;
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : メッシュのインデックスを指定して描画
//---------------------------------------------------------------------------------------------------
void XModel::RenderMesh(int index)
{
	glPushMatrix();
	
	if (rotationFlag)
	{
		glm::mat4x4 transRotMat = glm::translate<float>(glm::mat4(1.0f), position) * rotation * glm::translate<float>(glm::mat4(1.0f), glm::vec3(0.f, -sphere.radius, 0.f));
		glMultMatrixf(&transRotMat[0][0]);
	}
	else
	{
		glTranslatef(position.x, position.y, position.z);
	}

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
		if (matList[index][i].faceNum == 0) continue;
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

	glPopMatrix();
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : 描画処理
//---------------------------------------------------------------------------------------------------
void XModel::Render()
{
	for (int i = 0; i<numMeshes; i++)
	{
		RenderMesh(i);
	}
}