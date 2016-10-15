#pragma once

#include "XLoader.h"
#include "Mouse.h"
#include "Ball.h"
#include "Font.h"
#include <process.h>
#include <mmsystem.h>

#define WINDOW_DEFAULT_W 800
#define WINDOW_DEFAULT_H 800
#define WINDOW_DEFAULT_POS_X 0
#define WINDOW_DEFAULT_POS_Y 0
#define M_PI 3.1415926535f

enum KEY {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_V,
	KEY_SPACE, KEY_ENTER, KEY_BACKSPACE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5
};

enum GAME_STATUS {
	TITLE, MAIN, CONTINUE, GAMECLEAR, GAMEOVER,
};

class GameManager
{
private:
	bool InitializeGL(int argc, char** argv);
	void InitializeModel();
	bool LoadMaterial();
	void ErrorLog(char* msg);
	void DrawLoadScreen();
	void CloseLoadScreen();
	void Render2D();
	void Render3D();
	void RenderWithoutShadow();
	void RenderWithShadow();

	// FPS用
	double CurrentTime;
	double LastTime;
	double CurrentCount;
	double LastCount;
	int FrameCount;
	double Fps;

	// 遷移用
	GAME_STATUS status;

	// モデルデータ
	XModel tableModel;
	Ball balls[16];
	XModel skyboxModel;

	// カメラ
	ViewCamera camera;

	// シャドウ用
	GLuint shadowTexture;

	// フォント
	Font font;

	HANDLE thread;
	HANDLE mutex;
public:
	GameManager();
	~GameManager();
	bool Initialize(int argc, char** argv);
	void Release();
	void Render();
	void KeyFunc(KEY key);
	void Mouse(int button, int state, int x, int y) { camera.MouseInput(button, state, x, y); };
	void Motion(int x, int y) { camera.MouseMotion(x, y); }
	void Update();
	void SetWindowSize(int x, int y);
	uint GetWindowWidth();
	uint GetWindowHeight();
	uint GetWindowPosX();
	uint GetWindowPosY();
	GAME_STATUS GetStatus() { return status; };
};