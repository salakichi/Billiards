#include "stdafx.h"
#include "XLoader.h"
#include "Mouse.h"
#include "Ball.h"

int WindowWidth = 800;
int WindowHeight = 800;
XModel tableModel;
Ball balls[16];
XModel skyboxModel;
ViewCamera camera;

const char* tableModelPath = "data\\table.x";
const char* ballModelPath = "data\\%d.x";
const char* skyboxModelPath = "data\\room\\松野家居間_ダンス用.x";

#define M_PI 3.141592f
#define DEPTH_TEX_WIDTH  512
#define DEPTH_TEX_HEIGHT 512
//#define SHADOW_MAP_ON

static const GLfloat lightpos[] = { 0.0f, 5.0f, 0.0f, 1.f }; /* 位置　　　　　　　 */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.f }; /* 直接光強度　　　　 */
static const GLfloat lightdim[] = { 0.2f, 0.2f, 0.2f, 1.f }; /* 影内の拡散反射強度 */
static const GLfloat lightblk[] = { 0.0f, 0.0f, 0.0f, 1.f }; /* 影内の鏡面反射強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.f }; /* 環境光強度　　　　 */

char FpsString[50] = { 0 };
double CurrentTime = 0.0;
double LastTime = 0.0;
double CurrentCount = 0.0;
double LastCount = 0.0;
int FrameCount = 0;
double Fps = 0.0f;

void Initialize();
void Shutdown();
void Display();
void Idle();
void Reshape(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Render3D();
void glutRenderText(void* bitmapfont, char* text);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("ビリヤード");
	if (glewInit() != GLEW_OK)
	{
		cout << "glewの初期化に失敗しました。" << endl;
		return -1;
	}

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);

	Initialize();

	Sleep(2000);

	glutMainLoop();

	Shutdown();

	return 0;
}

double genfunc[][4] = {
	{ 1.0, 0.0, 0.0, 0.0 }, // s
	{ 0.0, 1.0, 0.0, 0.0 }, // t
	{ 0.0, 0.0, 1.0, 0.0 }, // r
	{ 0.0, 0.0, 0.0, 1.0 }, // q
};

void Initialize()
{
	// 通常のテクスチャの設定
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//　カウンターの初期化
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;

	//　モデルファイルの読み込み
	glm::vec3 globalPos = glm::vec3(0.f);
	tableModel.Load((char*)tableModelPath);
	globalPos.y = -tableModel.box.max.y + 0.3f;
	tableModel.position = globalPos;

	float padding = 2.4f;
	glm::vec3 wallMin = glm::vec3(-tableModel.box.max.x + padding, 0.f, -tableModel.box.max.z + padding);
	glm::vec3 wallMax = glm::vec3(tableModel.box.max.x - padding, 0.f, tableModel.box.max.z - padding);

	char ballFilename[256];
	for (int i=0; i<=15; ++i)
	{
		sprintf_s(ballFilename, ballModelPath, i);
		balls[i].model.Load(ballFilename);
		balls[i].SetWall(wallMin, wallMax);
		balls[i].model.EnableRotate();
	}

	balls[0].model.position = glm::vec3(0.f, 0.f, 0.f);
	float r = balls[0].model.sphere.radius*2.f;
	int k = 1;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5 - i; ++j)
		{
			balls[k].model.position = glm::vec3(9.0f - j*r*sin(M_PI / 3), 0.f, j*r*cos(M_PI / 3) + (i-2)*r);
			++k;
		}
	}

	skyboxModel.Load((char*)skyboxModelPath, 20.f);
	skyboxModel.position = globalPos;

#ifdef SHADOW_MAP_ON
	glActiveTexture(GL_TEXTURE1);

	/* テクスチャの割り当て */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/* テクスチャの繰り返し方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	/* もしＲの値がテクスチャの値以下なら真（つまり日向） */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	/* 比較の結果を輝度値として得る */
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	/* テクスチャユニット7のテクスチャ環境 */
	/*
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	static const GLfloat blend[] = { 1.0, 1.0, 1.0, 0.5 };
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blend);
	*/
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* テクスチャ座標に視点座標系における物体の座標値を用いる */
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	/* 生成したテクスチャ座標をそのまま (S, T, R, Q) に使う */
	static const GLdouble genfunc[][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 },
	};
	glTexGendv(GL_S, GL_EYE_PLANE, genfunc[0]);
	glTexGendv(GL_T, GL_EYE_PLANE, genfunc[1]);
	glTexGendv(GL_R, GL_EYE_PLANE, genfunc[2]);
	glTexGendv(GL_Q, GL_EYE_PLANE, genfunc[3]);

	// 通常のテクスチャに戻す
	glActiveTexture(GL_TEXTURE0);
#endif

	//　バックバッファをクリアする色の指定
	//glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
	glClearColor(1.f, 1.f, 1.0f, 1.0f);

	// カリングできるようにする
	glEnable(GL_CULL_FACE);

	//　深度テストON
	glEnable(GL_DEPTH_TEST);

	//　ライティング
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

	//　ウィンドウを表示
	glutShowWindow();
}

void Idle()
{
	glutPostRedisplay();
}

void Reshape(int x, int y)
{
	//　ウィンドウサイズを保存
	WindowWidth = x;
	WindowHeight = y;

	//　サイズチェック
	if (WindowWidth < 1) WindowWidth = 1;
	if (WindowHeight < 1) WindowHeight = 1;

	//　ビューポートの設定
	glViewport(0, 0, WindowWidth, WindowHeight);

	//　射影行列の設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 1000.0);
}

void Render2D()
{
	bool isLighting = false;

	//　時間計測とFPS算出
	CurrentCount = glutGet(GLUT_ELAPSED_TIME);
	CurrentTime = (CurrentCount - LastCount) / 1000.0;
	FrameCount++;
	if ((CurrentTime - LastTime) >= 1.0)
	{
		Fps = FrameCount / (CurrentTime - LastTime);
		sprintf_s(FpsString, "%.3f FPS", Fps);
		FrameCount = 0;
		LastTime = CurrentTime;
	}

	//　3D　→　2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WindowWidth, WindowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (glIsEnabled(GL_LIGHTING))
	{
		isLighting = true;
		glDisable(GL_LIGHTING);
	}

	//　文字の描画
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glRasterPos2i(15, 15);
	glutRenderText(GLUT_BITMAP_HELVETICA_12, FpsString);

	if (!glIsEnabled(GL_LIGHTING))
	{
		if (isLighting)
			glEnable(GL_LIGHTING);
	}

	//　2D　→　3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void Render3D()
{
	tableModel.Render();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Render();
	}
	skyboxModel.Render();
}

void Update()
{
	for (int i = 0; i < 16; ++i)
	{
		balls[i].Move();
		balls[i].TestCollisionWall();
		for (int j = 0; j < 16; ++j)
		{
			if (i != j)
			{
				balls[i].TestCollisionBall(balls[j]);
			}
		}
		balls[i].TestPocket();
		balls[i].UpdateVelocity();
	}
}

void Render()
{
	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//　視点の設定
	camera.Set();

	// 光源の位置を設定する
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	//　3Dシーンの描画
	Render3D();

	glDisable(GL_TEXTURE_2D);

	//　2Dシーンの描画
	Render2D();

	glPopMatrix();
}

void ShadowmapRender()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble currentModelView[16];

	/* デプステクスチャの作成ここから */
	glActiveTexture(GL_TEXTURE1);

	// デプスバッファをクリアする
	glClear(GL_DEPTH_BUFFER_BIT);

	// ビューポートをテクスチャのサイズに設定
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// 透視変換行列を単位行列に設定
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 光源位置を視点としシーンが視野に収まるようモデルビュー変換行列を設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(40.0, (GLdouble)DEPTH_TEX_WIDTH / (GLdouble)DEPTH_TEX_HEIGHT, 1.0, 20.0);
	gluLookAt(lightpos[0], lightpos[1], lightpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	// デプスバッファの内容だけを取得するのでフレームバッファには書き込まない
	// したがって陰影付けも不要なのでライティングをオフにする
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);

	// デプスバッファには背面のポリゴンの奥行きを記録するようにする
	glCullFace(GL_FRONT);

	//　3Dシーンの描画
	Render3D();

	// デプスバッファの内容をテクスチャメモリに転送
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// 通常の描画の設定に戻す
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	/* デプステクスチャの作成ここまで */

	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//　視点の設定
	camera.Set();

	// 光源の位置を設定する
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* デプステクスチャの設定ここから */
	glActiveTexture(GL_TEXTURE1);

	// 現在のモデルビューの取得
	glGetDoublev(GL_MODELVIEW_MATRIX, currentModelView);

	// テクスチャ変換行列を設定する
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// テクスチャ座標の [-1,1] の範囲を [0,1] の範囲に収める
	glTranslated(0.5, 0.5, 0.5);
	glScaled(0.5, 0.5, 0.5);

	// テクスチャのモデルビュー変換行列と透視変換行列の積をかける
	glMultMatrixd(modelview);

	// 現在のモデルビュー変換の逆変換をかけておく
	glMultTransposeMatrixd(currentModelView);

	// モデルビュー変換行列に戻す
	glMatrixMode(GL_MODELVIEW);

	glActiveTexture(GL_TEXTURE0);
	/* デプステクスチャの設定ここまで */

	glPushMatrix();

	/* テクスチャマッピングとテクスチャ座標の自動生成を有効にする */
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glActiveTexture(GL_TEXTURE0);

	// 光源の明るさを日向の部分での明るさに設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);

	//　3Dシーンの描画
	Render3D();

	/* テクスチャマッピングとテクスチャ座標の自動生成を無効にする */
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	//　2Dシーンの描画
	Render2D();

	glPopMatrix();
}

void Display()
{
	Update();

#ifdef SHADOW_MAP_ON
	ShadowmapRender();
#else
	Render();
#endif

	//　ダブルバッファ
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

void Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}

bool onEnter = false;
void Keyboard(unsigned char key, int x, int y)
{
	switch ((int)key)
	{
	case 13:	// Enter
		if (!onEnter)
		{
			balls[0].AddVec(glm::vec3(0.25f, 0.f, 0.f));
			onEnter = true;
		}
		break;
	default:
		cout << (int)key << endl;
		onEnter = false;
		break;
	}
}

void Shutdown()
{
	tableModel.Release();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Release();
	}
	skyboxModel.Release();
}

void glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

