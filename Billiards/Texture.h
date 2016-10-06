#pragma once

#include"lodepng.h"
#include"TgaReader.h"
#include"MyFreeglut.h"
#include"MyOpenCV.h"
#include<Windows.h>
#include<iostream>
using namespace std;

inline bool fileExists(char* name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

typedef unsigned char byte;

class Texture
{
private:
	int width, height, nChannels;
	GLubyte *image;
	char filepath[128];
public:
	Texture();
	~Texture();
	bool Load(char* path);
	bool CreateTexture(GLuint* imageTex);
	void Release();
};