#include "stdafx.h"
#include "TitleScene.h"

char FpsString[50] = { 0 };

TitleScene::TitleScene(ResourceManager& rm) : Scene(rm)
{
	XModel* tableModel = RModel(TABLE_MODEL);

	// �e�[�u���̏����Ƃ���
	glm::vec3 globalPos = glm::vec3(0.f, -tableModel->box.max.y + 0.3f, 0.f);

	// ���f���̃O���[�o�����W�𓝈�
	for (auto model : resource.modelList)
	{
		model.second->position = globalPos;
	}

	// �{�[��
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

	// �T�E���h
	RSound(TITLE_BGM)->Play();
}

void TitleScene::SetCamera()
{
	camera.Set();
}

void TitleScene::Render2D()
{
	//�@���Ԍv����FPS�Z�o
	CurrentCount = glutGet(GLUT_ELAPSED_TIME);
	CurrentTime = (CurrentCount - LastCount) / 1000.0;
	FrameCount++;
	if ((CurrentTime - LastTime) >= 1.0)
	{
		Fps = FrameCount / (CurrentTime - LastTime);
		sprintf_s(FpsString, "%.3f FPS", Fps);
		FrameCount = 0;
		LastTime = CurrentTime;
	}

	//�@�����̕`��
	drawText((char *)FpsString, BOKU_FONT, glm::uvec2(20, 40), glm::vec2());
}

void TitleScene::Render3D()
{
	for (int i = 0; i <= 16; ++i)
	{
		RBall(i)->model.Render();
	}
	RModel(TABLE_MODEL)->Render();
	RModel(STAGE_MODEL)->Render();
}

void TitleScene::Update()
{
	for (int i = 0; i < 16; ++i)
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

void TitleScene::KeyFunc(KEY key)
{
	switch (key)
	{
	case KEY_UP:
		RSound(TITLE_BGM)->AddGain(0.05f);
		break;
	case KEY_DOWN:
		RSound(TITLE_BGM)->AddGain(-0.05f);
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case KEY_W:
		break;
	case KEY_A:
		break;
	case KEY_S:
		break;
	case KEY_D:
		break;
	case KEY_V:
		break;
	case KEY_SPACE:
		break;
	case KEY_ENTER:
		RBall(0)->AddVec(glm::vec3(0.25f, 0.f, 0.f));
		break;
	case KEY_BACKSPACE:
		break;
	case KEY_1:
	case KEY_2:
	case KEY_3:
		break;
	}
}

void TitleScene::Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

void TitleScene::Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}