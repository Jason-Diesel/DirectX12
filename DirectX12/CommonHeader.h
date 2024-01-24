#pragma once

#include <initguid.h>
/////DX12/////
#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h" 
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "dxgi.lib")

#include <d3dcompiler.h>
#include <DirectXMath.h>
//#include <wrl.h>
///////////////
#include "Debugger.h"
#include "HelperFunctions.h"


class TextureViewClass {
public:
	TextureViewClass();
	ID3D12DescriptorHeap* srvHeap;
	ID3D12Resource* srvResource;
private:

};

#define NrOfFrameBuffers 2