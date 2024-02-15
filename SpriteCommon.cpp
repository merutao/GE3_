#include "SpriteCommon.h"
#include <cassert>

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result{};
	dxCommon_ = dxCommon;

	//DXC������
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	ComPtr<IDxcIncludeHandler> includeHandler;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(result));

	result = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(result));

	//RootSignature
	//�쐬
	D3D12_ROOT_SIGNATURE_DESC descriptorRootSignature{};
	descriptorRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//�摜�p�̃f�B�X�N���v�^�͈�
	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; //0����n�܂�
	descriptorRange[0].NumDescriptors = 1; //����1��
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRV���g��
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offset�������v�Z

	//RootParameter�쐬�BPixelShader��Material��VertexShader��Transform
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//�F
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBV���g��
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShader���g��
	rootParameters[0].Descriptor.ShaderRegister = 0; //���W�X�^�ԍ�0���g��
	//�s��
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBV�Ŏg��
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShader�Ŏg��
	rootParameters[1].Descriptor.ShaderRegister = 0; //���W�X�^�ԍ�0���g��
	//�摜
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTable���g��
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShader�Ŏg��
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;  //Table�̒��g
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Table�ŗ��p���鐔

	descriptorRootSignature.pParameters = rootParameters; //���[�g�p�����[�^�ւ̃|�C���^
	descriptorRootSignature.NumParameters = _countof(rootParameters); //�z��̒���

	//Sampler�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //�o�C���j�A�t�B���^
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0~1�͈̔͊O�����s�[�g
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //��r���Ȃ�
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //�����������Mipmap���g��
	staticSamplers[0].ShaderRegister = 0;  //���W�X�^�ԍ�0���g��
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShader�Ŏg��

	descriptorRootSignature.pStaticSamplers = staticSamplers;
	descriptorRootSignature.NumStaticSamplers = _countof(staticSamplers);
	
	//�V���A���C�Y���ăo�C�i���ɂ���
	ComPtr<ID3D10Blob> signatureBlob;
	ComPtr<ID3D10Blob> errorBlob;
	result = D3D12SerializeRootSignature(&descriptorRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		assert(false);
	}
	//�o�C�i�������ɐ���
	
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendState�̐ݒ�
	D3D12_BLEND_DESC blendDesc{};
	//���ׂĂ̐F�v�f����������
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasiterzerState�̐ݒ�
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//����(���v���)��\�����Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//�O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shader���R���p�C������
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/SpriteVS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(SUCCEEDED(result));

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/SpritePS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(SUCCEEDED(result));

	//PipelineState
	//PSO�𐶐�����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); //RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; //InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() }; //VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() }; //PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; //RasterizerState
	//��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//���p����g�|���W(�`��)�̃^�C�v�B�O�p�`
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//�ǂ̂悤�ȉ�ʂɐF��ł����ނ̂��̐ݒ�(�C�ɂ��Ȃ��ėǂ�)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//����
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

void SpriteCommon::SpritePreDraw()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(GetPipelineState());
	//�`���ݒ�B
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

IDxcBlob* SpriteCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	HRESULT result{};

	//hlsl
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(result));

	//�ǂݍ��񂾃t�@�C��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(), //�R���p�C���Ώۂ�hlsl�t�@�C����
		L"-E",L"main", //�G���g���[�|�C���g�̎w��B��{�I��main�ȊO�ɂ͂��Ȃ�
		L"-T",profile, //ShaderProfile�̐ݒ�
		L"-Zi",L"-Qembed_debug", //�f�o�b�O�p�̏��𖄂ߍ���
		L"-Od", //�œK�����O���Ă���
		L"-Zpr", //���������C�A�E�g�͍s�D��
	};

	//���ۂ�Shader���R���p�C������
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler->Compile(
		&shaderSourceBuffer, //�ǂݍ��񂾃t�@�C��
		arguments,           //�R���p�C���I�v�V����
		_countof(arguments), //�R���p�C���I�v�V�����̐�
		includeHandler,      //include���܂܂ꂽ���X
		IID_PPV_ARGS(&shaderResult) //�R���p�C������
	);
	//�R���p�C���G���[�ł͂Ȃ�dxc���N���ł��Ȃ��Ȃǒv���I�ȏ�
	assert(SUCCEEDED(result));

	//3.�x���E�G���[���łĂ��Ȃ����m�F����
	//�x���E�G���[���o���烍�O�ɏo���Ď~�߂�
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		//�x���E�G���[�_���[�b�^�C
		assert(false);
	}

	//4.Compile���ʂ��󂯎���ĕԂ�
	//�R���p�C�����ʂ�����s�p�̃o�C�i���������擾
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//�����g��Ȃ����\�[�X�����
	shaderSource->Release();
	shaderResult->Release();
	//���s�p�o�C�i����ԋp
	return shaderBlob;
}
