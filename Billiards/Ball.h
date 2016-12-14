#pragma once

#include "XLoader.h"
#include "TimeManager.h"

enum BallStatus
{
	STOP, MOVE, FALL, POCKET
};

class Ball
{
private:
	glm::vec3 wallMin;
	glm::vec3 wallMax;
	glm::vec3 rotVelocity;
	glm::vec3 pocketPos[6];
	float hitScale; // è’ìÀâπóp
public:
	BallStatus status;
	BallStatus lastStatus;
	XModel model;
	glm::vec3 velocity;

	float size;
	glm::vec3 position;
	glm::mat4 rotation;

	Ball() {}
	void Initialize(glm::vec3 pos);
	void SetWall(glm::vec3 min, glm::vec3 max);
	void AddVec(glm::vec3 addVel);
	void Move();
	void TestCollisionWall();
	void TestCollisionBall(Ball* ball);
	void UpdateVelocity();
	void TestPocket();
	void ResetHit() { hitScale = 0.f; }
	float GetHitScale() { return hitScale; }
	bool isMove() { return (status == MOVE || status == FALL); }
	bool Load(char *filename, float scale = 1.0f);
	void Render();
};