#pragma once

#include "Scene.h"
#include "Mouse.h"

// �^�C�g���p�V�[��

#define M_PI 3.1415926535f

class TitleScene : public Scene
{
private:
	// �J����
	ViewCamera camera;
public:
	TitleScene(ResourceManager& resource);
	void SetCamera();
	void Render2D();
	void Render3D();
	void Update();
	void KeyFunc(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
};