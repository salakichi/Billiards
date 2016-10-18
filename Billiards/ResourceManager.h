#pragma once

// ���\�[�X�i���f���E�t�H���g�E�T�E���h�j���ꌳ�Ǘ�����p�N���X
// ���[�h���ɂ̓��[�h��ʂ�\��

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

// ���\�[�X�̃t�@�C���p�X
// ���\�[�X�ɃA�N�Z�X����ۂ̃L�[�Ƃ��Ă��g�p
#define TABLE_MODEL "data\\model\\table.x"
#define STAGE_MODEL "data\\room\\����Ƌ���_�_���X�p.x"
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

	// ���[�f�B���O��ʗp
	HANDLE thread;
	HANDLE mutex;
public:
	ResourceManager();
	~ResourceManager();
	bool Load();
	void Release();

	// ���f���f�[�^
	Ball* balls[16];
	map<string, XModel*> modelList;

	// �t�H���g�f�[�^
	map<string, Font*> fontList;

	// �T�E���h�f�[�^
	map<string, Sound*> soundList;
};