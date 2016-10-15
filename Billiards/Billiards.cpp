#include "stdafx.h"
#include "GameManager.h"

GameManager gm;

void Shutdown();
void Display();
void Idle();
void Reshape(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);

int main(int argc, char **argv)
{
	if (!gm.Initialize(argc, argv))
		return -1;

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	
	//　ウィンドウを表示
	glutShowWindow();

	glutMainLoop();

	Shutdown();

	return 0;
}

void Idle()
{
	glutPostRedisplay();
}

void Reshape(int x, int y)
{
	gm.SetWindowSize(x, y);
}

void Display()
{
	gm.Update();

	gm.Render();

	//　ダブルバッファ
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	gm.Mouse(button, state, x, y);
}

void Motion(int x, int y)
{
	gm.Motion(x, y);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch ((int)key)
	{
	case 243:	// Esc
		exit(0);
		break;
	case 119:	// W
		gm.KeyFunc(KEY_W);
		break;
	case 97:	// A
		gm.KeyFunc(KEY_A);
		break;
	case 115:	// S
		gm.KeyFunc(KEY_S);
		break;
	case 100:	// D
		gm.KeyFunc(KEY_D);
		break;
	case 118:	// V
		gm.KeyFunc(KEY_V);
		break;
	case 32:	// space
		gm.KeyFunc(KEY_SPACE);
		break;
	case 13:	// Enter
		gm.KeyFunc(KEY_ENTER);
		break;
	case 8:	// Enter
		gm.KeyFunc(KEY_BACKSPACE);
		break;
	case 49:	// 1
		gm.KeyFunc(KEY_1);
		break;
	case 50:	// 2
		gm.KeyFunc(KEY_2);
		break;
	case 51:	// 3
		gm.KeyFunc(KEY_3);
		break;
	case 52:	// 3
		gm.KeyFunc(KEY_4);
		break;
	case 53:	// 3
		gm.KeyFunc(KEY_5);
		break;

	default:
		cout << (int)key << endl;
		break;
	}
}

void Shutdown()
{
	gm.Release();
}



