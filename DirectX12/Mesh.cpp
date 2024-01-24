#include "Mesh.h"

Mesh::Mesh():
	nrOfVertecies(0)
{
}

void Mesh::render()
{
	//TODO : should these be sent to the model and set togheter in one "IASetVertexBuffers"
	getGFX.getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	getGFX.getCommandList()->DrawInstanced(nrOfVertecies, 1, 0, 0);
}
