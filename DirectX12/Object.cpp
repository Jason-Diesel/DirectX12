#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

void Object::init()
{
	vertexBuffer = getGFX.createVertexBuffer(vertecies);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertecies.size() * sizeof(ColorVertex);
	vertexBufferView.StrideInBytes = sizeof(ColorVertex);
}

void Object::render()
{
	getGFX.getCommandList()->IASetVertexBuffers(0, 1, &getVertexBufferView());
	getGFX.getCommandList()->DrawInstanced(4, 1, 0, 0);
}

D3D12_VERTEX_BUFFER_VIEW& Object::getVertexBufferView()
{
	return this->vertexBufferView;
}

ID3D12Resource* Object::getVertexBuffer()
{
	return this->vertexBuffer.Get();
}
