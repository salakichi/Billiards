#pragma once

#include "Scene.h"
#include "Mouse.h"

// Game Over/ClearópÉVÅ[Éì

class FinishScene : public Scene
{
private:
public:
	FinishScene(ResourceManager& resource, glm::uvec2 &size);
	~FinishScene();
	void SetCamera();
	void Render2D();
	void Render3D();
	void Update();
	void Keyboard(KEY key);
	void KeyboardUp(KEY key);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y);
	void Finish();
};