#include "stdafx.h"
#include "TitleScene.h"

TitleScene::TitleScene(ResourceManager& rm, glm::uvec2 &size) : Scene(rm, size)
{
	XModel* tableModel = RModel(MODEL_TABLE);

	// テーブルの上を基準とする
	glm::vec3 globalPos = glm::vec3(0.f, -tableModel->box.max.y + 0.3f, 0.f);

	// モデルのグローバル座標を統一
	for (auto model : resource.modelList)
	{
		model.second->position = globalPos;
	}

	// ボール
	float padding = 2.4f;
	glm::vec3 wallMin = glm::vec3(-tableModel->box.max.x + padding, 0.f, -tableModel->box.max.z + padding);
	glm::vec3 wallMax = glm::vec3(tableModel->box.max.x - padding, 0.f, tableModel->box.max.z - padding);
	for (int i = 0; i <= 15; ++i)
	{
		RBall(i)->SetWall(wallMin, wallMax);
		RBall(i)->model.EnableRotate();
	}

	RBall(0)->model.position = glm::vec3(0.f, 0.f, 0.f);
	float r = RBall(0)->model.sphere.radius*2.f;
	int k = 1;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5 - i; ++j)
		{
			RBall(k)->model.position = glm::vec3(9.0f - j*r*sin(M_PI / 3), 0.f, j*r*cos(M_PI / 3) + (i - 2)*r);
			++k;
		}
	}

	// BGMを流す
	RSound(BGM_TITLE)->Play();
}

TitleScene::~TitleScene()
{
}

void TitleScene::Finish()
{
	// BGMをSTOP
	RSound(BGM_TITLE)->Stop();
}

void TitleScene::SetCamera()
{
	// 30秒の周期にする
	double elapsedTime = CurrentTime - (double)((int)(CurrentTime / 30.0)) * 30.0;
	glm::vec3 position;
	glm::vec3 angle    = glm::vec3(DegToRad(elapsedTime*12.0), DegToRad(45.0), 0.0);
	glm::vec3 target   = glm::vec3();
	glm::vec3 upvector = glm::vec3(0.0, 1.0, 0.0);
	float distance = 50.0f;

	//　視点位置を決定
	position[0] = sin(angle[0]) * cos(angle[1]) * distance;
	position[1] = sin(angle[1]) * distance;
	position[2] = cos(angle[0]) * cos(angle[1]) * distance;

	//　アップベクトルの設定
	if (angle[1] > DegToRad(90.0) && angle[1] < DegToRad(270.0)) upvector[1] = -1.0;
	else upvector[1] = 1.0;

	//　視点位置の設定
	gluLookAt(
		position[0], position[1], position[2],
		target[0], target[1], target[2],
		upvector[0], upvector[1], upvector[2]);
}

void TitleScene::Render2D()
{
	// 2秒の周期にする
	double elapsedTime = CurrentTime - (double)((int)(CurrentTime / 2.0)) * 2.0;
	double rate = elapsedTime;
	if (elapsedTime > 1.0)
	{
		rate = 2.0 - elapsedTime;
	}

	// FPSの描画
	glColor4f(1.f, 1.f, 1.f, 1.f);
	drawText(FpsString, FONT_BOKU, glm::uvec2(20, 40), glm::vec2());

	drawText("OpenGL Billiards", FONT_MISAKI, glm::uvec2(windowSize.x / 2 - 120, windowSize.y / 4), glm::vec2());

	glColor4f(1.f, 1.f, 1.f, (float)rate);
	drawText("Press Enter", FONT_MISAKI, glm::uvec2(windowSize.x / 2 - 80, windowSize.y * 3 / 4), glm::vec2());
}

void TitleScene::Render3D()
{
	for (int i = 0; i <= 15; ++i)
	{
		RBall(i)->model.Render();
	}
	RModel(MODEL_TABLE)->Render();
	RModel(MODEL_STAGE)->Render();
}

void TitleScene::Update()
{
	for (int i = 0; i < 15; ++i)
	{
		Ball* ball = RBall(i);
		ball->Move();
		ball->TestCollisionWall();
		for (int j = 0; j < 16; ++j)
		{
			if (i != j)
			{
				ball->TestCollisionBall(RBall(j));
			}
		}
		ball->TestPocket();
		ball->UpdateVelocity();
	}
}

void TitleScene::Keyboard(KEY key)
{
	switch (key)
	{
	case KEY_UP:
		RSound(BGM_TITLE)->AddGain(0.05f);
		break;
	case KEY_DOWN:
		RSound(BGM_TITLE)->AddGain(-0.05f);
		break;
	case KEY_ENTER:
		RSound(SE_START)->Play();
		Next(MAIN);
		break;
	default:
		break;
	}
}

void TitleScene::KeyboardUp(KEY key)
{
}

void TitleScene::Mouse(int button, int state, int x, int y)
{
}

void TitleScene::Motion(int x, int y)
{
}