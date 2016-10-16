#include "stdafx.h"
#include "Font.h"

void Font::ErrorLog(char* msg)
{
	cout << "Font Error : " << msg << endl;
}

bool Font::isLoaded(const char* fileName)
{
	bool loadedFlag = false;
	for (int i = 0; i < fileList.size(); ++i)
	{
		if (fileList[i] == fileName) {
			loadedFlag = true;
			break;
		}
	}
	return loadedFlag;
}

bool Font::Load(const char* fileName, const char* fontName, int fontSize)
{
	
	if (!isLoaded(fileName))
	{
		FTPixmapFont* newFont = new FTPixmapFont(fileName);
		if (newFont->Error())
		{
			delete newFont;
			ErrorLog("failed to load font file");
			return false;
		}
		else
		{
			newFont->FaceSize(fontSize);
			fileList.push_back(fileName);
			list.insert(map<string, FTPixmapFont*>::value_type(fontName, newFont));
		}
	}
	return true;
}

void Font::Release()
{
	for_each(list.begin(), list.end(), [](pair<string, FTPixmapFont*> elm) {
		delete elm.second;
	});
	list.clear();
	fileList.clear();
}

void Font::SetSize(string name, int size)
{
	list[name]->FaceSize(size);
}
