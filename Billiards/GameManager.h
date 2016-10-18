#pragma once

// �Q�[���Ǘ��p�N���X

#include "TitleScene.h"
#include "MainScene.h"

class GameManager
{
private:
	bool InitializeGL(int argc, char** argv);
	void ErrorLog(char* msg);
	void Render2D();
	void Render3D();
	void RenderWithoutShadow();
	void RenderWithShadow();

	// �V���h�E�p
	GLuint shadowTexture;

	// ���\�[�X
	ResourceManager resource;

	// �V�[��
	Scene* scene;
public:
	GameManager();
	~GameManager();
	bool Initialize(int argc, char** argv);
	void Release();
	void Render();
	void KeyFunc(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void Update();
	void SetWindowSize(int x, int y);
	uint GetWindowWidth();
	uint GetWindowHeight();
	uint GetWindowPosX();
	uint GetWindowPosY();
};