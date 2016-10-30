#include "stdafx.h"
#include "Ball.h"

void Ball::Initialize()
{
	status = STOP;
	lastStatus = STOP;
	velocity = glm::vec3(0.0f);
	rotVelocity = glm::vec3(0.0f);
}

void Ball::SetWall(glm::vec3 min, glm::vec3 max)
{
	wallMin = min;
	wallMax = max;

	pocketPos[0] = glm::vec3(0.f, 0.f, wallMin.z);
	pocketPos[1] = glm::vec3(0.f, 0.f, wallMax.z);
	pocketPos[2] = glm::vec3(wallMin.x, 0.f, wallMin.z);
	pocketPos[3] = glm::vec3(wallMin.x, 0.f, wallMax.z);
	pocketPos[4] = glm::vec3(wallMax.x, 0.f, wallMin.z);
	pocketPos[5] = glm::vec3(wallMax.x, 0.f, wallMax.z);
}

void Ball::AddVec(glm::vec3 addVel)
{
	velocity += addVel;
	status = MOVE;
}

void Ball::Move()
{
	if (status == MOVE)
	{
		model.position += velocity;
		rotVelocity = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), velocity)) * glm::length(velocity) * 2.f;
		model.rotation *= glm::rotate(glm::mat4(1.0f), glm::length(rotVelocity), glm::normalize(rotVelocity));
	}
	else if (status == FALL)
	{
		model.position += velocity;
		if (model.position.y < -5.f)
		{
			model.position.y = -5.f;
			status = POCKET;
		}
	}
}

void Ball::TestCollisionWall()
{
	if (status == MOVE)
	{
		glm::vec3 pos = model.position;
		float r = model.sphere.radius;
		bool xHitFlag = false;
		bool zHitFlag = false;

		if ((pos.x < wallMin.x + r && velocity.x < 0.0f) || (pos.x > wallMax.x - r && velocity.x > 0.0f))
		{
			xHitFlag = true;
		}
		if ((pos.z < wallMin.z + r && velocity.z < 0.0f) || (pos.z > wallMax.z - r && velocity.z > 0.0f))
		{
			zHitFlag = true;
		}

		if (xHitFlag) velocity.x *= -1.f;
		if (zHitFlag) velocity.z *= -1.f;
	}
}

void Ball::TestCollisionBall(Ball* ball)
{
	if (status == MOVE)
	{
		float old_distance = glm::distance(model.position - velocity, ball->model.position);
		float distance = glm::distance(model.position, ball->model.position);
		// ãﬂÇ√Ç¢Çƒê⁄êGÇµÇΩÇÁ
		if (old_distance > distance && distance < model.sphere.radius + ball->model.sphere.radius)
		{
			glm::vec3 vec = glm::normalize(ball->model.position - model.position);
			glm::vec3 gap = glm::dot(velocity, vec)*vec - glm::dot(ball->velocity, -vec)*(-vec);
			if (glm::length(gap) > FLT_EPSILON)
			{
				velocity -= gap;
				ball->AddVec(gap);

				hitScale = glm::length(gap);
			}
		}
	}
}

void Ball::UpdateVelocity()
{
	if (status == MOVE)
	{
		float dv = 0.007f;
		velocity *= (1.f - dv);

		if (glm::length(velocity) < 0.003f)
		{
			status = STOP;
			velocity = glm::vec3(0.0f);
			rotVelocity = glm::vec3(0.0f);
			return;
		}
	}
	else if (status == FALL)
	{
		glm::vec3 pos = model.position;
		float r = model.sphere.radius;
		if (pos.x < wallMin.x + r || pos.x > wallMax.x - r || pos.z < wallMin.z + r || pos.z > wallMax.z - r)
		{
			velocity.x *= 0.9f;
			velocity.y -= 0.06f;
			velocity.z *= 0.9f;
		}
	}
}

void Ball::TestPocket()
{
	if (status == MOVE)
	{
		glm::vec3 pos = model.position;
		pos.y = 0.f;
		float r = model.sphere.radius;

		// ê^ÇÒíÜÇÃåä
		if (glm::length(pocketPos[0] - pos) < r * 1.5f && velocity.z < 0.f)
		{
			status = FALL;
		}
		else if (glm::length(pocketPos[1] - pos) < r * 1.5f && velocity.z > 0.f)
		{
			status = FALL;
		}
		else
		{
			// í[ÇÃåä
			for (int i = 2; i < 6; ++i)
			{
				float distance = glm::length(pocketPos[i] - pos);
				if (distance < r * 2.f)
				{
					status = FALL;
				}
			}
		}
	}
};