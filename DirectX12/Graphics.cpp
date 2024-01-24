#include "Graphics.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

Graphics::Graphics():
	main_device(nullptr), dxgi_factory(nullptr)
{
	window.Initialize(GetModuleHandle(NULL), "Penis", "a", windowWidth, windowHeight);


	{
		const DirectX::FXMVECTOR camPosition = DirectX::XMVectorSet(0, 0, -5, 1);
		const DirectX::FXMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
		const DirectX::FXMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
		viewProj.view = DirectX::XMMatrixLookAtLH(camPosition, focusPoint, upDirection);
		const float aspectRatio = float(windowWidth) / float(windowHeight);
		viewProj.proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, 0.01f, 2000.f);
	}
	initInputLayouts();
	setUpDirectX12();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window.getRenderWindow().getHandle());
	ImGui_ImplDX12_Init(
		main_device.Get(),
		NrOfFrameBuffers,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		rtvDescriptorHeap.Get(),
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		rtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);

	std::vector<ConstantBuffer> bufferForDefShader;
	bufferForDefShader.push_back(ConstantBuffer{ sizeof(ViewProj), 0, D3D12_SHADER_VISIBILITY_VERTEX });//maybe can be change to all
	bufferForDefShader.push_back(ConstantBuffer{ sizeof(Transform), 1, D3D12_SHADER_VISIBILITY_VERTEX });
	defShader.init(
		getDevice(),
		bufferForDefShader,
		1,
		InputLayouts[0]
	);

#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue;
		if (main_device->QueryInterface(IID_PPV_ARGS(&infoQueue)) != S_OK) {
			breakDebug;
		}
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

	}
#endif // DEBUG

}

void Graphics::init()
{
	std::cout << "Create graphics" << std::endl;//does this work in release?
}

Graphics::~Graphics()
{
	ImGui::DestroyContext();
#ifdef _DEBUG
	{
		//{
		//	Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice;
		//	CheckHR(main_device->QueryInterface(IID_PPV_ARGS(&debugDevice)))
		//	CheckHR(debugDevice->ReportLiveDeviceObjects(
		//		D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL
		//	))
		//}
		//{
		//	Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue;
		//	CheckHR(main_device->QueryInterface(IID_PPV_ARGS(&infoQueue)))
		//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
		//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
		//}
	}
#endif // DEBUG

	commandQueue->Signal(fence.Get(), ++fenceValue);
	fence->SetEventOnCompletion(fenceValue, fenceEvent);
	if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED) {
		breakDebug;
	}
}

Graphics& Graphics::getInstance()
{
	static Graphics gfxInstance;
	return gfxInstance;
}

void Graphics::updateWindow()
{
	//window.Update();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	bool t = true;
	ImGui::ShowDemoWindow(&t);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}

void Graphics::beginFrame()
{
	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer = backBuffers[currentBackBufferIndex];

	CheckHR(commandAllocator.Get()->Reset())
	CheckHR(commandList.Get()->Reset(commandAllocator.Get(), nullptr))
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv {
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			(int)currentBackBufferIndex, rtvDescriptorSize
	};

	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		commandList->ResourceBarrier(1, &barrier);
		commandList->ClearRenderTargetView(rtv, backGroundColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}
	
	commandList->SetPipelineState(defShader.pipelineState.Get());
	commandList->SetGraphicsRootSignature(defShader.rootSignature.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->RSSetViewports(1, &viewPort);
	commandList->RSSetScissorRects(1, &scissorRect);

	commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);

	commandList->SetGraphicsRoot32BitConstants(0, sizeof(ViewProj) / 4, &viewProj, 0);
}

void Graphics::endFrame()
{
	Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer = backBuffers[currentBackBufferIndex];
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
		);
		commandList->ResourceBarrier(1, &barrier);
	}
	{
		CheckHR(commandList->Close())
		ID3D12CommandList* const commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}
	CheckHR(commandQueue->Signal(fence.Get(), ++fenceValue))
	CheckHR(swapChain->Present(1, 0))
	CheckHR(fence->SetEventOnCompletion(fenceValue, fenceEvent))

	if (::WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		breakDebug;
	}
}

ID3D12GraphicsCommandList* Graphics::getCommandList()
{
	return this->commandList.Get();
}

ID3D12Device8* Graphics::getDevice()
{
	return this->main_device.Get();
}

ID3D12Resource* Graphics::createVertexBuffer(std::vector<Vertex> vertecies)
{
	ID3D12Resource* vertexBuffer = nullptr;
	uint32_t nrOfVertecies = (uint32_t)vertecies.size();
	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nrOfVertecies * sizeof(Vertex));
		CheckHR(getGFX.getDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer)
		))
	}
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nrOfVertecies * sizeof(Vertex));
		CheckHR(getGFX.getDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexUploadBuffer)
		))
	}
	{
		Vertex* mappedVertexData = nullptr;
		CheckHR(vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)))
		std::ranges::copy(vertecies, mappedVertexData);
		vertexUploadBuffer->Unmap(0, nullptr);
	}

	CheckHR(commandAllocator->Reset())
	CheckHR(commandList->Reset(commandAllocator.Get(), nullptr))
	commandList->CopyResource(vertexBuffer, vertexUploadBuffer.Get());
	{
		const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		);
		commandList->ResourceBarrier(1, &barrier);
	}
	CheckHR(commandList->Close())
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	CheckHR(commandQueue->Signal(fence.Get(), ++fenceValue))
	CheckHR(fence->SetEventOnCompletion(fenceValue, fenceEvent))
	if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		breakDebug;
	}
	return vertexBuffer;
}

ID3D12Resource* Graphics::createIndeciesBuffer(std::vector<uint32_t> indecies)
{
	ID3D12Resource* indeciesBuffer = nullptr;
	uint32_t nrOfindecies = (uint32_t)indecies.size();
	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nrOfindecies * sizeof(uint32_t));
		CheckHR(getGFX.getDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&indeciesBuffer)
		))
	}
	Microsoft::WRL::ComPtr<ID3D12Resource> indeciesUploadBuffer;
	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(nrOfindecies * sizeof(uint32_t));
		CheckHR(getGFX.getDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indeciesUploadBuffer)
		))
	}
	{
		uint32_t* mappedIndeciesData = nullptr;
		CheckHR(indeciesUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndeciesData)))
			std::ranges::copy(indecies, mappedIndeciesData);
		indeciesUploadBuffer->Unmap(0, nullptr);
	}

	CheckHR(commandAllocator->Reset())
	CheckHR(commandList->Reset(commandAllocator.Get(), nullptr))
	commandList->CopyResource(indeciesBuffer, indeciesUploadBuffer.Get());
	{
		const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			indeciesBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER
		);
		commandList->ResourceBarrier(1, &barrier);
	}
	CheckHR(commandList->Close())
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	CheckHR(commandQueue->Signal(fence.Get(), ++fenceValue))
		CheckHR(fence->SetEventOnCompletion(fenceValue, fenceEvent))
		if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
		{
			breakDebug;
		}
	return indeciesBuffer;
}

TextureViewClass Graphics::createTexture(const std::string& filePath)
{
	return CreateTexture(
		filePath,
		main_device.Get(),
		commandAllocator.Get(),
		commandList.Get(),
		commandQueue.Get(),
		fence.Get(),
		fenceValue,
		fenceEvent
		);
}

bool Graphics::processMessages()
{
	return window.ProcessMessages();
}

void Graphics::changeBackgroundColor(float r, float g, float b, float a)
{
	backGroundColor[0] = r;
	backGroundColor[1] = g;
	backGroundColor[2] = b;
	backGroundColor[3] = a;
}

Mouse& Graphics::getMouse()
{
	return this->window.getMouse();
}

Keyboard& Graphics::getKeyboard()
{
	return this->window.getKeyboard();
}

void Graphics::setUpDirectX12()
{
	if (main_device) {
		exit(-1);
	}

	uint32_t dxgi_factory_flags = 0;
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface;
		CheckHR(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))
		debugInterface->EnableDebugLayer();
		debugInterface->SetEnableGPUBasedValidation(TRUE);
	}
	dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	CheckHR(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)))

		main_adapter.Attach(determineMainAdapter());
	if (!main_adapter.Get())
	{
		breakDebug;
	}

	D3D_FEATURE_LEVEL maxFeatureLevel{ getMaxFeatureLevel(main_adapter.Get()) };
	if (maxFeatureLevel < minumumFeatureLevel) {
		breakDebug;
	}

	CheckHR(D3D12CreateDevice(main_adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&main_device)))

	//should only be done in debug
	main_device->SetName(L"Main_Device");

	{
		D3D12_COMMAND_QUEUE_DESC desc;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CheckHR(main_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)))
	}

	{
		DXGI_SWAP_CHAIN_DESC1 desc;
		desc.Width = windowWidth;
		desc.Height = windowHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = NrOfFrameBuffers;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0;// DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;//is this v-sync off?

		Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
		CheckHR(dxgi_factory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			window.getRenderWindow().getHandle(),
			&desc,
			nullptr, nullptr,
			&tempSwapChain
		))
			CheckHR(tempSwapChain.As(&swapChain))
	}

	//rtv descriptor heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = NrOfFrameBuffers
		};
		CheckHR(main_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)))
	}
	rtvDescriptorSize = main_device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (uint32_t i = 0; i < NrOfFrameBuffers; i++)
		{
			CheckHR(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])))
				main_device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
	}

	{
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			windowWidth, windowHeight,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		const D3D12_CLEAR_VALUE clearValue = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = {1.0f, 0}
		};
		CheckHR(main_device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&depthBuffer)
		))
	}

	{
		const D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1
		};
		main_device->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(&depthBufferDescriptorHeap)
		);
		
		dsvHandle = { depthBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		main_device->CreateDepthStencilView(depthBuffer.Get(), nullptr, dsvHandle);
	}


	CheckHR(main_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))

	CheckHR(main_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))
	CheckHR(commandList->Close())

	fenceValue = 0;
	CheckHR(main_device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))

	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent)
	{
		GetLastError();
		breakDebug;
	}
}

void Graphics::initInputLayouts()
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> defLayout = {
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	InputLayouts.push_back(defLayout);
}

IDXGIAdapter4* Graphics::determineMainAdapter()
{
	IDXGIAdapter4* adapter = nullptr;

	for (uint32_t i = 0;
		dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		if (SUCCEEDED(D3D12CreateDevice(adapter, minumumFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
			return adapter;
		}
		HF::release(adapter);
	}

	return nullptr;
}

D3D_FEATURE_LEVEL Graphics::getMaxFeatureLevel(IDXGIAdapter4* main_adapter)
{
	constexpr D3D_FEATURE_LEVEL featureLevels[]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1,
	};
	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelInfo{};
	featureLevelInfo.NumFeatureLevels = _countof(featureLevels);
	featureLevelInfo.pFeatureLevelsRequested = featureLevels;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	CheckHR(D3D12CreateDevice(main_adapter, minumumFeatureLevel, IID_PPV_ARGS(&device)))
	CheckHR(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)))
	return featureLevelInfo.MaxSupportedFeatureLevel;

}
