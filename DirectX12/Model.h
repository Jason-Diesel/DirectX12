#pragma once
#include "Mesh.h"

class Model {
public:
	Model();
	void render();
	std::vector<Mesh*>& getMeshes();
private:
	std::vector<Mesh*> subMeshes;
};