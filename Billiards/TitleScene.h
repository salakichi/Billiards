#pragma once

#include "Scene.h"
#include "Mouse.h"

// �^�C�g���p�V�[��

#define M_PI 3.1415926535f

class TitleScene : public Scene
{
private:
public:
	TitleScene(ResourceManager& resource, glm::uvec2 &size);
	~TitleScene();
	void SetCamera();
	void Render2D();
	void Render3D();
	void Update();
	void Keyboard(KEY key);
	void KeyboardUp(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void Finish();
};