#pragma once

#include "MyAlut.h"
#include "glm\vec3.hpp"

class Sound
{
private:
	ALuint buffer;
	ALuint source;
	float gain;
	void ErrorLog(char* msg);
public:
	Sound();
	~Sound();
	bool LoadWave(const char* path, ALfloat gain = 1.0f, ALboolean loop = AL_FALSE);
	void Release();
	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3 pos);
	void SetVelocity(float x, float y, float z);
	void SetVelocity(glm::vec3 vel);
	void SetGain(float gain);
	void AddGain(float add);
	void SetPitch(float pitch);
	void SetLoop(bool loopFlag);
	void Play();
	void Pause();
	void Stop();
};