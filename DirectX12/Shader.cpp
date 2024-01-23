#include "Shader.h"

Shader::Shader()
{
	
}

void Shader::init(ID3D12Device8* device)
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	if (const HRESULT hr = D3D12SerializeRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,//Should be change to 1_1?
		&signatureBlob, &errorBlob); FAILED(hr)) {
		if (errorBlob) {
			const char* errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
			std::cout << "Error: " << errorBufferPtr << std::endl;
			breakDebug;
		}
		CheckHR(hr)
	}
	CheckHR(device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)
	))

	{
		struct PipelineStateStream {
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT inputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY topology;
			CD3DX12_PIPELINE_STATE_STREAM_VS vs;
			CD3DX12_PIPELINE_STATE_STREAM_PS ps;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS rtvFormat;
		}pipelineStateStream;

		const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};
		std::string filePath;
#ifdef _DEBUG
		filePath = "../x64/Debug/";
#else
		filePath = "../x64/Release/";
#endif // DEBUG

		Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

		D3DReadFileToBlob(createStringToWString(filePath + "VertexShader.cso").c_str(), &vertexShader);
		D3DReadFileToBlob(createStringToWString(filePath + "PixelShader.cso").c_str(), &pixelShader);

		pipelineStateStream.rootSignature = rootSignature.Get();
		pipelineStateStream.inputLayout = { inputLayout, (uint32_t)_countof(inputLayout) };//Hopefully this is okay?
		pipelineStateStream.topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.vs = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		pipelineStateStream.ps = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		pipelineStateStream.rtvFormat = {
			.RTFormats { DXGI_FORMAT_R8G8B8A8_UNORM },
			.NumRenderTargets = 1,
		};

		const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		CheckHR(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)))
		}
}
