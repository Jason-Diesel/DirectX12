#pragma once

#include "CommonHeader.h"
#include <wrl.h>
#include "WindowClass.h"
#include "Debugger.h"
#include "HelperFunctions.h"

class DirectXCommand {
public:
	DirectXCommand(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_BUNDLE);
	~DirectXCommand();
	void beginFrame();
	void endFrame();
	ID3D12CommandQueue* const command_queue() const;
	ID3D12GraphicsCommandList6* const command_list() const;
	constexpr uint32_t frame_index() const;
private:
	struct CommandFrame {
		ID3D12CommandAllocator* commandAllocator;
		uint64_t fenceValue = 0;
		void wait(HANDLE fenceEvent, ID3D12Fence1* fence);
		void release();
	};
	uint32_t frameIndex;
	CommandFrame commandFrames[NrOfFrameBuffers];
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList;

	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
	uint64_t fenceValue;
private:
	void flush();
};