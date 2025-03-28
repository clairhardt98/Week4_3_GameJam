#pragma once
#include "StaticMeshComponent.h"

class UCubeComp : public StaticMeshComp
{
    DECLARE_CLASS(UCubeComp, StaticMeshComp)

public:
    UCubeComp();
    virtual ~UCubeComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};
