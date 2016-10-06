#include "stdafx.h"
#include "XLoader.h"
#include "Mouse.h"

int WindowWidth = 512;
int WindowHeight = 512;
XModel model;
ViewCamera camera;

const char* testModelPath = "data\\table.x";

char FpsString[50] = { 0 };
double CurrentTime = 0.0;
double LastTime = 0.0;
double CurrentCount = 0.0;
double LastCount = 0.0;
int FrameCount = 0;
double Fps = 0.0f;

void Initialize();
void Shutdown();
void Display();
void Idle();
void Reshape(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Render3D();
void glutRenderText(void* bitmapfont, char* text);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("X�t�@�C���`��e�X�g");
	if (glewInit() != GLEW_OK)
	{
		cout << "glew�̏������Ɏ��s���܂����B" << endl;
		return -1;
	}

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	Initialize();

	glutMainLoop();

	Shutdown();

	return 0;
}

double genfunc[][4] = {
	{ 1.0, 0.0, 0.0, 0.0 }, // s
	{ 0.0, 1.0, 0.0, 0.0 }, // t
	{ 0.0, 0.0, 1.0, 0.0 }, // r
	{ 0.0, 0.0, 0.0, 1.0 }, // q
};

void Initialize()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//�@�J�E���^�[�̏�����
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;

	//�@���f���t�@�C���̓ǂݍ���
	model.Load((char*)testModelPath);

	//�@�o�b�N�o�b�t�@���N���A����F�̎w��
	glClearColor(0.3f, 0.3f, 1.0f, 1.0f);

	//�@�[�x�e�X�gON
	glEnable(GL_DEPTH_TEST);

	//�@���C�e�B���O
	float pos[4] = { 0.0f, 10.0f, 10.0f, 1.0f };
	float amb[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float dif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spe[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

	//�@�E�B���h�E��\��
	glutShowWindow();
}

void Idle()
{
	glutPostRedisplay();
}

void Reshape(int x, int y)
{
	//�@�E�B���h�E�T�C�Y��ۑ�
	WindowWidth = x;
	WindowHeight = y;

	//�@�T�C�Y�`�F�b�N
	if (WindowWidth < 1) WindowWidth = 1;
	if (WindowHeight < 1) WindowHeight = 1;

	//�@�r���[�|�[�g�̐ݒ�
	glViewport(0, 0, WindowWidth, WindowHeight);

	//�@�ˉe�s��̐ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 1000.0);
}

void Render2D()
{
	bool isLighting = false;

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

	//�@3D�@���@2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WindowWidth, WindowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (glIsEnabled(GL_LIGHTING))
	{
		isLighting = true;
		glDisable(GL_LIGHTING);
	}

	//�@�����̕`��
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glRasterPos2i(15, 15);
	glutRenderText(GLUT_BITMAP_HELVETICA_12, FpsString);

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
}

void Render3D()
{
	model.Render();
}

void Display()
{
	//�@�o�b�N�o�b�t�@���N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�@���f���r���[�s��̐ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//�@���_�̐ݒ�
	camera.Set();

	glPushMatrix();

	//�@3D�V�[���̕`��
	Render3D();

	//�@2D�V�[���̕`��
	Render2D();

	glPopMatrix();

	//�@�_�u���o�b�t�@
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

void Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}

void Shutdown()
{
	model.Release();
}

void glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

