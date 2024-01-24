#pragma once
#include "Graphics.h"

class Mesh
{
public:
	Mesh();
	template <typename T>
	Mesh(std::vector<T>& vertecies, std::vector<uint32_t>& indecies) {
		init(vertecies, indecies);
	}
	template <typename T>
	void init(std::vector<T>& vertecies, std::vector<uint32_t>& indecies)
	{
		vertexBuffer = getGFX.createVertexBuffer(vertecies);
		nrOfVertecies = vertecies.size();
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = vertecies.size() * sizeof(T);
		vertexBufferView.StrideInBytes = sizeof(T);
	}
	void render();
private:
	uint32_t nrOfVertecies;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
};