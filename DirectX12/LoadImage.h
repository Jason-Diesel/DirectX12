#pragma once
#include "CommonHeader.h"
#include <ranges>

TextureViewClass CreateTexture(
	const std::string& filePath, 
	Microsoft::WRL::ComPtr<ID3D12Device8>& device,
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& commandAllocator,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
	Microsoft::WRL::ComPtr<ID3D12Fence>& fence,
	uint64_t &fenceValue,
	HANDLE &fenceEvent
);