#include "Object.h"

Object::Object():
	position(0,0,0),
	rotation(0,0,0),
	scale(1,1,1)
{

}

Object::~Object()
{
}

void Object::init()
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

void Object::render()
{
	//DEBUG
	rotation.y += 0.01f;
	rotation.x += 0.03f;

	getGFX.getCommandList()->SetDescriptorHeaps(1, &texture.srvHeap);
	getGFX.getCommandList()->SetGraphicsRootDescriptorTable(1, texture.srvHeap->GetGPUDescriptorHandleForHeapStart());

	DirectX::XMMATRIX transformMatrix = CreateTransformMatrix();
	getGFX.getCommandList()->SetGraphicsRoot32BitConstants(1, sizeof(Transform) / 4, &transformMatrix, 0);
	getGFX.getCommandList()->IASetVertexBuffers(0, 1, &getVertexBufferView());
	getGFX.getCommandList()->IASetIndexBuffer(&indexBufferView);
	getGFX.getCommandList()->DrawIndexedInstanced(nrOfIndecies, 1, 0, 0, 0);
}

D3D12_VERTEX_BUFFER_VIEW& Object::getVertexBufferView()
{
	return this->vertexBufferView;
}

ID3D12Resource* Object::getVertexBuffer()
{
	return this->vertexBuffer.Get();
}

DirectX::XMMATRIX Object::CreateTransformMatrix()
{
	return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
