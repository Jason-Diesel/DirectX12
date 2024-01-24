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
	vertexBuffer = getGFX.createVertexBuffer(vertecies);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = (uint32_t)vertecies.size() * sizeof(ColorVertex);
	vertexBufferView.StrideInBytes = sizeof(ColorVertex);
}

void Object::render()
{
	//DEBUG
	rotation.y += 0.01f;
	DirectX::XMMATRIX transformMatrix = CreateTransformMatrix();
	getGFX.getCommandList()->SetGraphicsRoot32BitConstants(1, sizeof(Transform) / 4, &transformMatrix, 0);
	getGFX.getCommandList()->IASetVertexBuffers(0, 1, &getVertexBufferView());
	getGFX.getCommandList()->DrawInstanced(3, 1, 0, 0);
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
