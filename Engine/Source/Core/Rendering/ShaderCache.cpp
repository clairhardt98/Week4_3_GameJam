#include "pch.h"
#include "ShaderCache.h"
#include "Engine/Engine.h"
#include "Name.h"
#include <d3d11shader.h>

TArray<FString> FShaderCache::GetShaderNames(const FString& Directory) const
{
	TArray<FString> ShaderNames;

	WIN32_FIND_DATAA FindData;

	HANDLE hFind = FindFirstFileA((Directory + TEXT("\\*.hlsl")).c_char(), &FindData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			FString FileName = FindData.cFileName;
			size_t Pos = FileName.Find(TEXT(".hlsl"));

			if (Pos != std::wstring::npos)
			{
				FileName = FileName.Substr(0, Pos);
				ShaderNames.Add(FileName);
			}
		} while (FindNextFileA(hFind, &FindData) != 0);

		FindClose(hFind);
	}

	return ShaderNames;
}

bool FShaderCache::CompileShader(const FString& ShaderPath, const FString& EntryPoint, const FString& ShaderModel, ID3DBlob** ShaderBlob)
{
	ID3DBlob* ErrorMsg = nullptr;
	
	std::string EntryPointStr(EntryPoint.c_char());
	std::string ShaderModelStr(ShaderModel.c_char());

	HRESULT hr = D3DCompileFromFile(ShaderPath.c_wchar(), nullptr, nullptr, EntryPointStr.c_str(), ShaderModelStr.c_str(), 0, 0, ShaderBlob, &ErrorMsg);

	if (FAILED(hr))
	{
		if (ErrorMsg)
		{
			std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
			ErrorMsg->Release();
		}
		return false;
	}

	return true;
}

bool FShaderCache::CreateInputLayout(ID3DBlob* VertexShaderBlob, ID3D11InputLayout** InputLayout)
{
	ID3D11ShaderReflection* ShaderReflection = nullptr;
	HRESULT result = D3DReflect(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)& ShaderReflection);

	if (FAILED(result))
	{
		return false;
	}
	D3D11_SHADER_DESC ShaderDesc;
	ShaderReflection->GetDesc(&ShaderDesc);

	TArray<D3D11_INPUT_ELEMENT_DESC> InputElementDescs;

	for (UINT i = 0; i < ShaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
		ShaderReflection->GetInputParameterDesc(i, &ParamDesc);

		// 입력 요소 정보 설정
		D3D11_INPUT_ELEMENT_DESC ElementDesc;
		ElementDesc.SemanticName = ParamDesc.SemanticName;
		ElementDesc.SemanticIndex = ParamDesc.SemanticIndex;
		ElementDesc.InputSlot = 0;
		ElementDesc.AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		ElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ElementDesc.InstanceDataStepRate = 0;

		// 포맷 결정

		// 일단 이걸로 고정
		if (ParamDesc.Mask == 1)
		{
			if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (ParamDesc.Mask <= 3)
		{
			if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (ParamDesc.Mask <= 7)
		{
			if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (ParamDesc.Mask <= 15)
		{
			if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (ParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) ElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		InputElementDescs.Add(ElementDesc);
	}

	HRESULT HR = UEngine::Get().GetRenderer()->GetDevice()->CreateInputLayout(InputElementDescs.GetData(), InputElementDescs.Num(), VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), InputLayout);
	ShaderReflection->Release();

	if (FAILED(HR))
	{
		return false;
	}

	return true;
}

void FShaderCache::CreateShaders(const TArray<FString>& ShaderNames)
{
	/*
	* 컴파일된 셰이더의 바이트코드를 저장할 변수 (ID3DBlob)
	*
	* 범용 메모리 버퍼를 나타내는 형식
	*   - 여기서는 shader object bytecode를 담기위해 쓰임
	* 다음 두 메서드를 제공한다.
	*   - LPVOID GetBufferPointer
	*     - 버퍼를 가리키는 void* 포인터를 돌려준다.
	*   - SIZE_T GetBufferSize
	*     - 버퍼의 크기(바이트 갯수)를 돌려준다
	*/
	for (const FString& ShaderName : ShaderNames)
	{
		FString ShaderPath = TEXT("Shaders/") + ShaderName + TEXT(".hlsl");

		FShaderData ShaderData;

		ID3D11VertexShader* VertexShader = nullptr;
		ID3DBlob* VertexShaderBlob = nullptr;
		if (CompileShader(ShaderPath, "mainVS", "vs_5_0", &VertexShaderBlob))
		{
			UEngine::Get().GetRenderer()->GetDevice()->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), nullptr, &VertexShader);
			ShaderData.VertexShader.Attach(VertexShader);
		}

		ID3D11PixelShader* PixelShader = nullptr;
		ID3DBlob* PixelShaderBlob = nullptr;
		if (CompileShader(ShaderPath, "mainPS", "ps_5_0", &PixelShaderBlob))
		{
			UEngine::Get().GetRenderer()->GetDevice()->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), nullptr, &PixelShader);
			ShaderData.PixelShader.Attach(PixelShader);
		}

		// !TODO : 다른 셰이더 컴파일은 여기에 추가


		ID3D11InputLayout* InputLayout = nullptr;
		if (VertexShaderBlob)
		{
			CreateInputLayout(VertexShaderBlob, &InputLayout);
			ShaderData.InputLayout.Attach(InputLayout);
		}

		ShaderDatas.Add(ShaderName, ShaderData);
	}
}

ID3D11VertexShader* FShaderCache::GetVertexShader(const FString& ShaderName) const
{
	if (ShaderDatas.Contains(ShaderName))
	{
		return ShaderDatas[ShaderName].VertexShader.Get();
	}
	return nullptr;
}

ID3D11PixelShader* FShaderCache::GetPixelShader(const FString& ShaderName) const
{
	if (ShaderDatas.Contains(ShaderName))
	{
		return ShaderDatas[ShaderName].PixelShader.Get();
	}
	return nullptr;
}

ID3D11InputLayout* FShaderCache::GetInputLayout(const FString& ShaderName) const
{
	if (ShaderDatas.Contains(ShaderName))
	{
		return ShaderDatas[ShaderName].InputLayout.Get();
	}
	return nullptr;
}
