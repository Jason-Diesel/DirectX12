#include "LoadImage.h"
#include <DirectXTex.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureViewClass CreateTexture(
	const std::string& filePath, 
	Microsoft::WRL::ComPtr<ID3D12Device8>& device,
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& commandAllocator,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue,
	Microsoft::WRL::ComPtr<ID3D12Fence>& fence,
	uint64_t& fenceValue,
	HANDLE& fenceEvent
	)
{
	struct stat buffer;
	if (!(stat(filePath.c_str(), &buffer) == 0)) {
		return TextureViewClass();
	}

	TextureViewClass theReturn;

	ID3D12Resource* texture;
    
	DirectX::ScratchImage image;
	
	//DirectX::LoadFromWICFile(createStringToWString(filePath).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	DirectX::LoadFromTGAFile(createStringToWString(filePath).c_str(), nullptr, image);

	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(*image.GetImages(), DirectX::TEX_FILTER_BOX, 0, mipChain);
	{
		const auto& chainBase = *mipChain.GetImages();
		const D3D12_RESOURCE_DESC texDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)chainBase.width,
			.Height = (UINT)chainBase.height,
			.DepthOrArraySize = 1,
			.MipLevels = (UINT16)mipChain.GetImageCount(),
			.Format = chainBase.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		};

		const CD3DX12_HEAP_PROPERTIES heapProbs{ D3D12_HEAP_TYPE_DEFAULT };
		CheckHR(device->CreateCommittedResource(
			&heapProbs,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&texture)
		))
	}
	
	const auto subresourceData = std::ranges::views::iota(0, (int)mipChain.GetImageCount()) |
		std::ranges::views::transform([&](int i) {
		const auto img = mipChain.GetImage(i, 0, 0);
		return D3D12_SUBRESOURCE_DATA{
			.pData = img->pixels,
			.RowPitch = (LONG_PTR)img->rowPitch,
			.SlicePitch = (LONG_PTR)img->slicePitch,
		};
	});
	std::vector<D3D12_SUBRESOURCE_DATA> subresourceVector(subresourceData.begin(), subresourceData.end());

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
	{
		const CD3DX12_HEAP_PROPERTIES heapProbs{ D3D12_HEAP_TYPE_UPLOAD };
		const auto uploadBufferSize = GetRequiredIntermediateSize(
			texture, 0, (UINT)subresourceVector.size()
		);
		const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		CheckHR(device->CreateCommittedResource(
			&heapProbs,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		))
	}
	
	CheckHR(commandAllocator->Reset())
	CheckHR(commandList->Reset(commandAllocator.Get(), nullptr))
	
	UpdateSubresources(
		commandList.Get(),
		texture,
		uploadBuffer.Get(),
		0,0,
		(UINT)subresourceVector.size(),
		subresourceVector.data()
	);
	
	{
		const CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texture,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		commandList->ResourceBarrier(1, &Barrier);
	}
	{
		CheckHR(commandList->Close())
		ID3D12CommandList* const commandLists[] = { commandList.Get()};
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}
	CheckHR(commandQueue->Signal(fence.Get(), ++fenceValue))
	CheckHR(fence->SetEventOnCompletion(fenceValue, fenceEvent))
	if (::WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		breakDebug;
	}
	
	ID3D12DescriptorHeap* srvHeap;
	{
		const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		};
		device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	}
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
	{
		const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
			.Format = texture->GetDesc().Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D{
				.MipLevels = texture->GetDesc().MipLevels
			},
		};
		device->CreateShaderResourceView(texture, &srvDesc, srvHandle);
	}
	
	theReturn.srvHeap = srvHeap;
	theReturn.srvResource = texture;
	theReturn.srvHandle = srvHandle;

	return theReturn;
}

TextureViewClass::TextureViewClass():
	srvHeap(nullptr), srvResource(nullptr)
{
}
