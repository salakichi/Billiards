#pragma once

#include "MyFreeglut.h"

class TimeManager {
private:
	// FPS�p
	double currentTime;
	double lastTime;
	double firstCount;
	double currentCount;
	double lastCount;
	int frameCount;
	double fps;
	char fpsString[50];
public:
	// �V���O���g���p�̃C���X�^���X
	static TimeManager &Instance() {
		static TimeManager inst;
		return inst;
	}

	TimeManager();
	void Update();

	// n�b������0-1��������Ƃ��̌��݂̊���(0�`1)
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