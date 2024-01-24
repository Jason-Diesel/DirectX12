#pragma once
#include "CommonHeader.h"
#include "ConstantBuffers.h"

//RIGHT NOW THIS IS THE SHADER


struct Shader {
public:
	Shader();
	void init(
		ID3D12Device8* device, 
		std::vector<ConstantBuffer> constantBuffers,
		std::vector<D3D12_INPUT_ELEMENT_DESC> customInputLayout
	);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};