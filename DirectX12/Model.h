#pragma once
#include "Mesh.h"

class Model {
public:
	Model(std::string fileName);
	void render();
private:
	std::vector<Mesh> subMeshes;
};