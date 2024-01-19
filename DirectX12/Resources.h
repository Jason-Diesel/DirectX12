#pragma once
#include <mutex>
#include "CommonHeader.h"
#include "Graphics.h"

struct DescriptorHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE	cpu{};
	D3D12_GPU_DESCRIPTOR_HANDLE	gpu{};

	bool is_valid() const { return cpu.ptr != 0; };
	bool is_shader_visible() const { return gpu.ptr != 0; };
};

class DescriptorHeap
{
public:
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
	bool init(uint32_t capacity, bool is_shader_visible = false);
	DescriptorHandle allocate();
	void release();
	void free(DescriptorHandle& handle);


private:
	ID3D12DescriptorHeap*				heap;
	D3D12_CPU_DESCRIPTOR_HANDLE			cpuStart;
	D3D12_GPU_DESCRIPTOR_HANDLE			gpuStart;
	std::unique_ptr<uint32_t[]>			freeHandles;
	std::mutex							mutex;
	uint32_t							capacity = 0;
	uint32_t							size = 0;
	uint32_t							dexcriptorSize;
	const D3D12_DESCRIPTOR_HEAP_TYPE	type{};
};