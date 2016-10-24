#pragma once

#include "Scene.h"
#include "Mouse.h"

// �Q�[�����C���p�V�[��

#define M_PI 3.1415926535f

class MainScene : public Scene
{
private:
	// �J����
	ViewCamera camera;

	// �{�[����ł��āA�~�܂�܂�true
	bool shotFlag;

	// �Ō�Ƀ{�[�����Փ˂�������
	double lastHitTime;
public:
	MainScene(ResourceManager& resource, glm::uvec2 &size);
	~MainScene();
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