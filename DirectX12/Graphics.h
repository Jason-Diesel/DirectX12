#pragma once
#include "CommonHeader.h"
#include "WindowClass.h"
#include "Vertex.h"
#include <ranges>
#include "Shader.h"
#include "Object.h"
#include "LoadImage.h"

#if defined(min)

#undef min

#endif

#if defined(max)

#undef max

#endif

#define getGFX Graphics::getInstance()
constexpr D3D_FEATURE_LEVEL minumumFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };

class Graphics
{
private:
	Graphics();
public:
	void init();
	~Graphics();
	static Graphics& getInstance();

	void updateCamera(float dt);

	//Debug here for now
	//void ClearScreen();
	void updateWindow();
	void beginFrame();
	void endFrame();
public:
	ID3D12GraphicsCommandList* getCommandList();
	ID3D12Device8* getDevice();
	ID3D12Resource* createVertexBuffer(std::vector<Vertex> vertecies);
	ID3D12Resource* createIndeciesBuffer(std::vector<uint32_t> indecies);
	TextureViewClass createTexture(const std::string& filePath);
	bool processMessages();
public:
	void changeBackgroundColor(float r, float g, float b, float a = 1.0);
	Mouse& getMouse();
	Keyboard& getKeyboard();
private:
	Microsoft::WRL::ComPtr<ID3D12Device8> main_device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> main_adapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[NrOfFrameBuffers];

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthBufferDescriptorHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	uint32_t currentBackBufferIndex;
	uint32_t rtvDescriptorSize;

	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceValue;
	

private://Settings
	//Window
	uint32_t windowWidth = 1400;
	uint32_t windowHeight = 960;
	float fov = 60;
	Window window;
	const CD3DX12_RECT scissorRect { 0,0, LONG_MAX, LONG_MAX };
	const CD3DX12_VIEWPORT viewPort { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight };
	float backGroundColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	ViewProj viewProj;
	//cameraShit
	DirectX::XMFLOAT3 camera_Position;
	DirectX::XMFLOAT2 camera_Rotation;
	void handleEvents(float dt);

	//shaders
	Shader defShader;
	std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> InputLayouts;//make a map maybe?

private:
	void setUpDirectX12();
	void initInputLayouts();
	//helperfunctions
	D3D_FEATURE_LEVEL getMaxFeatureLevel(IDXGIAdapter4* main_adapter);
	IDXGIAdapter4* determineMainAdapter();
};
