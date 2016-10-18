#pragma once

#include "MyFtgl.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#define DEFAULT_FONT_SIZE 32

class Font
{
private:
	FTPixmapFont* data;
	void ErrorLog(char* msg);
public:
	bool Load(const char* fileName, int fontSize = DEFAULT_FONT_SIZE);
	void Release();
	void SetSize(int size);
	void Render(wchar_t* text);
};