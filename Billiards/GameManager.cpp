#include "stdafx.h"
#include "GameManager.h"

const char* tableModelPath = "data\\table.x";
const char* ballModelPath = "data\\%d.x";
const char* skyboxModelPath = "data\\room\\����Ƌ���_�_���X�p.x";
const char* misakiFontPath = "data\\font\\misaki_gothic.ttf";
const char* bokuFontPath = "data\\font\\bokutachi.otf";
#define MISAKI_FONT "����t�H���g"
#define BOKU_FONT "�ڂ������̃S�V�b�N"

static const GLfloat lightpos[] = { 0.0f, 5.0f, 0.0f, 1.f }; /* �ʒu�@�@�@�@�@�@�@ */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.f }; /* ���ڌ����x�@�@�@�@ */
static const GLfloat lightdim[] = { 0.2f, 0.2f, 0.2f, 1.f }; /* �e���̊g�U���ˋ��x */
static const GLfloat lightblk[] = { 0.0f, 0.0f, 0.0f, 1.f }; /* �e���̋��ʔ��ˋ��x */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.f }; /* �������x�@�@�@�@ */

#define DEPTH_TEX_WIDTH  512
#define DEPTH_TEX_HEIGHT 512
//#define SHADOW_MAP_ON

glm::uvec2 windowPos;
glm::uvec2 windowSize;
bool loadFinishFlag = false;
char FpsString[50] = { 0 };

void glutRenderText(void* bitmapfont, char*text);
void DrawSmallCircle(float radius, int x, int y);		// ���a32.0�܂� �i���F��
void DrawLargeCircle(float radius, int x, int y);		// ���a�����Ȃ� �i���F��
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move);
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize);

GameManager::GameManager()
{
	// �X���b�h�֘A�̏�����
	mutex = NULL;
	thread = NULL;

	status = TITLE;

	//�@�J�E���^�[�̏�����
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;
	CurrentTime = 0.0;
	LastTime = 0.0;
	LastCount = 0.0;
	Fps = 0.0f;

	windowSize.x = WINDOW_DEFAULT_W;
	windowSize.y = WINDOW_DEFAULT_H;
	windowPos.x = WINDOW_DEFAULT_POS_X;
	windowPos.y = WINDOW_DEFAULT_POS_Y;
}

GameManager::~GameManager()
{
}

void GameManager::ErrorLog(char* msg)
{
	cout << "GameManager Error : " << msg << endl;
}

bool GameManager::Initialize(int argc, char** argv)
{
	if (!InitializeGL(argc, argv))
	{
		ErrorLog("Initialize() : failed to initialize GL");
		return false;
	}

	if (!LoadMaterial())
	{
		ErrorLog("Initialize() : failed to load material data");
		return false;
	}
	InitializeModel();

	return true;
}

void GameManager::Release()
{
	tableModel.Release();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Release();
	}
	skyboxModel.Release();
}

bool GameManager::InitializeGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(windowPos.x, windowPos.y);
	glutInitWindowSize(windowSize.x, windowSize.y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("�r�����[�h");
	if (glewInit() != GLEW_OK)
	{
		ErrorLog("InitializeGL() : glew�̏������Ɏ��s���܂����B");
		return false;
	}

	// �ʏ�̃e�N�X�`���̐ݒ�
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//�@�o�b�N�o�b�t�@���N���A����F�̎w��
	//glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
	glClearColor(1.f, 1.f, 1.0f, 1.0f);

	// �J�����O�ł���悤�ɂ���
	glEnable(GL_CULL_FACE);

	//�@�[�x�e�X�gON
	glEnable(GL_DEPTH_TEST);

	//�@���C�e�B���O
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

#ifdef SHADOW_MAP_ON
	glActiveTexture(GL_TEXTURE1);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);

	/* �e�N�X�`���̊��蓖�� */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	/* �e�N�X�`�����g��E�k��������@�̎w�� */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/* �e�N�X�`���̌J��Ԃ����@�̎w�� */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* �������ރ|���S���̃e�N�X�`�����W�l�̂q�ƃe�N�X�`���Ƃ̔�r���s���悤�ɂ��� */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	/* �����q�̒l���e�N�X�`���̒l�ȉ��Ȃ�^�i�܂�����j */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	/* ��r�̌��ʂ��P�x�l�Ƃ��ē��� */
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	/* �e�N�X�`�����j�b�g7�̃e�N�X�`���� */
	/*
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	static const GLfloat blend[] = { 1.0, 1.0, 1.0, 0.5 };
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blend);
	*/
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* �e�N�X�`�����W�Ɏ��_���W�n�ɂ����镨�̂̍��W�l��p���� */
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	/* ���������e�N�X�`�����W�����̂܂� (S, T, R, Q) �Ɏg�� */
	static const GLdouble genfunc[][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 },
	};
	glTexGendv(GL_S, GL_EYE_PLANE, genfunc[0]);
	glTexGendv(GL_T, GL_EYE_PLANE, genfunc[1]);
	glTexGendv(GL_R, GL_EYE_PLANE, genfunc[2]);
	glTexGendv(GL_Q, GL_EYE_PLANE, genfunc[3]);

	glDisable(GL_TEXTURE_2D);

	// �ʏ�̃e�N�X�`���ɖ߂�
	glActiveTexture(GL_TEXTURE0);
#endif

	return true;
}

unsigned __stdcall LoadingScreen(void* pArguments)
{
	cv::Mat img((int)windowSize.y, (int)windowSize.x, CV_8UC3);

	const char* text[4] = { "Loading   ", "Loading.  ", "Loading.. ", "Loading..." };
	const string windowTitle("Loading");
	cv::namedWindow(windowTitle, CV_WINDOW_AUTOSIZE);

	int i = 0;

	while (1) {
		DWORD start, end;
		start = timeGetTime();

		cvMoveWindow(windowTitle.c_str(), (int)windowPos.x, (int)windowPos.y);

		img = cv::Scalar(255, 255, 255);
		cv::putText(img, text[i / 10], cv::Point((int)windowSize.x - 325, (int)windowSize.y - 75), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2.0, cv::Scalar(0, 0, 0), 2, CV_AA);

		cv::imshow(windowTitle, img);

		end = timeGetTime();
		if (end - start<66)
			cvWaitKey(66 - (end - start));

		void* handle = cvGetWindowHandle(windowTitle.c_str());
		if (loadFinishFlag || handle == NULL)
		{
			loadFinishFlag = true;
			break;
		}

		i++;
		if (i == 40)
			i = 0;
	}
	cv::destroyWindow(windowTitle);

	return 0;
}

void GameManager::DrawLoadScreen()
{
	loadFinishFlag = false;
	mutex = CreateMutex(NULL, FALSE, NULL);
	thread = (HANDLE)_beginthreadex(NULL, 0, &LoadingScreen, NULL, 0, NULL);
}

void GameManager::CloseLoadScreen()
{
	WaitForSingleObject(mutex, INFINITE);
	loadFinishFlag = true;
	ReleaseMutex(mutex);
	WaitForSingleObject(thread, INFINITE);

	CloseHandle(mutex);
	CloseHandle(thread);
}

bool GameManager::LoadMaterial()
{
	bool loadResult = true;

	try {
		// ���[�f�B���O�\��
		DrawLoadScreen();

		//�@���f���t�@�C���̓ǂݍ���
		loadResult = tableModel.Load((char*)tableModelPath);
		if (!loadResult) throw tableModelPath;

		loadResult = skyboxModel.Load((char*)skyboxModelPath, 20.f);
		if (!loadResult) throw skyboxModelPath;

		char ballFilename[256];
		for (int i = 0; i <= 15; ++i)
		{
			sprintf_s(ballFilename, ballModelPath, i);
			loadResult = balls[i].model.Load(ballFilename);
			if (!loadResult) throw ballFilename;
		}

		// �t�H���g�t�@�C���̓ǂݍ���
		loadResult = font.Load(misakiFontPath, MISAKI_FONT, 32);
		if (!loadResult) throw misakiFontPath;

		loadResult = font.Load(bokuFontPath, BOKU_FONT, 32);
		if (!loadResult) throw bokuFontPath;

		// ���[�f�B���O�I��
		CloseLoadScreen();

		return true;
	}
	catch (const char* str) {
		char buff[128] = "/0";
		strcat_s(buff, str);
		strcat_s(buff, " �̓ǂݍ��݂Ɏ��s���܂����B");
		ErrorLog(buff);

		// ���[�f�B���O�I��
		CloseLoadScreen();
		return false;
	}
}

void GameManager::InitializeModel()
{
	// �e�[�u���̏����Ƃ���
	glm::vec3 globalPos = glm::vec3(0.f, -tableModel.box.max.y + 0.3f, 0.f);

	// �e�[�u��
	tableModel.position = globalPos;

	// �X�J�C���f��
	skyboxModel.position = globalPos;

	// �{�[��
	float padding = 2.4f;
	glm::vec3 wallMin = glm::vec3(-tableModel.box.max.x + padding, 0.f, -tableModel.box.max.z + padding);
	glm::vec3 wallMax = glm::vec3(tableModel.box.max.x - padding, 0.f, tableModel.box.max.z - padding);
	for (int i = 0; i <= 15; ++i)
	{
		balls[i].SetWall(wallMin, wallMax);
		balls[i].model.EnableRotate();
	}

	balls[0].model.position = glm::vec3(0.f, 0.f, 0.f);
	float r = balls[0].model.sphere.radius*2.f;
	int k = 1;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5 - i; ++j)
		{
			balls[k].model.position = glm::vec3(9.0f - j*r*sin(M_PI / 3), 0.f, j*r*cos(M_PI / 3) + (i - 2)*r);
			++k;
		}
	}
}

void GameManager::Render2D()
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
	drawText(FpsString, font.list[BOKU_FONT], glm::vec2(20,40), glm::vec2());

#ifdef SHADOW_MAP_ON
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(windowSize.x*0.75f, windowSize.y*0.75f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(windowSize.x, windowSize.y*0.75f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(windowSize.x, windowSize.y);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(windowSize.x*0.75f, windowSize.y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
#endif
}

void GameManager::Render3D()
{
	tableModel.Render();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Render();
	}
	skyboxModel.Render();
}

int glProperties[] = { GL_LIGHTING, GL_CULL_FACE };
void GameManager::RenderWithoutShadow()
{
	// 2D <=> 3D ����ON/OFF����v���p�e�B�p�̃t���O��OFF��
	map<int, bool> propertyFlag;
	int propertyNum = sizeof(glProperties) / sizeof(glProperties[0]);
	for (int i = 0; i<propertyNum; ++i)
	{
		propertyFlag.insert(map<int, bool>::value_type(glProperties[i], false));
	}

	//�@�o�b�N�o�b�t�@���N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�@���f���r���[�s��̐ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//�@���_�̐ݒ�
	camera.Set();

	// �����̈ʒu��ݒ肷��
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glPushMatrix();

	//�@3D�V�[���̕`��
	Render3D();

	//�@3D�@���@2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowSize.x, windowSize.y, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// enable �� �v���p�e�B�� disable ��
	for (map<int, bool>::iterator it = propertyFlag.begin(); it != propertyFlag.end(); it++) {
		if (glIsEnabled(it->first))
		{
			it->second = true;
			glDisable(it->first);
		}
	}

	//�@2D�V�[���̕`��
	Render2D();

	// disable �ɂ����v���p�e�B�� enable ��
	for_each(propertyFlag.begin(), propertyFlag.end(), [](pair<int, bool> elm) {
		if (!glIsEnabled(elm.first))
		{
			if (elm.second) glEnable(elm.first);
		}
	});

	//�@2D�@���@3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

void GameManager::RenderWithShadow()
{
	bool isLighting = false;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble currentModelView[16];

	/* �f�v�X�e�N�X�`���̍쐬�������� */
	glActiveTexture(GL_TEXTURE1);

	// �f�v�X�o�b�t�@���N���A����
	glClear(GL_DEPTH_BUFFER_BIT);

	// �r���[�|�[�g���e�N�X�`���̃T�C�Y�ɐݒ�
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// �����ϊ��s���P�ʍs��ɐݒ�
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// �����ʒu�����_�Ƃ��V�[��������Ɏ��܂�悤���f���r���[�ϊ��s���ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(40.0, (GLdouble)DEPTH_TEX_WIDTH / (GLdouble)DEPTH_TEX_HEIGHT, 1.0, 20.0);
	gluLookAt(lightpos[0], lightpos[1], lightpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	// �f�v�X�o�b�t�@�̓��e�������擾����̂Ńt���[���o�b�t�@�ɂ͏������܂Ȃ�
	// ���������ĉA�e�t�����s�v�Ȃ̂Ń��C�e�B���O���I�t�ɂ���
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);

	// �f�v�X�o�b�t�@�ɂ͔w�ʂ̃|���S���̉��s�����L�^����悤�ɂ���
	glCullFace(GL_FRONT);

	//�@3D�V�[���̕`��
	Render3D();

	// �������ރe�N�X�`���w��
	glBindTexture(GL_TEXTURE_2D, shadowTexture);

	// �f�v�X�o�b�t�@�̓��e���e�N�X�`���������ɓ]��
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// �ʏ�̕`��̐ݒ�ɖ߂�
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	/* �f�v�X�e�N�X�`���̍쐬�����܂� */

	//�@�o�b�N�o�b�t�@���N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�@���f���r���[�s��̐ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//�@���_�̐ݒ�
	camera.Set();

	// �����̈ʒu��ݒ肷��
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* �f�v�X�e�N�X�`���̐ݒ肱������ */
	glActiveTexture(GL_TEXTURE1);

	// ���݂̃��f���r���[�̎擾
	glGetDoublev(GL_MODELVIEW_MATRIX, currentModelView);

	// �e�N�X�`���ϊ��s���ݒ肷��
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// �e�N�X�`�����W�� [-1,1] �͈̔͂� [0,1] �͈̔͂Ɏ��߂�
	glTranslated(0.5, 0.5, 0.5);
	glScaled(0.5, 0.5, 0.5);

	// �e�N�X�`���̃��f���r���[�ϊ��s��Ɠ����ϊ��s��̐ς�������
	glMultMatrixd(modelview);

	// ���݂̃��f���r���[�ϊ��̋t�ϊ��������Ă���
	glMultTransposeMatrixd(currentModelView);

	// ���f���r���[�ϊ��s��ɖ߂�
	glMatrixMode(GL_MODELVIEW);

	glActiveTexture(GL_TEXTURE0);
	/* �f�v�X�e�N�X�`���̐ݒ肱���܂� */

	glPushMatrix();

	/* �e�N�X�`���}�b�s���O�ƃe�N�X�`�����W�̎���������L���ɂ��� */
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glActiveTexture(GL_TEXTURE0);

	// �����̖��邳������̕����ł̖��邳�ɐݒ�
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);

	//�@3D�V�[���̕`��
	Render3D();

	/* �e�N�X�`���}�b�s���O�ƃe�N�X�`�����W�̎��������𖳌��ɂ��� */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	//�@3D�@���@2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowSize.x, windowSize.y, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (glIsEnabled(GL_LIGHTING))
	{
		isLighting = true;
		glDisable(GL_LIGHTING);
	}

	//�@2D�V�[���̕`��
	Render2D();

	if (!glIsEnabled(GL_LIGHTING))
	{
		if (isLighting)
			glEnable(GL_LIGHTING);
	}

	//�@2D�@���@3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

void GameManager::Render()
{
#ifdef SHADOW_MAP_ON
	RenderWithShadow();
#else
	RenderWithoutShadow();
#endif
}

void GameManager::Update()
{
	for (int i = 0; i < 16; ++i)
	{
		balls[i].Move();
		balls[i].TestCollisionWall();
		for (int j = 0; j < 16; ++j)
		{
			if (i != j)
			{
				balls[i].TestCollisionBall(balls[j]);
			}
		}
		balls[i].TestPocket();
		balls[i].UpdateVelocity();
	}
}

void GameManager::KeyFunc(KEY key)
{
	if (status == MAIN)
	{
		switch (key)
		{
		case KEY_UP:
			break;
		case KEY_DOWN:
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
			balls[0].AddVec(glm::vec3(0.25f, 0.f, 0.f));
			break;
		case KEY_BACKSPACE:
			break;
		case KEY_1:
		case KEY_2:
		case KEY_3:
			break;
		}
	}
}

uint GameManager::GetWindowWidth() { return windowSize.x; }
uint GameManager::GetWindowHeight() { return windowSize.y; }
uint GameManager::GetWindowPosX() { return windowPos.x; }
uint GameManager::GetWindowPosY() { return windowPos.y; }

void GameManager::SetWindowSize(int x, int y)
{
	//�@�E�B���h�E�T�C�Y��ۑ�
	windowSize.x = x;
	windowSize.y = y;

	//�@�T�C�Y�`�F�b�N
	if (windowSize.x < 1) windowSize.x = 1;
	if (windowSize.y < 1) windowSize.y = 1;

	//�@�r���[�|�[�g�̐ݒ�
	glViewport(0, 0, windowSize.x, windowSize.y);

	//�@�ˉe�s��̐ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 1000.0);
}

void DrawSmallCircle(float radius, int x, int y)
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

void DrawLargeCircle(float radius, int x, int y)
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

void glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move)
{
	unsigned int textLength;	//�����Ŏ󂯎�����e�L�X�g�̒���
	WCHAR * unicodeText;		//text��UNICODE�ɕϊ�������������i�[����
	GLuint listbaseIdx;		//�f�B�X�v���C���X�g�̍ŏ��̃C���f�b�N�X

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

	HDC hdc = wglGetCurrentDC();
	SelectObject(hdc, hfont);

	//���������̃f�B�X�v���C���X�g���m�ۂ��A�f�B�X�v���C���X�g�̍ŏ��̃C���f�b�N�X���擾
	listbaseIdx = glGenLists(textLength);

	GLYPHMETRICSFLOAT agmf;
	for (unsigned int textCnt = 0; textCnt < textLength; ++textCnt)
	{
		if (wglUseFontBitmapsW(hdc, unicodeText[textCnt], 1, listbaseIdx + textCnt) == FALSE)
		//if (wglUseFontOutlines(hdc, unicodeText[textCnt], 1, listbaseIdx + textCnt, 0.f, 1.f, WGL_FONT_POLYGONS, &agmf) == FALSE)
		{
			//MessageBox(hwnd, "wglUseFontBitmaps() Error!!", "wgl Error", MB_OK);
		}
	}

	//1�����`�悵���當������bit���炷��
	glBitmap(0, 0, 0, 0, move.x, move.y, NULL);

	//�f�B�X�v���C���X�g�����s����
	for (unsigned int textCnt = 0; textCnt < textLength; textCnt++)
	{
		glCallList(listbaseIdx + (GLuint)textCnt);
	}

	delete[] unicodeText;
	glDeleteLists(listbaseIdx, textLength);

	return 1;
}

// ����
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize)
{
	// ������`��
	glColor4f(mainColor.r, mainColor.g, mainColor.b, mainColor.a);
	drawText(text, hfont, move, pos);

	// �����
	if (edgeSize > 0)
	{
		glColor4f(edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
		for (int i = -edgeSize + 1; i<edgeSize; i++)
		{
			drawText(text, hfont, move, glm::vec2(pos.x + i, pos.y + (edgeSize - i)));
			drawText(text, hfont, move, glm::vec2(pos.x + i, pos.y - (edgeSize - i)));
		}
		drawText(text, hfont, move, glm::vec2(pos.x + edgeSize, pos.y));
		drawText(text, hfont, move, glm::vec2(pos.x - edgeSize, pos.y));
	}

	return 1;
}