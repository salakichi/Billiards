#include "stdafx.h"
#include "GameManager.h"

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
	if (!GameManager::Instance().Initialize(argc, argv))
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
	GameManager::Instance().SetWindowSize(x, y);
}

void Display()
{
	GameManager::Instance().Update();

	GameManager::Instance().Render();

	//　ダブルバッファ
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	GameManager::Instance().Mouse(button, state, x, y);
}

void Motion(int x, int y)
{
	GameManager::Instance().Motion(x, y);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch ((int)key)
	{
	case 243:	// Esc
		exit(0);
		break;
	case 119:	// W
		GameManager::Instance().Keyboard(KEY_W);
		break;
	case 97:	// A
		GameManager::Instance().Keyboard(KEY_A);
		break;
	case 115:	// S
		GameManager::Instance().Keyboard(KEY_S);
		break;
	case 100:	// D
		GameManager::Instance().Keyboard(KEY_D);
		break;
	case 118:	// V
		GameManager::Instance().Keyboard(KEY_V);
		break;
	case 32:	// space
		GameManager::Instance().Keyboard(KEY_SPACE);
		break;
	case 13:	// Enter
		GameManager::Instance().Keyboard(KEY_ENTER);
		break;
	case 8:	    // Backspace
		GameManager::Instance().Keyboard(KEY_BACKSPACE);
		break;
	case 49:	// 1
		GameManager::Instance().Keyboard(KEY_1);
		break;
	case 50:	// 2
		GameManager::Instance().Keyboard(KEY_2);
		break;
	case 51:	// 3
		GameManager::Instance().Keyboard(KEY_3);
		break;
	case 52:	// 4
		GameManager::Instance().Keyboard(KEY_4);
		break;
	case 53:	// 5
		GameManager::Instance().Keyboard(KEY_5);
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
		GameManager::Instance().KeyboardUp(KEY_W);
		break;
	case 97:	// A
		GameManager::Instance().KeyboardUp(KEY_A);
		break;
	case 115:	// S
		GameManager::Instance().KeyboardUp(KEY_S);
		break;
	case 100:	// D
		GameManager::Instance().KeyboardUp(KEY_D);
		break;
	case 118:	// V
		GameManager::Instance().KeyboardUp(KEY_V);
		break;
	case 32:	// space
		GameManager::Instance().KeyboardUp(KEY_SPACE);
		break;
	case 13:	// Enter
		GameManager::Instance().KeyboardUp(KEY_ENTER);
		break;
	case 8:	    // Backspace
		GameManager::Instance().KeyboardUp(KEY_BACKSPACE);
		break;
	case 49:	// 1
		GameManager::Instance().KeyboardUp(KEY_1);
		break;
	case 50:	// 2
		GameManager::Instance().KeyboardUp(KEY_2);
		break;
	case 51:	// 3
		GameManager::Instance().KeyboardUp(KEY_3);
		break;
	case 52:	// 4
		GameManager::Instance().KeyboardUp(KEY_4);
		break;
	case 53:	// 5
		GameManager::Instance().KeyboardUp(KEY_5);
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
		GameManager::Instance().Keyboard(KEY_LEFT);
		break;
	case GLUT_KEY_UP:
		GameManager::Instance().Keyboard(KEY_UP);
		break;
	case GLUT_KEY_RIGHT:
		GameManager::Instance().Keyboard(KEY_RIGHT);
		break;
	case GLUT_KEY_DOWN:
		GameManager::Instance().Keyboard(KEY_DOWN);
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
		GameManager::Instance().KeyboardUp(KEY_LEFT);
		break;
	case GLUT_KEY_UP:
		GameManager::Instance().KeyboardUp(KEY_UP);
		break;
	case GLUT_KEY_RIGHT:
		GameManager::Instance().KeyboardUp(KEY_RIGHT);
		break;
	case GLUT_KEY_DOWN:
		GameManager::Instance().KeyboardUp(KEY_DOWN);
		break;
	default:
		break;
	}
}

void Shutdown()
{
	GameManager::Instance().Release();
}



