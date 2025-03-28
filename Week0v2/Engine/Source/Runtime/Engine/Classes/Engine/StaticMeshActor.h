#pragma once
#include "GameFramework/Actor.h"


class AStaticMeshActor : public AActor
{
    DECLARE_CLASS(AStaticMeshActor, AActor)

public:
    AStaticMeshActor();

    StaticMeshComp* GetStaticMeshComponent() const { return StaticMeshComponent; }

private:
    StaticMeshComp* StaticMeshComponent = nullptr;
};
