#pragma once
#include "StaticMeshComponent.h"


class USphereComp : public StaticMeshComp
{
    DECLARE_CLASS(USphereComp, StaticMeshComp)

public:
    USphereComp();
    virtual ~USphereComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};
