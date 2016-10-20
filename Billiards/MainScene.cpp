#include "stdafx.h"
#include "MainScene.h"

MainScene::MainScene(ResourceManager& rm, glm::uvec2 &size) : Scene(rm, size)
{
	XModel* tableModel = RModel(MODEL_TABLE);

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

	// BGM�𗬂�
	RSound(BGM_MAIN)->Play();
}

MainScene::~MainScene()
{
}

void MainScene::Finish()
{
	// BGM��STOP
	RSound(BGM_MAIN)->Stop();
}

void MainScene::SetCamera()
{
	camera.Set();
}

void MainScene::Render2D()
{
	//�@�����̕`��
	drawText((char *)FpsString, FONT_BOKU, glm::uvec2(20, 40), glm::vec2());
}

void MainScene::Render3D()
{
	for (int i = 0; i <= 15; ++i)
	{
		RBall(i)->model.Render();
	}
	RModel(MODEL_TABLE)->Render();
	RModel(MODEL_STAGE)->Render();
}

void MainScene::Update()
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

void MainScene::Keyboard(KEY key)
{
	switch (key)
	{
	case KEY_UP:
		RSound(BGM_MAIN)->AddGain(0.05f);
		break;
	case KEY_DOWN:
		RSound(BGM_MAIN)->AddGain(-0.05f);
		break;
	case KEY_ENTER:
		RBall(0)->AddVec(glm::vec3(0.25f, 0.f, 0.f));
		break;
	default:
		break;
	}
}

void MainScene::KeyboardUp(KEY key)
{
}

void MainScene::Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

void MainScene::Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}