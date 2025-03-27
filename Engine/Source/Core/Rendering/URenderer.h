#pragma once

#define _TCHAR_DEFINED

#include "BufferCache.h"
#include "ShaderCache.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Primitive/PrimitiveVertices.h"
#include "RendererDefine.h"
#include "Constants.h"

struct FVertexSimple;
struct FVector4;

class ACamera;

class URenderer
{
public:
	/** Renderer를 초기화 합니다. */
	void Create(HWND hWindow);

	/** Renderer에 사용된 모든 리소스를 해제합니다. */
	void Release();

	void CreateShader();

	void ReleaseShader();

	void CreateConstantBuffer();

	void ReleaseConstantBuffer();

	/** 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력 */
	void SwapBuffer();

	/** 렌더링 파이프라인을 준비 합니다. */
	void PrepareRender();

	/** 셰이더를 준비 합니다. */
	void PrepareMainShader() const;

	void RenderPrimitive(class UPrimitiveComponent* PrimitiveComp);

	/**
	 * Buffer에 있는 Vertex를 그립니다.
	 * @param pBuffer 렌더링에 사용할 버텍스 버퍼에 대한 포인터
	 * @param numVertices 버텍스 버퍼에 저장된 버텍스의 총 개수
	 */
	void RenderPrimitiveInternal(ID3D11Buffer* pBuffer, ID3D11Buffer* IndexBuffer, UINT numVertices) const;

	void RenderBox(const class FBox& Box, const FVector4& Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f));

	void RenderMesh(class UMeshComponent* MeshComp);

	void PrepareMesh();

	void PrepareMeshShader();
	
	void PrepareWorldGrid();
	
	void RenderWorldGrid();
	
    /**
     * 정점 데이터로 Vertex Buffer를 생성합니다.
     * @param Vertices 버퍼로 변환할 정점 데이터 배열의 포인터
     * @param ByteWidth 버퍼의 총 크기 (바이트 단위)
     * @return 생성된 버텍스 버퍼에 대한 ID3D11Buffer 포인터, 실패 시 nullptr
     *
     * @note 이 함수는 D3D11_USAGE_IMMUTABLE 사용법으로 버퍼를 생성합니다.
     */
    ID3D11Buffer* CreateImmutableVertexBuffer(const FVertexSimple* Vertices, UINT ByteWidth) const;
	// TODO: 버텍스 정보를 void* 타입으로 받으면 오버로딩 안해도 됨.
	ID3D11Buffer* CreateImmutableVertexBuffer(const FStaticMeshVertex* Vertices, UINT ByteWidth) const;
	ID3D11Buffer* CreateDynamicVertexBuffer(UINT ByteWidth);
	ID3D11Buffer* CreateIndexBuffer(const UINT* Indices, UINT ByteWidth) const;

	/** Buffer를 해제합니다. */
	void ReleaseVertexBuffer(ID3D11Buffer* pBuffer) const;

	/** Constant Data를 업데이트 합니다. */
	void UpdateObjectConstantBuffer(const ConstantUpdateInfo& UpdateInfo) const;

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	/** View 변환 Matrix를 업데이트 합니다. */
	void UpdateViewMatrix(const FTransform& CameraTransform);

	/** Projection 변환 Matrix를 업데이트 합니다. */
	void UpdateProjectionMatrix(ACamera* Camera);

	void OnUpdateWindowSize(int Width, int Height);

    void GetPrimitiveLocalBounds(EPrimitiveType Type, FVector& OutMin, FVector& OutMax);

	void SetRenderMode(EViewModeIndex Type);

	// World Grid
	HRESULT GenerateWorldGridVertices(int32 WorldGridCellPerSide);

	bool IsOccluded();

	void AddDebugLine(FVector Start, FVector End, FVector Color, float Time);

	void RenderDebugLines(float DelatTime);

	// Texture
	void CreateTextureBuffer();

	void CreateTextureBlendState();

	void PrepareBillboard();

	void RenderBillboard();

	void UpdateTextureConstantBuffer(const FMatrix& World, float U, float V, float TotalCols, float TotalRows, FVector4 PartyMode);

	// Text Billboard
	// 버텍스 버퍼를 제외한 나머지 렌더링에 필요한 리소스는
	// Texture에서 사용하던 것을 재사용
	void CreateTextVertexBuffer(int32 InVertexCount);

	void UpdateTextVertexBuffer(const std::wstring& TextString, float TotalCols, float TotalRows);

	void RenderTextBillboard(const std::wstring& TextString, float TotalCols, float TotalRows);

	void UpdateTextConstantBuffer(const FMatrix& World);

	void PrepareTextBillboard();

protected:
	/** Direct3D Device 및 SwapChain을 생성합니다. */
	void CreateDeviceAndSwapChain(HWND hWindow);

	/** Direct3D Device 및 SwapChain을 해제합니다.  */
	void ReleaseDeviceAndSwapChain();

	/** 프레임 버퍼를 생성합니다. */
	void CreateFrameBuffer();

	/** 뎁스 스텐실 버퍼를 생성합니다. */
	void CreateDepthStencilBuffer();

	/** 뎁스 스텐실 상태를 생성합니다. */
	void CreateDepthStencilState();
	
	/** 프레임 버퍼를 해제합니다. */
	void ReleaseFrameBuffer();

	/** 뎁스 스텐실 버퍼를 해제합니다. */
	void ReleaseDepthStencilBuffer();

	/** 뎁스 스텐실 상태를 해제합니다. */
	void ReleaseDepthStencilState();

	/** 뎁스 스텐실 버퍼 및 상태를 해제합니다 */
	void ReleaseDepthStencilResources();
	
	/** 레스터라이즈 상태를 생성합니다. */
	void CreateRasterizerState();

	/** 레스터라이저 상태를 해제합니다. */
	void ReleaseRasterizerState();

	void CreateBufferCache();

	void CreateShaderCache();

	void InitMatrix();

	void CreateBlendState();

	void ReleaseBlendState();

	void CreateTextureSamplerState();

	void ReleaseTextureSamplerState();
	/**
	 * 필요한 선의 개수에 맞게 버퍼 크기 조정
	 * @param LineNum 필요한 선의 개수
	 */
	void AdjustDebugLineVertexBuffer(uint32 LineNum);
	
	void UpdateDebugLines(float DeltaTime);

	void PrepareDebugLines();

private:
	/**
	 * @param NewSize 정점 개수. 버퍼의 크기에 비례.
	 */
	void CreateDebugLineVertexBuffer(uint32 NewSize);

protected:
	HWND hWnd = nullptr;                                    // 렌더러가 사용할 윈도우 핸들
	// Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
	ID3D11Device* Device = nullptr;                         // GPU와 통신하기 위한 Direct3D 장치
	ID3D11Debug* debugDevice = nullptr;						// 디버깅을 위한 디버그 장치
	ID3D11InfoQueue* infoQueue = nullptr;					// 디버깅을 위한 정보 큐
	ID3D11DeviceContext* DeviceContext = nullptr;           // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr;                    // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인
	bool bSwapChainOccluded = false;                        // 스왑 체인이 가려졌는지 여부

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr;                 // 화면 출력용 텍스처
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
	ID3D11RasterizerState* RasterizerState_Solid = nullptr; // Solid 레스터라이즈 상태
	ID3D11RasterizerState* RasterizerState_Wireframe = nullptr; // Wireframe 레스터라이즈 상태

	ID3D11RasterizerState** CurrentRasterizerState = nullptr; // 현재 사용중인 레스터라이즈 상태
	EViewModeIndex CurrentRasterizerStateType = EViewModeIndex::ERS_Solid; // 현재 사용중인 레스터라이즈 상태 타입

	ID3D11Buffer* CbChangeEveryObject = nullptr;                 // 쉐이더에 데이터를 전달하기 위한 상수 버퍼
	ID3D11Buffer* CbChangeOnCameraMove = nullptr;                  // 쉐이더에 데이터를 전달하기 위한 상수 버퍼
	ID3D11Buffer* CbChangeOnResizeAndFov = nullptr;              // 쉐이더에 데이터를 전달하기 위한 상수 버퍼

	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (RGBA)
	D3D11_VIEWPORT ViewportInfo = {};                       // 렌더링 영역을 정의하는 뷰포트 정보

	ID3D11BlendState* GridBlendState = nullptr;
	
	uint32 Stride = 0;                                // Vertex 버퍼의 각 요소 크기
	uint32 GridStride = 0;
	// Depth Stenil Buffer
	ID3D11Texture2D* DepthStencilBuffer = nullptr;          // DepthStencil버퍼 역할을 하는 텍스쳐
	ID3D11DepthStencilView* DepthStencilView = nullptr;     // DepthStencil버퍼를 렌더 타겟으로 사용하는 뷰
	ID3D11DepthStencilState* DepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)
	ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // 기즈모용 스텐실 스테이트. Z버퍼 테스트 하지않고 항상 앞에렌더
	
	// Buffer Cache
	std::unique_ptr<class FBufferCache> BufferCache;
	ID3D11Buffer* DynamicVertexBuffer = nullptr;

	// Shader Cache
	std::unique_ptr<class FShaderCache> ShaderCache;

	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;

	D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	// World Grid
	uint32 GridVertexNum = 0;
	ID3D11Buffer* GridVertexBuffer = nullptr;

	// Texture Shader & Buffer;
	ID3D11Buffer* TextureVertexBuffer = nullptr;              // 텍스처 버퍼
	ID3D11VertexShader* TextureVertexShader = nullptr;      // 텍스처용 버텍스 쉐이더
	ID3D11PixelShader* TexturePixelShader = nullptr;        // 텍스처용 픽셀 쉐이더
	ID3D11InputLayout* TextureInputLayout = nullptr;        // 텍스처용 인풋 레이아웃
	ID3D11Buffer* TextureConstantBuffer = nullptr;
	ID3D11SamplerState* SamplerState = nullptr;				// 텍스쳐 샘플러 스테이트
	ID3D11BlendState* TextureBlendState = nullptr;

	// TextVertexBuffer
	ID3D11Buffer* TextVertexBuffer = nullptr;

private:
	// Debug Line
	struct FDebugLineInfo
	{
		FVector Start;
		FVector End;
		FVector Color;
		float Time;
	};

	TArray<FDebugLineInfo> DebugLines;

	ID3D11Buffer* DebugLineVertexBuffer = nullptr;
	// 현재 버퍼의 크기로 그릴 수 있는 선의 최대 개수로, 그려야 할 선의 개수와는 다름에 주의.
	uint32 DebugLineCurrentMaxNum = 0;
	uint32 DebugLineNumStep = 5;
	
#pragma region picking
protected:
	// 피킹용 버퍼들
	ID3D11Texture2D* PickingFrameBuffer = nullptr;                 // 화면 출력용 텍스처
	ID3D11Texture2D* PickingFrameBufferStaging = nullptr;
	ID3D11Texture2D* PickingDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* PickingDepthStencilView = nullptr;
	ID3D11RenderTargetView* PickingFrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
	ID3D11Buffer* ConstantPickingBuffer = nullptr;                 // 뷰 상수 버퍼
	FLOAT PickingClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //
	ID3D11PixelShader* PickingPixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더

	ID3D11DepthStencilState* IgnoreDepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)

public:
	//피킹용 함수들	
	void CreatePickingFrameBuffer();
	void ReleasePickingFrameBuffer();
	void PrepareZIgnore();
	void PreparePicking();
	void PreparePickingShader() const;
	void UpdateConstantPicking(FVector4 UUIDColor) const;

	void PrepareMain();
	void PrepareMainShader();

	FVector4 GetPixel(int32 X, int32 Y);

	void RenderPickingTexture();
	FMatrix GetProjectionMatrix() const;
	
#pragma endregion picking
};
