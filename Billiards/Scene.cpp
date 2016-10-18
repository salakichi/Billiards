#include "stdafx.h"
#include "Scene.h"

Scene::Scene(ResourceManager &r) : resource(r)
{
	//�@�J�E���^�[�̏�����
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;
	CurrentTime = 0.0;
	LastTime = 0.0;
	LastCount = 0.0;
	Fps = 0.0f;

	// ���̃V�[�������
	nextScene = NONE;
}

Scene::~Scene()
{
}

void Scene::Next(GAME_STATUS next)
{
	nextScene = next;
}

void Scene::UpdateFps()
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
}

void Scene::DrawSmallCircle(float radius, int x, int y)
{
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(radius*2.0f);
	glBegin(GL_POINTS);
	glVertex2f((float)x, (float)y);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
}

void Scene::DrawLargeCircle(float radius, int x, int y)
{
	for (float th1 = 0.0f; th1 <= 360.0f; th1 = th1 + 2.0f)
	{
		float th2 = th1 + 10.0f;
		float th1_rad = th1 * 0.0174533f;
		float th2_rad = th2 * 0.0174533f;

		float x1 = radius * cos(th1_rad);
		float y1 = radius * sin(th1_rad);
		float x2 = radius * cos(th2_rad);
		float y2 = radius * sin(th2_rad);

		glBegin(GL_TRIANGLES);
		glVertex2f((float)x, (float)y);
		glVertex2f(x1 + x, y1 + y);
		glVertex2f(x2 + x, y2 + y);
		glEnd();
	}
}

void Scene::glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

int Scene::drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move)
{
	unsigned int textLength;	//�����Ŏ󂯎�����e�L�X�g�̒���
	WCHAR * unicodeText;		//text��UNICODE�ɕϊ�������������i�[����

	glRasterPos2i(pos.x, pos.y);

	//���{��̕�����Ƃ��Ĉ����悤�ݒ�
	setlocale(LC_CTYPE, "jpn");

	//text�̕��������擾
	textLength = (unsigned int)_mbstrlen(text);
	if (textLength == -1)
		return -1;

	//text�̕��������̃��C�h������̗̈���쐬
	unicodeText = new WCHAR[textLength + 1];
	if (unicodeText == NULL)
	{
		return -2;
	}

	//�擾�����W���C���gID��UNICODE�ɕϊ�����
	if (MultiByteToWideChar(CP_ACP, 0, text, -1, unicodeText, (sizeof(WCHAR) * textLength) + 1) == 0)
		return -3;

	// �\��
	RFont(fontName)->Render(unicodeText);

	delete[] unicodeText;

	return 1;
}

// ����
int Scene::drawText(char *text, string fontName, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize)
{
	// ������`��
	glColor4f(mainColor.r, mainColor.g, mainColor.b, mainColor.a);
	drawText(text, fontName, move, pos);

	// �����
	if (edgeSize > 0)
	{
		glColor4f(edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
		for (int i = -edgeSize + 1; i<edgeSize; i++)
		{
			drawText(text, fontName, move, glm::vec2(pos.x + i, pos.y + (edgeSize - i)));
			drawText(text, fontName, move, glm::vec2(pos.x + i, pos.y - (edgeSize - i)));
		}
		drawText(text, fontName, move, glm::vec2(pos.x + edgeSize, pos.y));
		drawText(text, fontName, move, glm::vec2(pos.x - edgeSize, pos.y));
	}

	return 1;
}