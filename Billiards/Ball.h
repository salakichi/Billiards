#pragma once

#include "XLoader.h"

class Ball
{
private:
	bool isFall;
	bool isMove;
	glm::vec3 wallMin;
	glm::vec3 wallMax;
	glm::vec3 rotVelocity;

public:
	XModel model;
	glm::vec3 velocity;

	Ball()
	{
		isFall = false;
		isMove = false;
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
};