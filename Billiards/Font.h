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
	vector<string> fileList;
	void ErrorLog(char* msg);
	bool isLoaded(const char* fileName);
public:
	map<string, FTPixmapFont*> list;
	bool Load(const char* fileName, const char* fontName, int fontSize = DEFAULT_FONT_SIZE);
	void Release();
	void SetSize(string name, int size);
};