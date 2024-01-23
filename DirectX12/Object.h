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
protected:
	std::vector<ColorVertex> vertecies;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};