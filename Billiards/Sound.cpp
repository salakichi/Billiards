#include "stdafx.h"
#include "Sound.h"

void Sound::ErrorLog(char* msg)
{
	cout << "Sound Error : " << msg << endl;
}

Sound::Sound()
{
}

Sound::~Sound()
{
}

bool Sound::LoadWave(const char* path, ALfloat g, ALboolean loop)
{
	// Load wav data into a buffer.
	buffer = alutCreateBufferFromFile(path);

	if (alGetError() != AL_NO_ERROR)
	{
		ErrorLog("failed to create buffer");
		return AL_FALSE;
	}

	// Bind the buffer with the source.
	alGenSources(1, &source);

	if (alGetError() != AL_NO_ERROR)
	{
		ErrorLog("failed to generate source");
		return AL_FALSE;
	}

	//�\�[�X�̏����ݒ�
	alSourcei(source, AL_BUFFER, buffer);//����
	alSourcef(source, AL_REFERENCE_DISTANCE, 5.0f);//���W�n�ɂ�錸��
	SetPitch(1.f); //�s�b�`
	SetGain(g);  //�Q�C��
	SetPosition(glm::vec3());//���W
	SetVelocity(glm::vec3());//�ړ�
	SetLoop(loop); // ���[�v

	if (alGetError() != AL_NO_ERROR)
	{
		ErrorLog("failed to set config");
		return AL_FALSE;
	}

	return AL_TRUE;
}

void Sound::Release()
{
	if (alIsBuffer(buffer))
		alDeleteBuffers(1, &buffer);
	if (alIsSource(source))
		alDeleteSources(1, &source);
}

void Sound::SetPosition(float x, float y, float z)
{
	ALfloat pos[] = { x,y,z };
	alSourcefv(source, AL_POSITION, pos);
}

void Sound::SetPosition(glm::vec3 pos)
{
	SetPosition(pos.x, pos.y, pos.z);
}

void Sound::SetVelocity(float x, float y, float z)
{
	ALfloat vel[] = { x,y,z };
	alSourcefv(source, AL_VELOCITY, vel);
}

void Sound::SetVelocity(glm::vec3 vel)
{
	SetVelocity(vel.x, vel.y, vel.z);
}

void Sound::SetGain(float g)
{
	gain = g;
	alSourcef(source, AL_GAIN, gain);
}

void Sound::AddGain(float add)
{
	gain += add;
	if (gain < 0.f) gain = 0.f;
	if (gain > 1.f) gain = 1.f;
	SetGain(gain);
}

void Sound::SetPitch(float pitch)
{
	alSourcef(source, AL_PITCH, pitch);
}

void Sound::SetLoop(bool loopFlag)
{
	alSourcei(source, AL_LOOPING, loopFlag);
}

void Sound::Play()
{
	alSourcePlay(source);
}

void Sound::Pause()
{
	alSourcePause(source);
}

void Sound::Stop()
{
	alSourceStop(source);
}