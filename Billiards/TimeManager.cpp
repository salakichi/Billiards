#include "stdafx.h"
#include "TimeManager.h"

TimeManager::TimeManager()
{
	// FPSカウンターの初期化
	currentCount = lastCount = firstCount = glutGet(GLUT_ELAPSED_TIME);
	frameCount = 0;
	currentTime = 0.0;
	lastTime = 0.0;
	fps = 0.0f;
}

void TimeManager::Update()
{
	//　時間計測とFPS算出
	lastCount = currentCount;
	currentCount = glutGet(GLUT_ELAPSED_TIME);
	currentTime = (currentCount - firstCount) / 1000.0;
	frameCount++;
	if ((currentTime - lastTime) >= 1.0)
	{
		fps = frameCount / (currentTime - lastTime);
		sprintf_s(fpsString, "%.1fFPS", fps);
		frameCount = 0;
		lastTime = currentTime;
	}
}

// n秒周期で考えた時、現在の割合を算出する
double TimeManager::GetPeriodRatio(double period, double startTime)
{
	double elapsedTime = currentTime - startTime;
	double ratio = ((elapsedTime - (double)((int)(elapsedTime / period)) * period) / period) * 2.0;
	if (ratio > 1.0)
	{
		ratio = 2 - ratio;
	}
	return ratio;
}