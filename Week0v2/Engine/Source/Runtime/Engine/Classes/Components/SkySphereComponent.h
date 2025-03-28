#pragma once
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture.h"


class USkySphereComponent : public StaticMeshComp
{
    DECLARE_CLASS(USkySphereComponent, StaticMeshComp)

public:
    USkySphereComponent();
    virtual ~USkySphereComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    float UOffset = 0;
    float VOffset = 0;

};
