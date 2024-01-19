#include "Resources.h"

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type):
	type(type)
{
}

bool DescriptorHeap::init(uint32_t capacity, bool is_shader_visible)
{
	mutex.lock();
	if (!capacity || capacity > D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2) { breakDebug;  return false; }

	

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
		type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
		is_shader_visible = false;
	}

	release();

	this->capacity = capacity;


	return false;
}

DescriptorHandle DescriptorHeap::allocate()
{
	mutex.lock();
	return DescriptorHandle();
}

void DescriptorHeap::release()
{
	mutex.lock();
}

void DescriptorHeap::free(DescriptorHandle& handle)
{
	mutex.lock();
}

