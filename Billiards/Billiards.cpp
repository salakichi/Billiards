#include "stdafx.h"
#include "MyFreeglut.h"

void display(void)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

