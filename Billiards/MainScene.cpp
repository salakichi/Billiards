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

	RBall(0)->Initialize(glm::vec3());
	RBall(0)->SetWall(wallMin, wallMax);

	float r = RBall(0)->size*2.f;
	int k = 1;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5 - i; ++j)
		{
			RBall(k)->Initialize(glm::vec3(9.0f - j*r*sin(M_PI / 3), 0.f, j*r*cos(M_PI / 3) + (i - 2)*r));
			RBall(k)->SetWall(wallMin, wallMax);
			++k;
		}
	}

	// フォントサイズ
	RFont(FONT_MISAKI)->SetSize(32);

	// BGMを流す
	RSound(BGM_MAIN)->Play();

	// カメラ設定
	camera.SetEnableLeft(true, false);
	camera.SetEnableRight(false, false);
	camera.SetEnableMiddle(false, false);
	camera.SetDistance(25.0);
	camera.SetAngleDeg(glm::dvec3(-90.0, 20.0, 0));

	// スコアをリセット
	SetScore(0);

	// その他
	shotAngle = 0.f;
	shotPower = 0.f;
	lastHitTime = 0.0;
	combo = 0;
	oldCombo = 0;

	ChangeStatus(SELECT_ANGLE);
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
	if (status == SHOT)
	{
		camera.SetOffset(glm::dvec3());
		camera.SetTarget(RBall(0)->position);
	}
	else
	{
		camera.SetOffset(RBall(0)->position);
		camera.SetTarget(glm::dvec3());
	}
	camera.Set();
}

void MainScene::Render2D()
{
	char scoreString[256] = "";
	sprintf_s(scoreString, "Score:%d", GetScore());

	char comboString[256] = "";
	sprintf_s(comboString, "Combo:%d", combo);

	//　文字の描画
	glColor4f(1.f, 1.f, 1.f, 1.f);
	drawText((char *)TimeManager::Instance().GetFpsString(), FONT_CONTRA, glm::uvec2(20, 40), glm::vec2());
	drawText(scoreString, FONT_CONTRA_ITALIC, glm::uvec2(20, 80), glm::vec2());
	drawText(comboString, FONT_CONTRA_ITALIC, glm::uvec2(20, 120), glm::vec2());

	// 枠描画
	DrawSquareWithEdge(glm::uvec2(10, 10), glm::uvec2(200, 130), glm::vec4(0.f, 0.f, 0.f, 0.5f), glm::vec4(0.f, 0.f, 0.f, 1.f), 2.f);

	glColor4f(0.f, 0.f, 0.f, 1.f);
	drawText("Shot : Enter", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 40), glm::vec2());
	drawText("Angle : Drag", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 80), glm::vec2());

	if (status == SELECT_POWER)
	{
		// ゲージを2秒の周期にする
		shotPower = TimeManager::Instance().GetPeriodRatio(2.0, statusChangedTime);
		glm::uvec2 center = glm::uvec2(windowSize.x / 2, windowSize.y / 2);
		DrawSquareWithGradation(glm::uvec2(center.x + 100, center.y + 50 - 100 * shotPower), glm::uvec2(center.x + 120, center.y + 50), glm::vec4(1.f, 1.f-shotPower, 0.f, 1.f), glm::vec4(1.f, 1.f, 0.f, 1.f));
		DrawSquareWithEdge(glm::uvec2(center.x + 100, center.y - 50), glm::uvec2(center.x + 120, center.y + 50), glm::vec4(0.f, 0.f, 0.f, 0.5f), glm::vec4(0.f, 0.f, 0.f, 1.f), 2.f);
	}
}

void MainScene::Render3D()
{
	for (int i = 0; i <= 15; ++i)
	{
		if (RBall(i)->status != POCKET)
			RBall(i)->Render();
	}
	RModel(MODEL_TABLE)->Render();
	RModel(MODEL_STAGE)->Render();

	if (status != SHOT)
	{
		// キューを2秒周期で移動させる
		double distance = TimeManager::Instance().GetPeriodRatio(2.0, statusChangedTime);

		float angle = (float)camera.GetAngle().x;
		if (status == SELECT_POWER)
		{
			angle = shotAngle;
		}

		// キューの描画
		glm::vec3 pos = RBall(0)->position;
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(RadToDeg(angle) + 90, 0.f, 1.f, 0.f);
		glTranslatef(-9.0f, 0.45f, 0.f);
		glRotatef(-93.f, 0.f, 0.f, 1.f);
		glTranslatef(0.f, -distance, 0.f);
		RModel(MODEL_CUE)->Render();
		glPopMatrix();

		// 補助線の描画
		float length = 10.f;
		glLineWidth(3.0);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xACF3);
		glBegin(GL_LINE_LOOP);
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x - length * sin(angle), pos.y, pos.z - length * cos(angle));
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
}

void MainScene::Update()
{
	if (status == SHOT)
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
			if (ball->isMove())
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
					AddScore(BASE_SCORE*pow(2, combo));
					combo++;
				}
			}

			ball->lastStatus = ball->status;
		}
		// ボール同士が衝突していれば
		double currentTime = TimeManager::Instance().GetCurrentTime();
		if (maxHitScale > 0.f && currentTime - lastHitTime > 0.05)
		{
			maxHitScale = min(maxHitScale, 1.f);
			RSound(SE_BALL)->SetGain(maxHitScale);
			RSound(SE_BALL)->Play();
			lastHitTime = currentTime;
		}
		if (pocketFlag)
		{
			// SE
			RSound(SE_POCKET)->Play();

			// 全て落ちたら終了
			int pocketNum = 0;
			for (int i = 1; i <= 15; ++i)
			{
				if (RBall(i)->status == POCKET) ++pocketNum;
			}
			if (pocketNum == 15)
			{
				Next(FINISH);
			}
		}
		if (!isMove)
		{
			if (RBall(0)->status == POCKET)
			{
				// 白玉が落ちたら終了
				Next(FINISH);
			}
			else
			{
				// コンボが増えてなかったらリセット
				if (combo == oldCombo)
				{
					combo = 0;
				}
				oldCombo = combo;

				ChangeStatus(SELECT_ANGLE);
			}
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
	case KEY_LEFT:
		break;
	case KEY_DOWN:
		RSound(BGM_MAIN)->AddGain(-0.05f);
		break;
	case KEY_RIGHT:
		break;
	case KEY_ENTER:
		if (status == SELECT_ANGLE)
		{
			// 角度を決定
			shotAngle = (float)camera.GetAngle().x;
			ChangeStatus(SELECT_POWER);
		}
		else if (status == SELECT_POWER)
		{
			// ボールを打つ
			speed = MAX_POWER * shotPower;
			RBall(0)->AddVec(glm::vec3(speed*-sin(shotAngle), 0.f, speed*-cos(shotAngle)));
			RSound(SE_SHOT)->SetGain(0.5f);
			RSound(SE_SHOT)->Play();
			ChangeStatus(SHOT);
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

void MainScene::ChangeStatus(MAIN_SCENE_STATUS next)
{
	status = next;
	statusChangedTime = TimeManager::Instance().GetCurrentTime();
}
