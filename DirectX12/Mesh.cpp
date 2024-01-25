#include "Mesh.h"

Mesh::Mesh():
	nrOfVertecies(0)
{
}

void Mesh::init(std::vector<Vertex>& vertecies, std::vector<uint32_t>& indecies)
{
	this->nrOfIndecies = (uint32_t)indecies.size();
	this->nrOfVertecies = (uint32_t)vertecies.size();

	vertexBuffer = getGFX.createVertexBuffer(vertecies);
	indexBuffer = getGFX.createIndeciesBuffer(indecies);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = (uint32_t)vertecies.size() * sizeof(Vertex);
	vertexBufferView.StrideInBytes = sizeof(Vertex);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = nrOfIndecies * sizeof(uint32_t);
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Mesh::render()
{
	getGFX.getCommandList()->SetDescriptorHeaps(1, &texture.srvHeap);
	getGFX.getCommandList()->SetGraphicsRootDescriptorTable(2, texture.srvHeap->GetGPUDescriptorHandleForHeapStart());
	//TODO : should these be sent to the model and set togheter in one "IASetVertexBuffers"
	getGFX.getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	getGFX.getCommandList()->IASetIndexBuffer(&indexBufferView);
	getGFX.getCommandList()->DrawIndexedInstanced(nrOfIndecies, 1, 0, 0, 0);
}

void Mesh::setTextureViewClass(TextureViewClass textureView)
{
	this->texture = textureView;
}
