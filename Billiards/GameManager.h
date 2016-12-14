#pragma once

// �Q�[���Ǘ��p�N���X

#include "TitleScene.h"
#include "MainScene.h"
#include "FinishScene.h"

class GameManager
{
private:
	bool InitializeGL(int argc, char** argv);
	void ErrorLog(char* msg);
	void Render2D();
	void Render3D();
	void RenderWithoutShadow();
	void RenderWithShadow();
	void UpdateFps();

	// �V���h�E�p
	GLuint shadowTexture;

	// ���\�[�X
	ResourceManager resource;

	// �V�[��
	Scene* scene;

	// �E�B���h�E�p
	glm::uvec2 windowSize;
public:
	// �V���O���g���p�̃C���X�^���X
	static GameManager &Instance() {
		static GameManager inst;
		return inst;
	}

	GameManager();
	~GameManager();
	bool Initialize(int argc, char** argv);
	void Release();
	void Render();
	void Keyboard(KEY key);
	void KeyboardUp(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void Update();
	void SetWindowSize(int x, int y);
};