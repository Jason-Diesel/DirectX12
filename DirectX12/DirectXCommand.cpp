#include "DirectXCommand.h"



DirectXCommand::DirectXCommand(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type) :
	frameIndex(0)
{
	D3D12_COMMAND_QUEUE_DESC CQDesc{};
	CQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CQDesc.NodeMask = 0;
	CQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//higher later?
	CQDesc.Type = type;
	if (device->CreateCommandQueue(&CQDesc, IID_PPV_ARGS(&commandQueue)) != S_OK)
	{
		breakDebug;
	}
	//should only be done in debug
	commandQueue->SetName(type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
		L"GFX Command Queue" :
		type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
		L"Compute Command Queue" : L"Command Queue"
	);

	for (int i = 0; i < NrOfFrameBuffers; i++)
	{
		if (device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandFrames[i].commandAllocator)) != S_OK) {
			breakDebug;
		}
		commandFrames[i].commandAllocator->SetName(type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
			CreateStringandIndexToWString("commandAllocator", i).c_str() :
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
			CreateStringandIndexToWString("Compute Command List", i).c_str() :
			CreateStringandIndexToWString("Command List", i).c_str()
		);
	}

	if (device->CreateCommandList(0, type, commandFrames[0].commandAllocator, nullptr, IID_PPV_ARGS(&commandList)) != S_OK) {
		breakDebug;
	}
	commandList->Close();
	commandList->SetName(type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
		L"GFX Command List" :
		type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
		L"Compute Command List" : L"Command List"
	);

	if (device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) != S_OK)
	{
		breakDebug;
	}
	//Only in debug
	fence.Get()->SetName(L"fence");

	fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
}

DirectXCommand::~DirectXCommand()
{
	this->flush();
	CloseHandle(fenceEvent);
	fenceEvent = nullptr;
}

void DirectXCommand::beginFrame()
{
	commandFrames[frameIndex].wait(fenceEvent, fence.Get());
	if (commandFrames[frameIndex].commandAllocator->Reset() != S_OK)
	{
		breakDebug;
	}
	if (commandList->Reset(commandFrames[frameIndex].commandAllocator, nullptr) != S_OK)
	{
		breakDebug;
	}
}

void DirectXCommand::endFrame()
{
	if (commandList->Close() != S_OK)
	{
		breakDebug;
	}
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(commandLists), &commandLists[0]);

	fenceValue++;
	commandFrames[frameIndex].fenceValue = fenceValue;
	commandQueue->Signal(fence.Get(), fenceValue);

	frameIndex = (++frameIndex) % NrOfFrameBuffers;

}

ID3D12CommandQueue* const DirectXCommand::command_queue() const
{
	return this->commandQueue.Get();
}

ID3D12GraphicsCommandList6* const DirectXCommand::command_list() const
{
	return this->commandList.Get();
}

constexpr uint32_t DirectXCommand::frame_index() const
{
	return this->frameIndex;
}

void DirectXCommand::flush()
{
	for (int i = 0; i < NrOfFrameBuffers; i++)
	{
		commandFrames[i].wait(fenceEvent, fence.Get());
	}
}

void DirectXCommand::CommandFrame::wait(HANDLE fenceEvent, ID3D12Fence1* fence)
{
	if (fence->GetCompletedValue() < fenceValue)
	{
		if (fence->SetEventOnCompletion(fenceValue, fenceEvent) != S_OK)
		{
			breakDebug;
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void DirectXCommand::CommandFrame::release()
{
	HF::release(commandAllocator);
}
