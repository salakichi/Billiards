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
void KeyboardUp(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void SpecialUp(int key, int x, int y);

int main(int argc, char **argv)
{
	if (!gm.Initialize(argc, argv))
		return -1;

	glutIgnoreKeyRepeat(GL_TRUE); // キーの繰り返し入力を無効にする
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	
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
		gm.Keyboard(KEY_W);
		break;
	case 97:	// A
		gm.Keyboard(KEY_A);
		break;
	case 115:	// S
		gm.Keyboard(KEY_S);
		break;
	case 100:	// D
		gm.Keyboard(KEY_D);
		break;
	case 118:	// V
		gm.Keyboard(KEY_V);
		break;
	case 32:	// space
		gm.Keyboard(KEY_SPACE);
		break;
	case 13:	// Enter
		gm.Keyboard(KEY_ENTER);
		break;
	case 8:	    // Backspace
		gm.Keyboard(KEY_BACKSPACE);
		break;
	case 49:	// 1
		gm.Keyboard(KEY_1);
		break;
	case 50:	// 2
		gm.Keyboard(KEY_2);
		break;
	case 51:	// 3
		gm.Keyboard(KEY_3);
		break;
	case 52:	// 4
		gm.Keyboard(KEY_4);
		break;
	case 53:	// 5
		gm.Keyboard(KEY_5);
		break;
	default:
		cout << (int)key << endl;
		break;
	}
}

void KeyboardUp(unsigned char key, int x, int y)
{
	switch ((int)key)
	{
	case 243:	// Esc
		exit(0);
		break;
	case 119:	// W
		gm.KeyboardUp(KEY_W);
		break;
	case 97:	// A
		gm.KeyboardUp(KEY_A);
		break;
	case 115:	// S
		gm.KeyboardUp(KEY_S);
		break;
	case 100:	// D
		gm.KeyboardUp(KEY_D);
		break;
	case 118:	// V
		gm.KeyboardUp(KEY_V);
		break;
	case 32:	// space
		gm.KeyboardUp(KEY_SPACE);
		break;
	case 13:	// Enter
		gm.KeyboardUp(KEY_ENTER);
		break;
	case 8:	    // Backspace
		gm.KeyboardUp(KEY_BACKSPACE);
		break;
	case 49:	// 1
		gm.KeyboardUp(KEY_1);
		break;
	case 50:	// 2
		gm.KeyboardUp(KEY_2);
		break;
	case 51:	// 3
		gm.KeyboardUp(KEY_3);
		break;
	case 52:	// 4
		gm.KeyboardUp(KEY_4);
		break;
	case 53:	// 5
		gm.KeyboardUp(KEY_5);
		break;
	default:
		cout << (int)key << endl;
		break;
	}
}

void Special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		gm.Keyboard(KEY_LEFT);
		break;
	case GLUT_KEY_UP:
		gm.Keyboard(KEY_UP);
		break;
	case GLUT_KEY_RIGHT:
		gm.Keyboard(KEY_RIGHT);
		break;
	case GLUT_KEY_DOWN:
		gm.Keyboard(KEY_DOWN);
		break;
	default:
		break;
	}
}

void SpecialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		gm.KeyboardUp(KEY_LEFT);
		break;
	case GLUT_KEY_UP:
		gm.KeyboardUp(KEY_UP);
		break;
	case GLUT_KEY_RIGHT:
		gm.KeyboardUp(KEY_RIGHT);
		break;
	case GLUT_KEY_DOWN:
		gm.KeyboardUp(KEY_DOWN);
		break;
	default:
		break;
	}
}

void Shutdown()
{
	gm.Release();
}



