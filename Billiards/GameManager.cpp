#include "stdafx.h"
#include "GameManager.h"

const char* tableModelPath = "data\\table.x";
const char* ballModelPath = "data\\%d.x";
const char* skyboxModelPath = "data\\room\\松野家居間_ダンス用.x";
const char* misakiFontPath = "data\\font\\misaki_gothic.ttf";
const char* bokuFontPath = "data\\font\\bokutachi.otf";
#define MISAKI_FONT "美咲フォント"
#define BOKU_FONT "ぼくたちのゴシック"

static const GLfloat lightpos[] = { 0.0f, 5.0f, 0.0f, 1.f }; /* 位置　　　　　　　 */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.f }; /* 直接光強度　　　　 */
static const GLfloat lightdim[] = { 0.2f, 0.2f, 0.2f, 1.f }; /* 影内の拡散反射強度 */
static const GLfloat lightblk[] = { 0.0f, 0.0f, 0.0f, 1.f }; /* 影内の鏡面反射強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.f }; /* 環境光強度　　　　 */

#define DEPTH_TEX_WIDTH  512
#define DEPTH_TEX_HEIGHT 512
//#define SHADOW_MAP_ON

glm::uvec2 windowPos;
glm::uvec2 windowSize;
bool loadFinishFlag = false;
char FpsString[50] = { 0 };

void glutRenderText(void* bitmapfont, char*text);
void DrawSmallCircle(float radius, int x, int y);		// 半径32.0まで 品質：良
void DrawLargeCircle(float radius, int x, int y);		// 半径制限なし 品質：悪
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move);
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize);

GameManager::GameManager()
{
	// スレッド関連の初期化
	mutex = NULL;
	thread = NULL;

	status = TITLE;

	//　カウンターの初期化
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;
	CurrentTime = 0.0;
	LastTime = 0.0;
	LastCount = 0.0;
	Fps = 0.0f;

	windowSize.x = WINDOW_DEFAULT_W;
	windowSize.y = WINDOW_DEFAULT_H;
	windowPos.x = WINDOW_DEFAULT_POS_X;
	windowPos.y = WINDOW_DEFAULT_POS_Y;
}

GameManager::~GameManager()
{
}

void GameManager::ErrorLog(char* msg)
{
	cout << "GameManager Error : " << msg << endl;
}

bool GameManager::Initialize(int argc, char** argv)
{
	if (!InitializeGL(argc, argv))
	{
		ErrorLog("Initialize() : failed to initialize GL");
		return false;
	}

	if (!LoadMaterial())
	{
		ErrorLog("Initialize() : failed to load material data");
		return false;
	}
	InitializeModel();

	return true;
}

void GameManager::Release()
{
	tableModel.Release();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Release();
	}
	skyboxModel.Release();
}

bool GameManager::InitializeGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(windowPos.x, windowPos.y);
	glutInitWindowSize(windowSize.x, windowSize.y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("ビリヤード");
	if (glewInit() != GLEW_OK)
	{
		ErrorLog("InitializeGL() : glewの初期化に失敗しました。");
		return false;
	}

	// 通常のテクスチャの設定
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//　バックバッファをクリアする色の指定
	//glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
	glClearColor(1.f, 1.f, 1.0f, 1.0f);

	// カリングできるようにする
	glEnable(GL_CULL_FACE);

	//　深度テストON
	glEnable(GL_DEPTH_TEST);

	//　ライティング
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

#ifdef SHADOW_MAP_ON
	glActiveTexture(GL_TEXTURE1);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);

	/* テクスチャの割り当て */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/* テクスチャの繰り返し方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	/* もしＲの値がテクスチャの値以下なら真（つまり日向） */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	/* 比較の結果を輝度値として得る */
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	/* テクスチャユニット7のテクスチャ環境 */
	/*
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	static const GLfloat blend[] = { 1.0, 1.0, 1.0, 0.5 };
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blend);
	*/
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* テクスチャ座標に視点座標系における物体の座標値を用いる */
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	/* 生成したテクスチャ座標をそのまま (S, T, R, Q) に使う */
	static const GLdouble genfunc[][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 },
	};
	glTexGendv(GL_S, GL_EYE_PLANE, genfunc[0]);
	glTexGendv(GL_T, GL_EYE_PLANE, genfunc[1]);
	glTexGendv(GL_R, GL_EYE_PLANE, genfunc[2]);
	glTexGendv(GL_Q, GL_EYE_PLANE, genfunc[3]);

	glDisable(GL_TEXTURE_2D);

	// 通常のテクスチャに戻す
	glActiveTexture(GL_TEXTURE0);
#endif

	return true;
}

unsigned __stdcall LoadingScreen(void* pArguments)
{
	cv::Mat img((int)windowSize.y, (int)windowSize.x, CV_8UC3);

	const char* text[4] = { "Loading   ", "Loading.  ", "Loading.. ", "Loading..." };
	const string windowTitle("Loading");
	cv::namedWindow(windowTitle, CV_WINDOW_AUTOSIZE);

	int i = 0;

	while (1) {
		DWORD start, end;
		start = timeGetTime();

		cvMoveWindow(windowTitle.c_str(), (int)windowPos.x, (int)windowPos.y);

		img = cv::Scalar(255, 255, 255);
		cv::putText(img, text[i / 10], cv::Point((int)windowSize.x - 325, (int)windowSize.y - 75), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2.0, cv::Scalar(0, 0, 0), 2, CV_AA);

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

void GameManager::DrawLoadScreen()
{
	loadFinishFlag = false;
	mutex = CreateMutex(NULL, FALSE, NULL);
	thread = (HANDLE)_beginthreadex(NULL, 0, &LoadingScreen, NULL, 0, NULL);
}

void GameManager::CloseLoadScreen()
{
	WaitForSingleObject(mutex, INFINITE);
	loadFinishFlag = true;
	ReleaseMutex(mutex);
	WaitForSingleObject(thread, INFINITE);

	CloseHandle(mutex);
	CloseHandle(thread);
}

bool GameManager::LoadMaterial()
{
	bool loadResult = true;

	try {
		// ローディング表示
		DrawLoadScreen();

		//　モデルファイルの読み込み
		loadResult = tableModel.Load((char*)tableModelPath);
		if (!loadResult) throw tableModelPath;

		loadResult = skyboxModel.Load((char*)skyboxModelPath, 20.f);
		if (!loadResult) throw skyboxModelPath;

		char ballFilename[256];
		for (int i = 0; i <= 15; ++i)
		{
			sprintf_s(ballFilename, ballModelPath, i);
			loadResult = balls[i].model.Load(ballFilename);
			if (!loadResult) throw ballFilename;
		}

		// フォントファイルの読み込み
		loadResult = font.Load(misakiFontPath, MISAKI_FONT, 32);
		if (!loadResult) throw misakiFontPath;

		loadResult = font.Load(bokuFontPath, BOKU_FONT, 32);
		if (!loadResult) throw bokuFontPath;

		// ローディング終了
		CloseLoadScreen();

		return true;
	}
	catch (const char* str) {
		char buff[128] = "/0";
		strcat_s(buff, str);
		strcat_s(buff, " の読み込みに失敗しました。");
		ErrorLog(buff);

		// ローディング終了
		CloseLoadScreen();
		return false;
	}
}

void GameManager::InitializeModel()
{
	// テーブルの上を基準とする
	glm::vec3 globalPos = glm::vec3(0.f, -tableModel.box.max.y + 0.3f, 0.f);

	// テーブル
	tableModel.position = globalPos;

	// スカイモデル
	skyboxModel.position = globalPos;

	// ボール
	float padding = 2.4f;
	glm::vec3 wallMin = glm::vec3(-tableModel.box.max.x + padding, 0.f, -tableModel.box.max.z + padding);
	glm::vec3 wallMax = glm::vec3(tableModel.box.max.x - padding, 0.f, tableModel.box.max.z - padding);
	for (int i = 0; i <= 15; ++i)
	{
		balls[i].SetWall(wallMin, wallMax);
		balls[i].model.EnableRotate();
	}

	balls[0].model.position = glm::vec3(0.f, 0.f, 0.f);
	float r = balls[0].model.sphere.radius*2.f;
	int k = 1;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5 - i; ++j)
		{
			balls[k].model.position = glm::vec3(9.0f - j*r*sin(M_PI / 3), 0.f, j*r*cos(M_PI / 3) + (i - 2)*r);
			++k;
		}
	}
}

void GameManager::Render2D()
{
	//　時間計測とFPS算出
	CurrentCount = glutGet(GLUT_ELAPSED_TIME);
	CurrentTime = (CurrentCount - LastCount) / 1000.0;
	FrameCount++;
	if ((CurrentTime - LastTime) >= 1.0)
	{
		Fps = FrameCount / (CurrentTime - LastTime);
		sprintf_s(FpsString, "%.3f FPS", Fps);
		FrameCount = 0;
		LastTime = CurrentTime;
	}

	//　文字の描画
	drawText(FpsString, font.list[BOKU_FONT], glm::vec2(20,40), glm::vec2());

#ifdef SHADOW_MAP_ON
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(windowSize.x*0.75f, windowSize.y*0.75f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(windowSize.x, windowSize.y*0.75f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(windowSize.x, windowSize.y);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(windowSize.x*0.75f, windowSize.y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
#endif
}

void GameManager::Render3D()
{
	tableModel.Render();
	for (int i = 0; i <= 16; ++i)
	{
		balls[i].model.Render();
	}
	skyboxModel.Render();
}

int glProperties[] = { GL_LIGHTING, GL_CULL_FACE };
void GameManager::RenderWithoutShadow()
{
	// 2D <=> 3D 時にON/OFFするプロパティ用のフラグをOFFに
	map<int, bool> propertyFlag;
	int propertyNum = sizeof(glProperties) / sizeof(glProperties[0]);
	for (int i = 0; i<propertyNum; ++i)
	{
		propertyFlag.insert(map<int, bool>::value_type(glProperties[i], false));
	}

	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//　視点の設定
	camera.Set();

	// 光源の位置を設定する
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glPushMatrix();

	//　3Dシーンの描画
	Render3D();

	//　3D　→　2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowSize.x, windowSize.y, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// enable の プロパティを disable に
	for (map<int, bool>::iterator it = propertyFlag.begin(); it != propertyFlag.end(); it++) {
		if (glIsEnabled(it->first))
		{
			it->second = true;
			glDisable(it->first);
		}
	}

	//　2Dシーンの描画
	Render2D();

	// disable にしたプロパティを enable に
	for_each(propertyFlag.begin(), propertyFlag.end(), [](pair<int, bool> elm) {
		if (!glIsEnabled(elm.first))
		{
			if (elm.second) glEnable(elm.first);
		}
	});

	//　2D　→　3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

void GameManager::RenderWithShadow()
{
	bool isLighting = false;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble currentModelView[16];

	/* デプステクスチャの作成ここから */
	glActiveTexture(GL_TEXTURE1);

	// デプスバッファをクリアする
	glClear(GL_DEPTH_BUFFER_BIT);

	// ビューポートをテクスチャのサイズに設定
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// 透視変換行列を単位行列に設定
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 光源位置を視点としシーンが視野に収まるようモデルビュー変換行列を設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(40.0, (GLdouble)DEPTH_TEX_WIDTH / (GLdouble)DEPTH_TEX_HEIGHT, 1.0, 20.0);
	gluLookAt(lightpos[0], lightpos[1], lightpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	// デプスバッファの内容だけを取得するのでフレームバッファには書き込まない
	// したがって陰影付けも不要なのでライティングをオフにする
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_LIGHTING);

	// デプスバッファには背面のポリゴンの奥行きを記録するようにする
	glCullFace(GL_FRONT);

	//　3Dシーンの描画
	Render3D();

	// 書き込むテクスチャ指定
	glBindTexture(GL_TEXTURE_2D, shadowTexture);

	// デプスバッファの内容をテクスチャメモリに転送
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, DEPTH_TEX_WIDTH, DEPTH_TEX_HEIGHT);

	// 通常の描画の設定に戻す
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_LIGHTING);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	/* デプステクスチャの作成ここまで */

	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//　視点の設定
	camera.Set();

	// 光源の位置を設定する
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* デプステクスチャの設定ここから */
	glActiveTexture(GL_TEXTURE1);

	// 現在のモデルビューの取得
	glGetDoublev(GL_MODELVIEW_MATRIX, currentModelView);

	// テクスチャ変換行列を設定する
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// テクスチャ座標の [-1,1] の範囲を [0,1] の範囲に収める
	glTranslated(0.5, 0.5, 0.5);
	glScaled(0.5, 0.5, 0.5);

	// テクスチャのモデルビュー変換行列と透視変換行列の積をかける
	glMultMatrixd(modelview);

	// 現在のモデルビュー変換の逆変換をかけておく
	glMultTransposeMatrixd(currentModelView);

	// モデルビュー変換行列に戻す
	glMatrixMode(GL_MODELVIEW);

	glActiveTexture(GL_TEXTURE0);
	/* デプステクスチャの設定ここまで */

	glPushMatrix();

	/* テクスチャマッピングとテクスチャ座標の自動生成を有効にする */
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glActiveTexture(GL_TEXTURE0);

	// 光源の明るさを日向の部分での明るさに設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);

	//　3Dシーンの描画
	Render3D();

	/* テクスチャマッピングとテクスチャ座標の自動生成を無効にする */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	//　3D　→　2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowSize.x, windowSize.y, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (glIsEnabled(GL_LIGHTING))
	{
		isLighting = true;
		glDisable(GL_LIGHTING);
	}

	//　2Dシーンの描画
	Render2D();

	if (!glIsEnabled(GL_LIGHTING))
	{
		if (isLighting)
			glEnable(GL_LIGHTING);
	}

	//　2D　→　3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

void GameManager::Render()
{
#ifdef SHADOW_MAP_ON
	RenderWithShadow();
#else
	RenderWithoutShadow();
#endif
}

void GameManager::Update()
{
	for (int i = 0; i < 16; ++i)
	{
		balls[i].Move();
		balls[i].TestCollisionWall();
		for (int j = 0; j < 16; ++j)
		{
			if (i != j)
			{
				balls[i].TestCollisionBall(balls[j]);
			}
		}
		balls[i].TestPocket();
		balls[i].UpdateVelocity();
	}
}

void GameManager::KeyFunc(KEY key)
{
	if (status == MAIN)
	{
		switch (key)
		{
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_LEFT:
			break;
		case KEY_RIGHT:
			break;
		case KEY_W:
			break;
		case KEY_A:
			break;
		case KEY_S:
			break;
		case KEY_D:
			break;
		case KEY_V:
			break;
		case KEY_SPACE:
			break;
		case KEY_ENTER:
			balls[0].AddVec(glm::vec3(0.25f, 0.f, 0.f));
			break;
		case KEY_BACKSPACE:
			break;
		case KEY_1:
		case KEY_2:
		case KEY_3:
			break;
		}
	}
}

uint GameManager::GetWindowWidth() { return windowSize.x; }
uint GameManager::GetWindowHeight() { return windowSize.y; }
uint GameManager::GetWindowPosX() { return windowPos.x; }
uint GameManager::GetWindowPosY() { return windowPos.y; }

void GameManager::SetWindowSize(int x, int y)
{
	//　ウィンドウサイズを保存
	windowSize.x = x;
	windowSize.y = y;

	//　サイズチェック
	if (windowSize.x < 1) windowSize.x = 1;
	if (windowSize.y < 1) windowSize.y = 1;

	//　ビューポートの設定
	glViewport(0, 0, windowSize.x, windowSize.y);

	//　射影行列の設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 1000.0);
}

void DrawSmallCircle(float radius, int x, int y)
{
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(radius*2.0f);
	glBegin(GL_POINTS);
	glVertex2f((float)x, (float)y);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
}

void DrawLargeCircle(float radius, int x, int y)
{
	for (float th1 = 0.0f; th1 <= 360.0f; th1 = th1 + 2.0f)
	{
		float th2 = th1 + 10.0f;
		float th1_rad = th1 * 0.0174533f;
		float th2_rad = th2 * 0.0174533f;

		float x1 = radius * cos(th1_rad);
		float y1 = radius * sin(th1_rad);
		float x2 = radius * cos(th2_rad);
		float y2 = radius * sin(th2_rad);

		glBegin(GL_TRIANGLES);
		glVertex2f((float)x, (float)y);
		glVertex2f(x1 + x, y1 + y);
		glVertex2f(x2 + x, y2 + y);
		glEnd();
	}
}

void glutRenderText(void* bitmapfont, char*text)
{
	for (int i = 0; i<(int)strlen(text); i++)
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move)
{
	unsigned int textLength;	//引数で受け取ったテキストの長さ
	WCHAR * unicodeText;		//textをUNICODEに変換した文字列を格納する
	GLuint listbaseIdx;		//ディスプレイリストの最初のインデックス

	glRasterPos2i(pos.x, pos.y);

	//日本語の文字列として扱うよう設定
	setlocale(LC_CTYPE, "jpn");

	//textの文字数を取得
	textLength = (unsigned int)_mbstrlen(text);
	if (textLength == -1)
		return -1;

	//textの文字数分のワイド文字列の領域を作成
	unicodeText = new WCHAR[textLength + 1];
	if (unicodeText == NULL)
	{
		return -2;
	}

	//取得したジョイントIDをUNICODEに変換する
	if (MultiByteToWideChar(CP_ACP, 0, text, -1, unicodeText, (sizeof(WCHAR) * textLength) + 1) == 0)
		return -3;

	HDC hdc = wglGetCurrentDC();
	SelectObject(hdc, hfont);

	//文字数分のディスプレイリストを確保し、ディスプレイリストの最初のインデックスを取得
	listbaseIdx = glGenLists(textLength);

	GLYPHMETRICSFLOAT agmf;
	for (unsigned int textCnt = 0; textCnt < textLength; ++textCnt)
	{
		if (wglUseFontBitmapsW(hdc, unicodeText[textCnt], 1, listbaseIdx + textCnt) == FALSE)
		//if (wglUseFontOutlines(hdc, unicodeText[textCnt], 1, listbaseIdx + textCnt, 0.f, 1.f, WGL_FONT_POLYGONS, &agmf) == FALSE)
		{
			//MessageBox(hwnd, "wglUseFontBitmaps() Error!!", "wgl Error", MB_OK);
		}
	}

	//1文字描画したら文字を何bitずらすか
	glBitmap(0, 0, 0, 0, move.x, move.y, NULL);

	//ディスプレイリストを実行する
	for (unsigned int textCnt = 0; textCnt < textLength; textCnt++)
	{
		glCallList(listbaseIdx + (GLuint)textCnt);
	}

	delete[] unicodeText;
	glDeleteLists(listbaseIdx, textLength);

	return 1;
}

// 縁つき
int drawText(char *text, HFONT hfont, glm::uvec2 pos, glm::vec2 move, glm::vec4 mainColor, glm::vec4 edgeColor, int edgeSize)
{
	// 文字列描画
	glColor4f(mainColor.r, mainColor.g, mainColor.b, mainColor.a);
	drawText(text, hfont, move, pos);

	// 縁取り
	if (edgeSize > 0)
	{
		glColor4f(edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
		for (int i = -edgeSize + 1; i<edgeSize; i++)
		{
			drawText(text, hfont, move, glm::vec2(pos.x + i, pos.y + (edgeSize - i)));
			drawText(text, hfont, move, glm::vec2(pos.x + i, pos.y - (edgeSize - i)));
		}
		drawText(text, hfont, move, glm::vec2(pos.x + edgeSize, pos.y));
		drawText(text, hfont, move, glm::vec2(pos.x - edgeSize, pos.y));
	}

	return 1;
}