#include "stdafx.h"
#include "XLoader.h"
#include "Mouse.h"

int WindowWidth = 512;
int WindowHeight = 512;
XModel model;
ViewCamera camera;

const char* testModelPath = "data\\table.x";

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
void Render3D();
void glutRenderText(void* bitmapfont, char* text);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Xファイル描画テスト");
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

	Initialize();

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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//　カウンターの初期化
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;

	//　モデルファイルの読み込み
	model.Load((char*)testModelPath);

	//　バックバッファをクリアする色の指定
	glClearColor(0.3f, 0.3f, 1.0f, 1.0f);

	//　深度テストON
	glEnable(GL_DEPTH_TEST);

	//　ライティング
	float pos[4] = { 0.0f, 10.0f, 10.0f, 1.0f };
	float amb[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float dif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spe[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

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
	model.Render();
}

void Display()
{
	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//　視点の設定
	camera.Set();

	glPushMatrix();

	//　3Dシーンの描画
	Render3D();

	//　2Dシーンの描画
	Render2D();

	glPopMatrix();

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

void Shutdown()
{
	model.Release();
}

void glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

