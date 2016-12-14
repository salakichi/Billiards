#include "stdafx.h"
#include "Ball.h"

void Ball::Initialize(glm::vec3 pos)
{
	status = STOP;
	lastStatus = STOP;
	velocity = glm::vec3(0.0f);
	rotVelocity = glm::vec3(0.0f);
	position = pos;
	rotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0, 0.0, 0.0));
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
	float timeRatio = (float)(TimeManager::Instance().GetDeltaTime() / 16.666);
	if (status == MOVE)
	{
		position += velocity * timeRatio;
		rotVelocity = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), velocity)) * glm::length(velocity) * 2.f * timeRatio;
		rotation *= glm::rotate(glm::mat4(1.0f), glm::length(rotVelocity), glm::normalize(rotVelocity));
	}
	else if (status == FALL)
	{
		position += velocity * timeRatio;
		if (position.y < -5.f)
		{
			position.y = -5.f;
			status = POCKET;
		}
	}
}

void Ball::TestCollisionWall()
{
	if (status == MOVE)
	{
		bool xHitFlag = false;
		bool zHitFlag = false;

		if ((position.x < wallMin.x + size && velocity.x < 0.0f) || (position.x > wallMax.x - size && velocity.x > 0.0f))
		{
			xHitFlag = true;
		}
		if ((position.z < wallMin.z + size && velocity.z < 0.0f) || (position.z > wallMax.z - size && velocity.z > 0.0f))
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
		float old_distance = glm::distance(position - velocity, ball->position);
		float distance = glm::distance(position, ball->position);
		// ãﬂÇ√Ç¢Çƒê⁄êGÇµÇΩÇÁ
		if (old_distance > distance && distance < size + ball->size)
		{
			glm::vec3 vec = glm::normalize(ball->position - position);
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
	float timeRatio = (float)(TimeManager::Instance().GetDeltaTime() / 16.666);
	if (status == MOVE)
	{
		float dv = 0.007f * timeRatio;
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
		if (position.x < wallMin.x + size || position.x > wallMax.x - size || position.z < wallMin.z + size || position.z > wallMax.z - size)
		{
			velocity.x *= (1.f - timeRatio);
			velocity.y -= 0.06f * timeRatio;
			velocity.z *= (1.f - timeRatio);
		}
	}
}

void Ball::TestPocket()
{
	if (status == MOVE)
	{
		position.y = 0.f;

		// ê^ÇÒíÜÇÃåä
		if (glm::length(pocketPos[0] - position) < size * 1.5f && velocity.z < 0.f)
		{
			status = FALL;
		}
		else if (glm::length(pocketPos[1] - position) < size * 1.5f && velocity.z > 0.f)
		{
			status = FALL;
		}
		else
		{
			// í[ÇÃåä
			for (int i = 2; i < 6; ++i)
			{
				float distance = glm::length(pocketPos[i] - position);
				if (distance < size * 2.f)
				{
					status = FALL;
				}
			}
		}
	}
}

bool Ball::Load(char *filename, float scale)
{
	bool result = model.Load(filename, scale);
	if (result) {
		size = model.sphere.radius;
	}
	return result;
}

void Ball::Render()
{
	glPushMatrix();

	glm::mat4x4 transRotMat = glm::translate<float>(glm::mat4(1.0f), position) * rotation * glm::translate<float>(glm::mat4(1.0f), glm::vec3(0.f, -size, 0.f));
	glMultMatrixf(&transRotMat[0][0]);

	model.Render();

	glPopMatrix();
}