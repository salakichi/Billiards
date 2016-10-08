/***************************************************************************
Mouse.h

Mouse Function Library

Version : 1.0
Date : Nov. 02, 2007
Author : Pocol
****************************************************************************/

#pragma once

//
//　include
//
#include <iostream>
#include <cmath>
#include "MyFreeglut.h"
#include "glm/vec3.hpp"
using namespace std;


//
//　global
//
const float PI = 3.1415926535f;
template<class T> static inline T RadToDeg(T rad) { return ((rad)*(180.f / PI)); }
template<class T> static inline T DegToRad(T deg) { return ((deg)*(PI / 180.f)); }
typedef enum MState { Push, Release, None };

//////////////////////////////////////////////////////////////////////////
//　　MPosition class
//////////////////////////////////////////////////////////////////////////
class MPosition
{
public:
	float x;
	float y;

	MPosition(float nx = 0.f, float ny = 0.f) { x = nx; y = ny; }
	~MPosition() {}
	void Reset() { x = 0.f, y = 0.f; }
	void ConsoleOut() { cout << "(x, y) = (" << x << ", " << y << ")\n"; }
};


//////////////////////////////////////////////////////////////////////////
//　　MouseButton class
//////////////////////////////////////////////////////////////////////////
class MouseButton
{
public:
	MPosition before;
	MPosition current;
	MPosition after;
	MState state;

	MouseButton();
	~MouseButton();
	void Reset();
	void ConsoleOut();
};

//////////////////////////////////////////////////////////////////////////
//　　ViewCamara class
//////////////////////////////////////////////////////////////////////////
class ViewCamera
{
public:
	MouseButton right;
	MouseButton left;
	MouseButton middle;

	float distance;
	glm::vec3 angle;
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 upvector;
	glm::vec3 translate;

	ViewCamera(float dist = 45.0);
	~ViewCamera();
	void Reset();
	void MouseInput(int button, int state, int x, int y);
	void MouseMotion(int x, int y);
	void Set();
	void RenderSubAxis(int w, int h);
};
