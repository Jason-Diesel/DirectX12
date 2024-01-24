#pragma once
#include "CommonHeader.h"

struct ConstantBuffer {
	uint32_t size;
	uint32_t shaderRigister = 0;
	D3D12_SHADER_VISIBILITY shaderVisablility = D3D12_SHADER_VISIBILITY_ALL;
};



struct ViewProj
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

struct Transform
{
	DirectX::XMMATRIX transform;
};