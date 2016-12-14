#pragma once

// �V�[���̊��N���X
// �V�[���ʂŕ`�����́E�X�V�������s�����߂̂���

#include "ResourceManager.h"

enum KEY {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_V,
	KEY_SPACE, KEY_ENTER, KEY_BACKSPACE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5
};

enum GAME_STATUS {
	NONE, TITLE, MAIN, FINISH,
};

class Scene
{
protected:
	// ���\�[�X�A�N�Z�X�p
	ResourceManager& resource;
	XModel* RModel(string name) { return resource.modelList[name]; }
	Ball* RBall(int i) { return resource.balls[i]; }
	Sound* RSound(string name) { return resource.soundList[name]; }
	Font* RFont(string name) { return resource.fontList[name]; }
	int GetScore() { return resource.score; }
	void SetScore(int score) { resource.score = score; }
	void AddScore(int score) { resource.score += score; }

	// �V�[���J�ڗp
	GAME_STATUS nextScene;
	void Next(GAME_STATUS next);

	// ��ʃT�C�Y
	glm::uvec2 &windowSize;

	// ���ʕ`��֐��i2D�p�j
	void glutRenderText(void* bitmapfont, char*text);
	void DrawSmallCircle(float radius, int x, int y);		// ���a32.0�܂� �i���F��
	void DrawLargeCircle(float radius, int x, int y);		// ���a�����Ȃ� �i���F��
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move);
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize);
	void DrawSquareWithEdge(glm::uvec2 startPos, glm::uvec2 endPos, glm::vec4 mainColor, glm::vec4 edgeColor = glm::vec4(), float edgeSize = 0.f);
	void DrawSquareWithGradation(glm::uvec2 startPos, glm::uvec2 endPos, glm::vec4 startColor, glm::vec4 endColor);
public:
	Scene(ResourceManager &r, glm::uvec2 &size);
	~Scene();
	
	bool IsFinished() { return nextScene != NONE; }
	GAME_STATUS GetNextScene() { return nextScene; }

	// ���z�֐�
	virtual void SetCamera(){}
	virtual void Render2D(){}
	virtual void Render3D(){}
	virtual void Update(){}
	virtual void Keyboard(KEY key){}
	virtual void KeyboardUp(KEY key) {}
	virtual void Mouse(int button, int state, int x, int y){}
	virtual void Motion(int x, int y){}
	virtual void Finish(){}
};