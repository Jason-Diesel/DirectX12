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
	std::vector<ColorVertex> vertecies;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};