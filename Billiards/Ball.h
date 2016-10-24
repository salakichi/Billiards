#pragma once

#include "XLoader.h"

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

	Ball()
	{
		status = STOP;
		lastStatus = STOP;
		velocity = glm::vec3(0.0f);
		rotVelocity = glm::vec3(0.0f);
	};
	void SetWall(glm::vec3 min, glm::vec3 max);
	void AddVec(glm::vec3 addVel);
	void Move();
	void TestCollisionWall();
	void TestCollisionBall(Ball* ball);
	void UpdateVelocity();
	void TestPocket();
	void ResetHit() { hitScale = 0.f; }
	float GetHitScale() { return hitScale; }
};