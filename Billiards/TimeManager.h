#pragma once

#include "MyFreeglut.h"

class TimeManager {
private:
	// FPS用
	double currentTime;
	double lastTime;
	double firstCount;
	double currentCount;
	double lastCount;
	int frameCount;
	double fps;
	char fpsString[50];
public:
	// シングルトン用のインスタンス
	static TimeManager &Instance() {
		static TimeManager inst;
		return inst;
	}

	TimeManager();
	void Update();

	// n秒周期で0-1増減するときの現在の割合(0～1)
	double GetPeriodRatio(double period, double startTime = 0.0);

	char* GetFpsString() {
		return fpsString;
	}
	double GetDeltaTime() {
		return currentCount - lastCount;
	}
	double GetCurrentTime() {
		return currentTime;
	}
};