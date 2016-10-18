#include "stdafx.h"
#include "Font.h"

void Font::ErrorLog(char* msg)
{
	cout << "Font Error : " << msg << endl;
}


bool Font::Load(const char* fileName,  int fontSize)
{
	
	data = new FTPixmapFont(fileName);
	if (data->Error())
	{
		delete data;
		ErrorLog("failed to load font file");
		return false;
	}
	else
	{
		data->FaceSize(fontSize);
	}
	return true;
}

void Font::Release()
{
	delete data;
}

void Font::SetSize(int size)
{
	data->FaceSize(size);
}

void Font::Render(wchar_t* text)
{
	data->Render(text);
}
