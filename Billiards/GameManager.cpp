#include "stdafx.h"
#include "GameManager.h"

static const GLfloat lightpos[] = { 0.0f, 5.0f, 0.0f, 1.f }; /* 位置　　　　　　　 */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.f }; /* 直接光強度　　　　 */
static const GLfloat lightdim[] = { 0.2f, 0.2f, 0.2f, 1.f }; /* 影内の拡散反射強度 */
static const GLfloat lightblk[] = { 0.0f, 0.0f, 0.0f, 1.f }; /* 影内の鏡面反射強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.f }; /* 環境光強度　　　　 */

#define DEPTH_TEX_WIDTH  512
#define DEPTH_TEX_HEIGHT 512
//#define SHADOW_MAP_ON

GameManager::GameManager()
{
	windowSize.x = WINDOW_DEFAULT_W;
	windowSize.y = WINDOW_DEFAULT_H;
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

	if (!alutInit(&argc, argv))
	{
		ErrorLog("Initialize() : failed to initialize AL");
		return false;
	}

	if (!resource.Load())
	{
		ErrorLog("Initialize() : failed to load resource data");
		return false;
	}

	// タイトル画面へ
	scene = new TitleScene(resource, windowSize);

	return true;
}

void GameManager::Release()
{
	resource.Release();
}

bool GameManager::InitializeGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(WINDOW_DEFAULT_POS_X, WINDOW_DEFAULT_POS_Y);
	glutInitWindowSize(windowSize.x, windowSize.y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
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

	// マルチサンプリングを有効にする
	glEnable(GL_MULTISAMPLE);

	// アルファ値をサンプルの被覆率にする
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

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

void GameManager::Render2D()
{
	scene->Render2D();

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
	scene->Render3D();
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
	scene->SetCamera();

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
	scene->SetCamera();

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
	if (scene->IsFinished())
	{
		// 画面遷移
		GAME_STATUS next = scene->GetNextScene();
		scene->Finish();
		delete scene;

		switch (next)
		{
		case TITLE:
			scene = new TitleScene(resource, windowSize);
			break;
		case MAIN:
			scene = new MainScene(resource, windowSize);
			break;
		default:
			ErrorLog("Unknown Scene");
			break;
		}
	}
	else
	{
		scene->UpdateFps();
		scene->Update();
	}
}

void GameManager::Keyboard(KEY key)
{
	scene->Keyboard(key);
}

void GameManager::KeyboardUp(KEY key)
{
	scene->KeyboardUp(key);
}

void GameManager::Mouse(int button, int state, int x, int y)
{
	scene->Mouse(button, state, x, y);
}

void GameManager::Motion(int x, int y)
{
	scene->Motion(x, y);
}

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
	gluPerspective(45.0, (GLdouble)windowSize.x/(GLdouble)windowSize.y, 0.1, 1000.0);
}

