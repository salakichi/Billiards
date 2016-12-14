#pragma once

#include "Scene.h"
#include "Mouse.h"

// ゲームメイン用シーン

#define M_PI 3.1415926535f
#define BASE_SCORE 100
#define MAX_POWER 0.5f

enum MAIN_SCENE_STATUS {
	SELECT_ANGLE, SELECT_POWER, SHOT,
};

class MainScene : public Scene
{
private:
	// 状態
	MAIN_SCENE_STATUS status;

	// 状態が切り替わった時間
	double statusChangedTime;

	// 状態変更
	void ChangeStatus(MAIN_SCENE_STATUS next);

	// カメラ
	ViewCamera camera;

	// 打つ角度(rad)
	float shotAngle;

	// 打つ力(0～1)
	float shotPower;

	// 最後にボールが衝突した時間
	double lastHitTime;

	// 何連続でボールを入れたか
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