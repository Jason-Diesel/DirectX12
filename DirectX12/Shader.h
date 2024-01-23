#pragma once
#include "CommonHeader.h"
#include <d3dcompiler.h>

//RIGHT NOW THIS IS THE SHADER
struct Shader {
public:
	Shader();
	void init(ID3D12Device8* device);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};