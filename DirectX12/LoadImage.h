#pragma once
#include "CommonHeader.h"
#include <ranges>

TextureViewClass CreateTexture(
	const std::string& filePath, 
	ID3D12Device8* device, 
	ID3D12CommandAllocator* commandAllocator, 
	ID3D12GraphicsCommandList* commandList, 
	ID3D12CommandQueue* commandQueue,
	ID3D12Fence* fence,
	uint64_t &fenceValue,
	HANDLE &fenceEvent
);