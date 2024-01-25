#include "Model.h"

Model::Model()
{
}

void Model::render()
{
	for (int i = 0; i < subMeshes.size(); i++)
	{
		subMeshes[i]->render();
	}
}

std::vector<Mesh*>& Model::getMeshes()
{
	return this->subMeshes;
}
