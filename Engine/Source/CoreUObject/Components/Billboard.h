#pragma once
#include "CoreUObject/Components/PrimitiveComponent.h"

class UBillboard : public UPrimitiveComponent
{
	UCLASS(UBillboard, UPrimitiveComponent);
	using Super = UPrimitiveComponent;
	
public:
	UBillboard();
	virtual ~UBillboard() = default;

	virtual void BeginPlay() override;
	virtual void Render(class URenderer* Renderer) override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason);

	void SetTexture(class ID3D11ShaderResourceView* InTexture, float Cols = 1.0f, float Rows = 1.0f);
	void SetRenderUV(float Col, float Row);

protected:
	ID3D11ShaderResourceView* Texture = nullptr;
	
	float TotalCols = 1.0f;
	float TotalRows = 1.0f;
	float RenderCol = 1.0f;
	float RenderRow = 1.0f;

	FVector4 PartyMode = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	float PartyHue = 0.0f;
	
protected:
	virtual	FVector4 PartyHsvToRgb(float Hue);
};

