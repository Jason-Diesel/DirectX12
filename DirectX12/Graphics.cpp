#include "Graphics.h"

Graphics::Graphics():
	main_device(nullptr), dxgi_factory(nullptr)
{
	if (main_device) {
		exit(-1);
	}

	uint32_t dxgi_factory_flags = 0;
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug3> debugInterface;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)) != S_OK) {
			breakDebug;
		}
		debugInterface->EnableDebugLayer();
		//debugInterface->SetEnableGPUBasedValidation(TRUE);
	}
	dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)) != S_OK) {
		breakDebug;
	}
	main_adapter.Attach(determineMainAdapter());
	if (!main_adapter.Get())
	{
		breakDebug;
	}

	D3D_FEATURE_LEVEL maxFeatureLevel{ getMaxFeatureLevel(main_adapter.Get()) };
	if (maxFeatureLevel < minumumFeatureLevel) {
		breakDebug;
	}

	if (D3D12CreateDevice(main_adapter.Get(), maxFeatureLevel, IID_PPV_ARGS(&main_device)) != S_OK) {
		breakDebug;
	}

	//should only be done in debug
	main_device->SetName(L"Main_Device");

	graphicsCommander = new DirectXCommand(main_device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

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

Graphics::~Graphics()
{
	//don't need to release beacuse ComPtr?
	delete graphicsCommander;
#ifdef _DEBUG
	{
		{
			Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue;
			if (main_device->QueryInterface(IID_PPV_ARGS(&infoQueue)) != S_OK) {
				breakDebug;
			}
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
		}
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debugDevice;
		if (main_device->QueryInterface(IID_PPV_ARGS(&debugDevice)) != S_OK)
		{
			breakDebug;
		}
		if (debugDevice->ReportLiveDeviceObjects(
			D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL
		) != S_OK)
		{
			breakDebug;
		}
	}
#endif // DEBUG
}

void Graphics::beginFrame()
{
	graphicsCommander->beginFrame();
}

void Graphics::endFrame()
{
	graphicsCommander->endFrame();
}

void Graphics::Show()
{

}

ID3D12Device8* Graphics::getDevice()
{
	return this->main_device.Get();
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
	if (D3D12CreateDevice(main_adapter, minumumFeatureLevel, IID_PPV_ARGS(&device)) != S_OK) {
		breakDebug;
	}
	if(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelInfo, sizeof(featureLevelInfo)) != S_OK) {
		breakDebug;
	}
	return featureLevelInfo.MaxSupportedFeatureLevel;

}
