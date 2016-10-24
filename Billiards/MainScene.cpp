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
	float padding = 2.3f;
	glm::vec3 wallMin = glm::vec3(-tableModel->box.max.x + padding, 0.f, -tableModel->box.max.z + padding);
	glm::vec3 wallMax = glm::vec3(tableModel->box.max.x - padding, 0.f, tableModel->box.max.z - padding);
	for (int i = 0; i <= 15; ++i)
	{
		RBall(i)->SetWall(wallMin, wallMax);
		RBall(i)->model.EnableRotate();
	}

	RBall(0)->model.position = glm::vec3();
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
	lastHitTime = 0.0;
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
	if (shotFlag)
	{
		camera.SetOffset(glm::dvec3());
		camera.SetTarget(RBall(0)->model.position);
	}
	else
	{
		camera.SetOffset(RBall(0)->model.position);
		camera.SetTarget(glm::dvec3());
	}
	camera.Set();
}

void MainScene::Render2D()
{
	//　文字の描画
	glColor4f(1.f, 1.f, 1.f, 1.f);
	drawText((char *)FpsString, FONT_BOKU, glm::uvec2(20, 40), glm::vec2());

	glColor4f(0.f, 0.f, 0.f, 1.f);
	drawText("Shot : Enter", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 40), glm::vec2());
	drawText("Angle : Drag", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 80), glm::vec2());
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
		glRotatef(RadToDeg(camera.GetAngle().x) + 90, 0.f, 1.f, 0.f);
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
		bool pocketFlag = false;
		float maxHitScale = 0.f;
		for (int i = 0; i <= 15; ++i)
		{
			Ball* ball = RBall(i);
			ball->ResetHit();
			ball->Move();
			ball->TestPocket();
			ball->TestCollisionWall();
			for (int j = 0; j < 16; ++j)
			{
				if (i != j)
				{
					ball->TestCollisionBall(RBall(j));
				}
			}
			ball->UpdateVelocity();

			// 動いている
			if (ball->status == MOVE || ball->status == FALL)
			{
				isMove = true;
			}
			// ボールの衝突音の最大値を求める
			maxHitScale = max(maxHitScale, ball->GetHitScale());

			if (ball->lastStatus != ball->status)
			{
				if (ball->status == POCKET)
				{
					pocketFlag = true;
				}
			}

			ball->lastStatus = ball->status;
		}
		if (maxHitScale > 0.f && CurrentTime - lastHitTime > 0.05)
		{
			maxHitScale = min(maxHitScale, 1.f);
			RSound(SE_BALL)->SetGain(maxHitScale);
			RSound(SE_BALL)->Play();
			lastHitTime = CurrentTime;
		}
		if (pocketFlag)
		{
			RSound(SE_POCKET)->Play();
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
			angle = (float)camera.GetAngle().x;
			RBall(0)->AddVec(glm::vec3(speed*-sin(angle), 0.f, speed*-cos(angle)));
			RSound(SE_SHOT)->SetGain(0.5f);
			RSound(SE_SHOT)->Play();
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