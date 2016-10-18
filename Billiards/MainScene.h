#pragma once

#include "Scene.h"
#include "Mouse.h"

// ゲームメイン用シーン

#define M_PI 3.1415926535f

class MainScene : public Scene
{
private:
	// カメラ
	ViewCamera camera;
public:
	MainScene(ResourceManager& resource);
	~MainScene();
	void SetCamera();
	void Render2D();
	void Render3D();
	void Update();
	void KeyFunc(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void Finish();
};