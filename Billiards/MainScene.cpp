#include "stdafx.h"
#include "MainScene.h"

MainScene::MainScene(ResourceManager& rm, glm::uvec2 &size) : Scene(rm, size)
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
	RSound(BGM_MAIN)->Play();

	// カメラ設定
	camera.SetEnableLeft(true, false);
	camera.SetEnableRight(false, false);
	camera.SetEnableMiddle(false, false);
	camera.SetDistance(25.0);
	camera.SetAngleDeg(glm::dvec3(-90.0, 20.0, 0));

	shotFlag = false;
}

MainScene::~MainScene()
{
}

void MainScene::Finish()
{
	// BGMをSTOP
	RSound(BGM_MAIN)->Stop();
}

void MainScene::SetCamera()
{
	camera.SetTarget(RBall(0)->model.position);
	camera.Set();
}

void MainScene::Render2D()
{
	//　文字の描画
	drawText((char *)FpsString, FONT_BOKU, glm::uvec2(20, 40), glm::vec2());
}

void MainScene::Render3D()
{
	// 2秒の周期にする
	double elapsedTime = CurrentTime - (double)((int)(CurrentTime / 2.0)) * 2.0;
	double distance = elapsedTime;
	if (elapsedTime > 1.0)
	{
		distance = 2.0 - elapsedTime;
	}

	for (int i = 0; i <= 15; ++i)
	{
		RBall(i)->model.Render();
	}
	RModel(MODEL_TABLE)->Render();
	RModel(MODEL_STAGE)->Render();

	if (!shotFlag)
	{
		glm::vec3 pos = RBall(0)->model.position;
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(RadToDeg(camera.angle[0]) + 90, 0.f, 1.f, 0.f);
		glTranslatef(-9.0f, 0.45f, 0.f);
		glRotatef(-93.f, 0.f, 0.f, 1.f);
		glTranslatef(0.f, -distance, 0.f);
		RModel(MODEL_CUE)->Render();
		glPopMatrix();
	}
}

void MainScene::Update()
{
	if (shotFlag)
	{
		bool isMove = false;
		for (int i = 0; i <= 15; ++i)
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
			if (ball->isMove)
			{
				isMove = true;
			}
		}
		if (!isMove)
		{
			shotFlag = false;
		}
	}
}

void MainScene::Keyboard(KEY key)
{
	float speed, angle;
	switch (key)
	{
	case KEY_UP:
		RSound(BGM_MAIN)->AddGain(0.05f);
		break;
	case KEY_DOWN:
		RSound(BGM_MAIN)->AddGain(-0.05f);
		break;
	case KEY_ENTER:
		if (!shotFlag)
		{
			speed = 0.25f;
			angle = (float)camera.angle[0];
			RBall(0)->AddVec(glm::vec3(speed*-sin(angle), 0.f, speed*-cos(angle)));
			shotFlag = true;
		}
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