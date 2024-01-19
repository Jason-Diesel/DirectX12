#pragma once
#include "DirectXCommand.h"

#if defined(min)

#undef min

#endif

#if defined(max)

#undef max

#endif

constexpr D3D_FEATURE_LEVEL minumumFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };

class Graphics
{
public:
	Graphics();
	~Graphics();

	void beginFrame();
	void endFrame();
	void Show();
	ID3D12Device8* getDevice();
private:
	Microsoft::WRL::ComPtr<ID3D12Device8> main_device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> main_adapter;
	DirectXCommand* graphicsCommander;
private:
	IDXGIAdapter4* determineMainAdapter();
	D3D_FEATURE_LEVEL getMaxFeatureLevel(IDXGIAdapter4* main_adapter);
	
};
