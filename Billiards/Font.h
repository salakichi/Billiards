#pragma once

#include <Windows.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
using namespace::std;

class Font
{
private:
	vector<string> fileList;
	void ErrorLog(char* msg);
	bool isLoaded(const char* fileName);
public:
	map<string, HFONT> list;
	bool Load(const char* fileName, const char* fontName, int fontSize);
	void Release();
};