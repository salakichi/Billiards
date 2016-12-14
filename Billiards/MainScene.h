#pragma once

#include "Scene.h"
#include "Mouse.h"

// �Q�[�����C���p�V�[��

#define M_PI 3.1415926535f
#define BASE_SCORE 100
#define MAX_POWER 0.5f

enum MAIN_SCENE_STATUS {
	SELECT_ANGLE, SELECT_POWER, SHOT,
};

class MainScene : public Scene
{
private:
	// ���
	MAIN_SCENE_STATUS status;

	// ��Ԃ��؂�ւ��������
	double statusChangedTime;

	// ��ԕύX
	void ChangeStatus(MAIN_SCENE_STATUS next);

	// �J����
	ViewCamera camera;

	// �łp�x(rad)
	float shotAngle;

	// �ł�(0�`1)
	float shotPower;

	// �Ō�Ƀ{�[�����Փ˂�������
	double lastHitTime;

	// ���A���Ń{�[������ꂽ��
	int oldCombo;
	int combo;
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