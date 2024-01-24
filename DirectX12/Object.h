#pragma once
#include "Graphics.h"

class Object {
public:
	Object();
	~Object();
	void init();
	void render();
	D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView();
	ID3D12Resource* getVertexBuffer();

	DirectX::XMMATRIX CreateTransformMatrix();
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
protected:
	TextureViewClass texture;
	std::vector<Vertex> vertecies;
	std::vector<uint32_t> indecies;
	uint32_t nrOfVertecies;
	uint32_t nrOfIndecies;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	
};