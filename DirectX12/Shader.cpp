#include "Shader.h"

Shader::Shader()
{
	
}

void Shader::init(ID3D12Device8* device, std::vector<ConstantBuffer> constantBuffers, uint32_t nrOfTexture, std::vector<D3D12_INPUT_ELEMENT_DESC> customInputLayout)
{
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters{};
	rootParameters.resize(constantBuffers.size() + nrOfTexture);
	
	int index = 0;
	for (int i = 0; i < constantBuffers.size(); i++)
	{
		rootParameters[index++].InitAsConstants(
			constantBuffers[i].size / 4,
			constantBuffers[i].shaderRigister,
			0, 
			constantBuffers[i].shaderVisablility
		);
	}
	//UGLY AF
	for (uint32_t i = 0; i < nrOfTexture; i++)
	{
		const CD3DX12_DESCRIPTOR_RANGE descRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i };
		rootParameters[index++].InitAsDescriptorTable(1, &descRange);
	}
	
	const CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR };
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	rootSignatureDesc.Init(
		(uint32_t)rootParameters.size(), rootParameters.data(),
		1, &staticSampler,
		rootSignatureFlags
	);

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
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS rtvFormat;
		}pipelineStateStream;

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
		pipelineStateStream.inputLayout = { customInputLayout.data(), (uint32_t)customInputLayout.size()};//Hopefully this is okay?
		pipelineStateStream.topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.vs = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		pipelineStateStream.ps = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
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
