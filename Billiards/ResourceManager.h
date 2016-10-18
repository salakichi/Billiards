#pragma once

// リソース（モデル・フォント・サウンド）を一元管理する用クラス
// ロード時にはロード画面を表示

#include "XLoader.h"
#include "Ball.h"
#include "Font.h"
#include "Sound.h"
#include <process.h>
#include <mmsystem.h>

#define WINDOW_DEFAULT_W 800
#define WINDOW_DEFAULT_H 800
#define WINDOW_DEFAULT_POS_X 0
#define WINDOW_DEFAULT_POS_Y 0

// リソースのファイルパス
// リソースにアクセスする際のキーとしても使用
#define TABLE_MODEL "data\\model\\table.x"
#define STAGE_MODEL "data\\room\\松野家居間_ダンス用.x"
#define BALL_MODEL "data\\model\\%d.x"
#define MISAKI_FONT "data\\font\\misaki_gothic.ttf"
#define BOKU_FONT "data\\font\\bokutachi.otf"
#define TITLE_BGM "data\\sound\\bgm.wav"

class ResourceManager
{
private:
	void ErrorLog(char* msg);
	void DrawLoadScreen();
	void CloseLoadScreen();

	// ローディング画面用
	HANDLE thread;
	HANDLE mutex;
public:
	ResourceManager();
	~ResourceManager();
	bool Load();
	void Release();

	// モデルデータ
	Ball* balls[16];
	map<string, XModel*> modelList;

	// フォントデータ
	map<string, Font*> fontList;

	// サウンドデータ
	map<string, Sound*> soundList;
};