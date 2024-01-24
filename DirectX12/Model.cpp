#include "Model.h"

Model::Model(std::string fileName)
{
}

void Model::render()
{
	for (int i = 0; i < subMeshes.size(); i++)
	{
		subMeshes[i].render();
	}
}
