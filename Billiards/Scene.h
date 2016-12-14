#pragma once

// シーンの基底クラス
// シーン別で描画や入力・更新処理を行うためのもの

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
	// リソースアクセス用
	ResourceManager& resource;
	XModel* RModel(string name) { return resource.modelList[name]; }
	Ball* RBall(int i) { return resource.balls[i]; }
	Sound* RSound(string name) { return resource.soundList[name]; }
	Font* RFont(string name) { return resource.fontList[name]; }
	int GetScore() { return resource.score; }
	void SetScore(int score) { resource.score = score; }
	void AddScore(int score) { resource.score += score; }

	// シーン遷移用
	GAME_STATUS nextScene;
	void Next(GAME_STATUS next);

	// 画面サイズ
	glm::uvec2 &windowSize;

	// 共通描画関数（2D用）
	void glutRenderText(void* bitmapfont, char*text);
	void DrawSmallCircle(float radius, int x, int y);		// 半径32.0まで 品質：良
	void DrawLargeCircle(float radius, int x, int y);		// 半径制限なし 品質：悪
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move);
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize);
	void DrawSquareWithEdge(glm::uvec2 startPos, glm::uvec2 endPos, glm::vec4 mainColor, glm::vec4 edgeColor = glm::vec4(), float edgeSize = 0.f);
	void DrawSquareWithGradation(glm::uvec2 startPos, glm::uvec2 endPos, glm::vec4 startColor, glm::vec4 endColor);
public:
	Scene(ResourceManager &r, glm::uvec2 &size);
	~Scene();
	
	bool IsFinished() { return nextScene != NONE; }
	GAME_STATUS GetNextScene() { return nextScene; }

	// 仮想関数
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