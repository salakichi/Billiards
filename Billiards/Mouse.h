/***************************************************************************
Mouse.h

Original:
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
#include "glm\vec3.hpp"
#include "MyFreeglut.h"
using namespace std;


//
//　global
//
const double PI = 3.14159265358979323846264338327;
template<class T> static inline T RadToDeg(T rad) { return ((rad)*(180.0 / PI)); }
template<class T> static inline T DegToRad(T deg) { return ((deg)*(PI / 180.0)); }
typedef enum MState { Push, Release, None };

//////////////////////////////////////////////////////////////////////////
//　　MPosition class
//////////////////////////////////////////////////////////////////////////
class MPosition
{
public:
	double x;
	double y;

	MPosition(double nx = 0.0, double ny = 0.0) { x = nx; y = ny; }
	~MPosition() {}
	void Reset() { x = 0.0, y = 0.0; }
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
private:
	bool enableLeftX;
	bool enableLeftY;
	bool enableRightX;
	bool enableRightY;
	bool enableMiddleX;
	bool enableMiddleY;

	MouseButton right;
	MouseButton left;
	MouseButton middle;

	double distance;
	glm::dvec3 angle;
	glm::dvec3 position;
	glm::dvec3 target;
	glm::dvec3 upvector;
	glm::dvec3 translate;
	glm::dvec3 offset;

public:
	ViewCamera(double dist = 45.0);
	~ViewCamera();
	void Reset();
	void MouseInput(int button, int state, int x, int y);
	void MouseMotion(int x, int y);
	void Set();
	void RenderSubAxis(int w, int h);
	void SetEnableLeft(bool x, bool y) {
		enableLeftX = x;
		enableLeftY = y;
	}
	void SetEnableRight(bool x, bool y) {
		enableRightX = x;
		enableRightY = y;
	}
	void SetEnableMiddle(bool x, bool y) {
		enableMiddleX = x;
		enableMiddleY = y;
	}
	void SetDistance(double d) { distance = d; }
	glm::vec3 GetAngle() { return angle; }
	void SetAngleDeg(glm::dvec3 a) { angle = a*(PI / 180.0); }
	void SetTarget(glm::dvec3 t) { target = t; }
	void SetTranslate(glm::dvec3 t) { translate = t; }
	void SetOffset(glm::dvec3 o) { offset = o; }
};