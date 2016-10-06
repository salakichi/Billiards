#pragma once

#include "XLoader.h"

class Ball
{
private:
	glm::vec3 wallMin;
	glm::vec3 wallMax;

public:
	XModel model;
	void setWall(glm::vec3 min, glm::vec3 max)
	{
		wallMin = min; wallMax = max;
	}
	void pocketTest()
	{

	};
};