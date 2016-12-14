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

	// �t�H���g�T�C�Y
	RFont(FONT_MISAKI)->SetSize(32);

	// BGM�𗬂�
	RSound(BGM_MAIN)->Play();

	// �J�����ݒ�
	camera.SetEnableLeft(true, false);
	camera.SetEnableRight(false, false);
	camera.SetEnableMiddle(false, false);
	camera.SetDistance(25.0);
	camera.SetAngleDeg(glm::dvec3(-90.0, 20.0, 0));

	// �X�R�A�����Z�b�g
	SetScore(0);

	// ���̑�
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
	// BGM��STOP
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

	//�@�����̕`��
	glColor4f(1.f, 1.f, 1.f, 1.f);
	drawText((char *)TimeManager::Instance().GetFpsString(), FONT_CONTRA, glm::uvec2(20, 40), glm::vec2());
	drawText(scoreString, FONT_CONTRA_ITALIC, glm::uvec2(20, 80), glm::vec2());
	drawText(comboString, FONT_CONTRA_ITALIC, glm::uvec2(20, 120), glm::vec2());

	// �g�`��
	DrawSquareWithEdge(glm::uvec2(10, 10), glm::uvec2(200, 130), glm::vec4(0.f, 0.f, 0.f, 0.5f), glm::vec4(0.f, 0.f, 0.f, 1.f), 2.f);

	glColor4f(0.f, 0.f, 0.f, 1.f);
	drawText("Shot : Enter", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 40), glm::vec2());
	drawText("Angle : Drag", FONT_MISAKI, glm::uvec2(windowSize.x - 200, 80), glm::vec2());

	if (status == SELECT_POWER)
	{
		// �Q�[�W��2�b�̎����ɂ���
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
		// �L���[��2�b�����ňړ�������
		double distance = TimeManager::Instance().GetPeriodRatio(2.0, statusChangedTime);

		float angle = (float)camera.GetAngle().x;
		if (status == SELECT_POWER)
		{
			angle = shotAngle;
		}

		// �L���[�̕`��
		glm::vec3 pos = RBall(0)->position;
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(RadToDeg(angle) + 90, 0.f, 1.f, 0.f);
		glTranslatef(-9.0f, 0.45f, 0.f);
		glRotatef(-93.f, 0.f, 0.f, 1.f);
		glTranslatef(0.f, -distance, 0.f);
		RModel(MODEL_CUE)->Render();
		glPopMatrix();

		// �⏕���̕`��
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

			// �����Ă���
			if (ball->isMove())
			{
				isMove = true;
			}
			// �{�[���̏Փˉ��̍ő�l�����߂�
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
		// �{�[�����m���Փ˂��Ă����
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

			// �S�ė�������I��
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
				// ���ʂ���������I��
				Next(FINISH);
			}
			else
			{
				// �R���{�������ĂȂ������烊�Z�b�g
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
			// �p�x������
			shotAngle = (float)camera.GetAngle().x;
			ChangeStatus(SELECT_POWER);
		}
		else if (status == SELECT_POWER)
		{
			// �{�[����ł�
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
