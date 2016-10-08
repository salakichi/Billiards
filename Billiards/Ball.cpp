#include "stdafx.h"
#include "Ball.h"

void Ball::SetWall(glm::vec3 min, glm::vec3 max)
{
	wallMin = min;
	wallMax = max;
}

void Ball::AddVec(glm::vec3 addVel)
{
	velocity += addVel;
	rotVelocity = glm::cross(glm::vec3(0.f, 1.f, 0.f), velocity) * glm::length(velocity) * 12.f;
	isMove = true;
}
void Ball::Move()
{
	if (!isMove) return;

	model.position += velocity;
	model.rotation *= glm::rotate(glm::mat4(1.0f), glm::length(rotVelocity), glm::normalize(rotVelocity));
}
void Ball::TestCollisionWall()
{
	glm::vec3 pos = model.position;
	float r = model.sphere.radius;
	bool xHitFlag = false;
	bool zHitFlag = false;

	if ((pos.x < wallMin.x + r && velocity.x < 0.0f) || (pos.x > wallMax.x - r && velocity.x > 0.0f)) xHitFlag = true;
	if ((pos.z < wallMin.z + r && velocity.z < 0.0f) || (pos.z > wallMax.z - r && velocity.z > 0.0f)) zHitFlag = true;

	if (xHitFlag) velocity.x *= -1.f;
	if (zHitFlag) velocity.z *= -1.f;
	if (xHitFlag || zHitFlag) rotVelocity = glm::cross(glm::vec3(0.f, 1.f, 0.f), velocity) * glm::length(velocity) * 12.f;
}
void Ball::TestCollisionBall(Ball& ball)
{
	float old_distance = glm::distance(model.position - velocity, ball.model.position);
	float distance = glm::distance(model.position, ball.model.position);
	// ‹ß‚Ã‚¢‚ÄÚG‚µ‚½‚ç
	if (old_distance > distance && distance < model.sphere.radius + ball.model.sphere.radius)
	{
		glm::vec3 vec = glm::normalize(ball.model.position - model.position);
		glm::vec3 gap = glm::dot(velocity, vec)*vec - glm::dot(ball.velocity, -vec)*(-vec);
		if (glm::length(gap) > FLT_EPSILON)
		{
			velocity -= gap;
			ball.AddVec(gap);
		}
	}
}
void Ball::UpdateVelocity()
{
	if (!isMove) return;

	float dv = 0.6f*9.8f*0.001f;
	velocity *= (1.f - dv);
	rotVelocity *= (1.f - model.sphere.radius * 2.f * dv);

	if (glm::length(velocity) < FLT_EPSILON && glm::length(rotVelocity) < FLT_EPSILON)
	{
		isMove = false;
		velocity = glm::vec3(0.0f);
		rotVelocity = glm::vec3(0.0f);
		return;
	}
}
void Ball::TestPocket()
{

};