#include "stdafx.h"
#include "Font.h"

void Font::ErrorLog(char* msg)
{
	cout << "GameManager Error : " << msg << endl;
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
	if (!isLoaded(fileName)) {
		if (AddFontResourceEx(fileName, FR_PRIVATE, NULL) <= 0)
		{
			cout << "Font Error : Init() failed to read font\\bokutachi.otf" << endl;
			return false;
		}
	}
	
	fileList.push_back(fileName);
	HFONT newFont = CreateFont(fontSize, 0, 0, 0,
		FW_REGULAR, FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY | PROOF_QUALITY, DEFAULT_PITCH | OUT_DEFAULT_PRECIS,
		fontName);
	list.insert(map<string, HFONT>::value_type(fontName, newFont));
}

void Font::Release()
{
	for_each(list.begin(), list.end(), [](pair<string, HFONT> elm) {
		DeleteObject(elm.second);
	});
	for_each(fileList.begin(), fileList.end(), [](string elm) {
		RemoveFontResourceEx(elm.c_str(), FR_PRIVATE, NULL);
	});
}