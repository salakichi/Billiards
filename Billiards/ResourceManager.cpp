#include "stdafx.h"
#include "ResourceManager.h"

bool loadFinishFlag = false;

ResourceManager::ResourceManager()
{
	// スレッド関連の初期化
	mutex = NULL;
	thread = NULL;
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::ErrorLog(char* msg)
{
	cout << "ResourceManager Error : " << msg << endl;
}

unsigned __stdcall LoadingScreen(void* pArguments)
{
	cv::Mat img(WINDOW_DEFAULT_H, WINDOW_DEFAULT_W, CV_8UC3);

	const char* text[4] = { "Loading   ", "Loading.  ", "Loading.. ", "Loading..." };
	const string windowTitle("Loading");
	cv::namedWindow(windowTitle, CV_WINDOW_AUTOSIZE);

	int i = 0;

	while (1) {
		DWORD start, end;
		start = timeGetTime();

		cvMoveWindow(windowTitle.c_str(), WINDOW_DEFAULT_POS_X, WINDOW_DEFAULT_POS_Y);

		img = cv::Scalar(255, 255, 255);
		cv::putText(img, text[i / 10], cv::Point(WINDOW_DEFAULT_W - 325, WINDOW_DEFAULT_H - 75), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2.0, cv::Scalar(0, 0, 0), 2, CV_AA);

		cv::imshow(windowTitle, img);

		end = timeGetTime();
		if (end - start<66)
			cvWaitKey(66 - (end - start));

		void* handle = cvGetWindowHandle(windowTitle.c_str());
		if (loadFinishFlag || handle == NULL)
		{
			loadFinishFlag = true;
			break;
		}

		i++;
		if (i == 40)
			i = 0;
	}
	cv::destroyWindow(windowTitle);

	return 0;
}

void ResourceManager::DrawLoadScreen()
{
	loadFinishFlag = false;
	mutex = CreateMutex(NULL, FALSE, NULL);
	thread = (HANDLE)_beginthreadex(NULL, 0, &LoadingScreen, NULL, 0, NULL);
}

void ResourceManager::CloseLoadScreen()
{
	WaitForSingleObject(mutex, INFINITE);
	loadFinishFlag = true;
	ReleaseMutex(mutex);
	WaitForSingleObject(thread, INFINITE);

	CloseHandle(mutex);
	CloseHandle(thread);
}

bool ResourceManager::Load()
{
	bool loadResult = true;

	try {
		// ローディング表示
		DrawLoadScreen();

		//　モデルファイルの読み込み
		XModel* tableModel = new XModel();
		loadResult = tableModel->Load(TABLE_MODEL);
		if (!loadResult) throw TABLE_MODEL;
		modelList.insert(map<string, XModel*>::value_type(TABLE_MODEL, tableModel));

		XModel* stageModel = new XModel();
		loadResult = stageModel->Load(STAGE_MODEL, 20.f);
		if (!loadResult) throw STAGE_MODEL;
		modelList.insert(map<string, XModel*>::value_type(STAGE_MODEL, stageModel));

		char ballFilename[256];
		for (int i = 0; i <= 15; ++i)
		{
			Ball* ball = new Ball();
			sprintf_s(ballFilename, BALL_MODEL, i);
			loadResult = ball->model.Load(ballFilename);
			if (!loadResult) throw ballFilename;
			balls[i] = ball;
		}

		// フォントファイルの読み込み
		Font* misakiFont = new Font();
		loadResult = misakiFont->Load(MISAKI_FONT, 32);
		if (!loadResult) throw MISAKI_FONT;
		fontList.insert(map<string, Font*>::value_type(MISAKI_FONT, misakiFont));

		Font* bokuFont = new Font();
		loadResult = bokuFont->Load(BOKU_FONT, 32);
		if (!loadResult) throw BOKU_FONT;
		fontList.insert(map<string, Font*>::value_type(BOKU_FONT, bokuFont));

		// BGMの読み込み
		Sound* titleBgm = new Sound();
		loadResult = titleBgm->LoadWave(TITLE_BGM, 0.25f, true);
		if (!loadResult) throw TITLE_BGM;
		soundList.insert(map<string, Sound*>::value_type(TITLE_BGM, titleBgm));

		// ローディング終了
		CloseLoadScreen();

		return true;
	}
	catch (const char* str) {
		char buff[128];
		sprintf_s(buff, "%s の読み込みに失敗しました。", str);
		ErrorLog(buff);

		// ローディング終了
		CloseLoadScreen();

		getchar();
		return false;
	}
}

void ResourceManager::Release()
{
	// モデルの解放
	for (auto model : modelList)
	{
		model.second->Release();
		delete model.second;
	}
	for (int i = 0; i <= 16; ++i)
	{
		balls[i]->model.Release();
		delete balls[i];
	}

	// フォントの解放
	for (auto font : fontList)
	{
		font.second->Release();
		delete font.second;
	}

	// bgmの解放
	for (auto sound : soundList)
	{
		sound.second->Release();
		delete sound.second;
	}
}