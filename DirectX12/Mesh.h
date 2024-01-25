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
	void init(std::vector<Vertex>& vertecies, std::vector<uint32_t>& indecies);
	void render();
	void setTextureViewClass(TextureViewClass textureView);
private:
	TextureViewClass texture;
	uint32_t nrOfVertecies;//probably not needed here
	uint32_t nrOfIndecies;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
};