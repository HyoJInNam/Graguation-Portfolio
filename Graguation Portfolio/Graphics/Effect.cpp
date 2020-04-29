#include "Effect.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Effect::Effect(ID3D11Device * device, ID3D11DeviceContext * deviceContext, wstring file, bool bUseCS)
	: file(file)
{
	this->device = device;
	this->deviceContext = deviceContext;
	initialStateBlock = new StateBlock();
	{
		this->deviceContext->RSGetState(&initialStateBlock->RSRasterizerState);
		this->deviceContext->OMGetBlendState(&initialStateBlock->OMBlendState, initialStateBlock->OMBlendFactor, &initialStateBlock->OMSampleMask);
		this->deviceContext->OMGetDepthStencilState(&initialStateBlock->OMDepthStencilState, &initialStateBlock->OMStencilRef);
	}

	CreateEffect();
}

Effect::~Effect()
{
	for (Technique& temp : techniques)
	{
		for (Pass& pass : temp.Passes)
		{
			SAFE_RELEASE(pass.InputLayout);
		}
	}

	SAFE_DELETE(initialStateBlock);
	SAFE_RELEASE(effect);
}

void Effect::CreateEffect()
{
	ID3DBlob* fxBlob = NULL;
	ID3DBlob* error;

	INT flag = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	HRESULT hr = D3DCompileFromFile(file.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, NULL, "fx_5_0", flag, NULL, &fxBlob, &error);
	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}

	hr = D3DX11CreateEffectFromMemory(fxBlob->GetBufferPointer(), fxBlob->GetBufferSize(), 0, device, &effect);
	assert(SUCCEEDED(hr));
	//Effects::GetEffect(file, &fxBlob, &effect);

	effect->GetDesc(&effectDesc);
	for (UINT t = 0; t < effectDesc.Techniques; t++)
	{
		Technique technique;
		technique.ITechnique = effect->GetTechniqueByIndex(t);
		technique.ITechnique->GetDesc(&technique.Desc);
		technique.Name = StringHelper::StringToWide(technique.Desc.Name);
		for (UINT p = 0; p < technique.Desc.Passes; p++)
		{
			Pass pass(this->deviceContext);
			pass.IPass = technique.ITechnique->GetPassByIndex(p);
			pass.IPass->GetDesc(&pass.Desc);
			pass.Name = StringHelper::StringToWide(pass.Desc.Name);
			pass.IPass->GetVertexShaderDesc(&pass.PassVsDesc);
			pass.PassVsDesc.pShaderVariable->GetShaderDesc(pass.PassVsDesc.ShaderIndex, &pass.EffectVsDesc);

			for (UINT s = 0; s < pass.EffectVsDesc.NumInputSignatureEntries; s++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC desc;

				//	GetInputSignatureElementDesc : 이전에 reflection이랑 같은 기능
				HRESULT hr = pass.PassVsDesc.pShaderVariable->GetInputSignatureElementDesc(pass.PassVsDesc.ShaderIndex, s, &desc);
				assert(SUCCEEDED(hr));

				pass.SignatureDescs.push_back(desc);
			}

			pass.InputLayout = CreateInputLayout(fxBlob, &pass.EffectVsDesc, pass.SignatureDescs);
			pass.StateBlock = initialStateBlock;

			technique.Passes.push_back(pass);
		}

		techniques.push_back(technique);
	}
}

ID3D11InputLayout * Effect::CreateInputLayout(ID3DBlob * fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (D3D11_SIGNATURE_PARAMETER_DESC& paramDesc : params)
	{
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string name = paramDesc.SemanticName;

		if (name == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		if (name == "INSTANCE") {
			elementDesc.InputSlot = 1;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		inputLayoutDesc.push_back(elementDesc);
	}

	const void* pCode = effectVsDesc->pBytecode;
	UINT pCodeSize = effectVsDesc->BytecodeLength;

	if (inputLayoutDesc.size() > 0)
	{
		ID3D11InputLayout* inputLayout = NULL;
		HRESULT hr = this->device->CreateInputLayout
		(
			&inputLayoutDesc[0]
			, inputLayoutDesc.size()
			, pCode
			, pCodeSize
			, &inputLayout
		);
		assert(SUCCEEDED(hr));

		return inputLayout;
	}

	return NULL;
}

void Effect::Pass::Draw(UINT vertexCount, UINT startVertexLocation)
{
	BeginDraw();
	{
		deviceContext->Draw(vertexCount, startVertexLocation);
	}
	EndDraw();
}

void Effect::Pass::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	BeginDraw();
	{
		deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
	EndDraw();
}

void Effect::Pass::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		deviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}
	EndDraw();
}

void Effect::Pass::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		deviceContext->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startIndexLocation);
	}
	EndDraw();
}

void Effect::Pass::BeginDraw()
{
	IPass->ComputeStateBlockMask(&StateBlockMask);

	deviceContext->IASetInputLayout(InputLayout);
	IPass->Apply(0, deviceContext);
}

void Effect::Pass::EndDraw()
{
	ID3D11ShaderResourceView * nullSRV[16] = { 0 };

	deviceContext->VSSetShaderResources(0, 16, nullSRV);
	deviceContext->GSSetShaderResources(0, 16, nullSRV);
	deviceContext->DSSetShaderResources(0, 16, nullSRV);
	deviceContext->HSSetShaderResources(0, 16, nullSRV);
	deviceContext->PSSetShaderResources(0, 16, nullSRV);

	if (StateBlockMask.GS == 1)
		deviceContext->GSSetShader(NULL, NULL, 0);

	if (StateBlockMask.RSRasterizerState == 1)
		deviceContext->RSSetState(StateBlock->RSRasterizerState);

	if (StateBlockMask.OMDepthStencilState == 1)
		deviceContext->OMSetDepthStencilState(StateBlock->OMDepthStencilState, StateBlock->OMStencilRef);

	if (StateBlockMask.OMBlendState == 1)
		deviceContext->OMSetBlendState(StateBlock->OMBlendState, StateBlock->OMBlendFactor, StateBlock->OMSampleMask);

	deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
}

void Effect::Pass::Dispatch(UINT x, UINT y, UINT z)
{
	IPass->Apply(0, deviceContext);
	deviceContext->Dispatch(x, y, z);

	ID3D11ShaderResourceView * null[16] = { 0 };
	deviceContext->CSSetShaderResources(0, 16, null);

	ID3D11UnorderedAccessView* nullUav[16] = { 0 };
	deviceContext->CSSetUnorderedAccessViews(0, 16, nullUav, NULL);

	deviceContext->CSSetShader(NULL, NULL, 0);
}

ID3DX11EffectVariable * Effect::Variable(string name)
{
	return effect->GetVariableByName(name.c_str());
}

ID3DX11EffectScalarVariable * Effect::AsScalar(string name)
{
	return effect->GetVariableByName(name.c_str())->AsScalar();
}

ID3DX11EffectVectorVariable * Effect::AsVector(string name)
{
	return effect->GetVariableByName(name.c_str())->AsVector();
}

ID3DX11EffectMatrixVariable * Effect::AsMatrix(string name)
{
	return effect->GetVariableByName(name.c_str())->AsMatrix();
}

ID3DX11EffectStringVariable * Effect::AsString(string name)
{
	return effect->GetVariableByName(name.c_str())->AsString();
}

ID3DX11EffectShaderResourceVariable * Effect::AsSRV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsShaderResource();
}

ID3DX11EffectRenderTargetViewVariable * Effect::AsRTV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsRenderTargetView();
}

ID3DX11EffectDepthStencilViewVariable * Effect::AsDSV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsDepthStencilView();
}

ID3DX11EffectUnorderedAccessViewVariable * Effect::AsUAV(string name)
{
	return effect->GetVariableByName(name.c_str())->AsUnorderedAccessView();
}

ID3DX11EffectConstantBuffer * Effect::AsConstantBuffer(string name)
{
	return effect->GetConstantBufferByName(name.c_str())->AsConstantBuffer();
}

ID3DX11EffectShaderVariable * Effect::AsShader(string name)
{
	return effect->GetVariableByName(name.c_str())->AsShader();
}

ID3DX11EffectBlendVariable * Effect::AsBlend(string name)
{
	return effect->GetVariableByName(name.c_str())->AsBlend();
}

ID3DX11EffectDepthStencilVariable * Effect::AsDepthStencil(string name)
{
	return effect->GetVariableByName(name.c_str())->AsDepthStencil();
}

ID3DX11EffectRasterizerVariable * Effect::AsRasterizer(string name)
{
	return effect->GetVariableByName(name.c_str())->AsRasterizer();
}

ID3DX11EffectSamplerVariable * Effect::AsSampler(string name)
{
	return effect->GetVariableByName(name.c_str())->AsSampler();
}

void Effect::Technique::Draw(UINT pass, UINT vertexCount, UINT startVertexLocation)
{
	Passes[pass].Draw(vertexCount, startVertexLocation);
}

void Effect::Technique::DrawIndexed(UINT pass, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	Passes[pass].DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Effect::Technique::DrawInstanced(UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	Passes[pass].DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Effect::Technique::DrawIndexedInstanced(UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	Passes[pass].DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Effect::Technique::Dispatch(UINT pass, UINT x, UINT y, UINT z)
{
	Passes[pass].Dispatch(x, y, z);
}

void Effect::Draw(UINT technique, UINT pass, UINT vertexCount, UINT startVertexLocation)
{
	techniques[technique].Passes[pass].Draw(vertexCount, startVertexLocation);
}

void Effect::DrawIndexed(UINT technique, UINT pass, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	techniques[technique].Passes[pass].DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Effect::DrawInstanced(UINT technique, UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	techniques[technique].Passes[pass].DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Effect::DrawIndexedInstanced(UINT technique, UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	techniques[technique].Passes[pass].DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Effect::Dispatch(UINT technique, UINT pass, UINT x, UINT y, UINT z)
{
	techniques[technique].Passes[pass].Dispatch(x, y, z);
}
