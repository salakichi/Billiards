#pragma once

// シーンの基底クラス
// シーン別で描画や入力・更新処理を行うためのもの

#include "ResourceManager.h"

enum KEY {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_V,
	KEY_SPACE, KEY_ENTER, KEY_BACKSPACE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5
};

class Scene
{
protected:
	// FPS用
	double CurrentTime;
	double LastTime;
	double CurrentCount;
	double LastCount;
	int FrameCount;
	double Fps;

	// リソースアクセス用
	ResourceManager& resource;
	XModel* RModel(string name) { return resource.modelList[name]; }
	Ball* RBall(int i) { return resource.balls[i]; }
	Sound* RSound(string name) { return resource.soundList[name]; }
	Font* RFont(string name) { return resource.fontList[name]; }

	// 共通描画関数（2D用）
	void glutRenderText(void* bitmapfont, char*text);
	void DrawSmallCircle(float radius, int x, int y);		// 半径32.0まで 品質：良
	void DrawLargeCircle(float radius, int x, int y);		// 半径制限なし 品質：悪
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move);
	int drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize);
public:
	Scene(ResourceManager &r);
	~Scene();
	virtual void SetCamera(){}
	virtual void Render2D(){}
	virtual void Render3D(){}
	virtual void Update(){}
	virtual void KeyFunc(KEY key){}
	virtual void Mouse(int button, int state, int x, int y){}
	virtual void Motion(int x, int y){}
};