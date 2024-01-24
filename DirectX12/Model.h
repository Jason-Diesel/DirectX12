#pragma once
#include "Mesh.h"

class Model {
public:
	Model(std::string fileName);
private:
	std::vector<Mesh> subMeshes;
};