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
#define MODEL_TABLE "data\\model\\table.x"
#define MODEL_STAGE "data\\room\\松野家居間_ダンス用.x"
#define MODEL_BALL "data\\model\\%d.x"
#define MODEL_CUE "data\\model\\cue.x"
#define FONT_MISAKI "data\\font\\misaki_gothic.ttf"
#define FONT_BOKU "data\\font\\bokutachi.otf"
#define BGM_TITLE "data\\bgm\\title.wav"
#define BGM_MAIN "data\\bgm\\main.wav"
#define SE_START "data\\se\\start.wav"
#define SE_CLICK "data\\se\\click.wav"
#define SE_SHOT "data\\se\\shot.wav"
#define SE_BALL "data\\se\\ball.wav"
#define SE_POCKET "data\\se\\pocket.wav"

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