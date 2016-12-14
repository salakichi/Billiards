#include "stdafx.h"
#include "FinishScene.h"

FinishScene::FinishScene(ResourceManager& rm, glm::uvec2 &size) : Scene(rm, size)
{

}

FinishScene::~FinishScene()
{
}

void FinishScene::Finish()
{
}

void FinishScene::SetCamera()
{
	glm::vec3 position;
	glm::vec3 angle = glm::vec3(DegToRad(45.0), DegToRad(45.0), 0.0);
	glm::vec3 target = glm::vec3();
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

void FinishScene::Render2D()
{
	char scoreString[256] = "";
	sprintf_s(scoreString, "Score:%d", GetScore());

	//　文字の描画
	glColor4f(1.f, 1.f, 1.f, 1.f);
	drawText(TimeManager::Instance().GetFpsString(), FONT_CONTRA, glm::uvec2(20, 40), glm::vec2());

	drawText("Finish", FONT_CONTRA_ITALIC, glm::uvec2(windowSize.x / 2 - 100, windowSize.y / 2), glm::vec2());
	drawText(scoreString, FONT_CONTRA_ITALIC, glm::uvec2(windowSize.x / 2 - 100, windowSize.y / 2 + 40), glm::vec2());
	drawText("Continue => Press Enter", FONT_CONTRA_ITALIC, glm::uvec2(windowSize.x / 2 - 100, windowSize.y / 2 + 80), glm::vec2());

	glColor4f(0.f, 0.f, 0.f, 0.4f);
	glBegin(GL_QUADS);
	glVertex2f(0.f, 0.f);
	glVertex2f(windowSize.x, 0.f);
	glVertex2f(windowSize.x, windowSize.y);
	glVertex2f(0.f, windowSize.y);
	glEnd();
}

void FinishScene::Render3D()
{
	for (int i = 0; i <= 15; ++i)
	{
		if (RBall(i)->status != POCKET)
			RBall(i)->Render();
	}
	RModel(MODEL_TABLE)->Render();
	RModel(MODEL_STAGE)->Render();
}

void FinishScene::Update()
{
}

void FinishScene::Keyboard(KEY key)
{
	float speed, angle;
	switch (key)
	{
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_ENTER:
		Next(MAIN);
		break;
	default:
		break;
	}
}

void FinishScene::KeyboardUp(KEY key)
{
}

void FinishScene::Mouse(int button, int state, int x, int y)
{
}

void FinishScene::Motion(int x, int y)
{
}