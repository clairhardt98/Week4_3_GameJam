#include "Engine/Source/Runtime/Engine/World.h"

#include "Actors/Player.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/FLoaderOBJ.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SkySphereComponent.h"
#include "Editor/UnrealEd/SceneMgr.h"

#include "EngineLoop.h"
#include "Editor/UnrealEd/EditorViewportClient.h"


void UWorld::LoadDefaultScene()
{
    // 여기서 json파싱
    FString jsonStr = FSceneMgr::LoadSceneFromFile("Assets/Scene/Default.scene");
    //FString jsonStr = FSceneMgr::LoadSceneFromFile("Assets/Scene/Default_0.scene");
    //FString jsonStr = FSceneMgr::LoadSceneFromFile("Assets/Scene/Default_10000.scene");
    //FString jsonStr = FSceneMgr::LoadSceneFromFile("Assets/Scene/Default_2.scene");
    //FString jsonStr = FSceneMgr::LoadSceneFromFile("Assets/Scene/Default_10.scene");
    SceneData sceneData = FSceneMgr::ParseSceneData(jsonStr);

    CreateBaseObject(sceneData);
}

void UWorld::Initialize()
{
    LoadDefaultScene();
    FEngineLoop::renderer.BuildMergedMeshBuffers(FSceneMgr::GetStaticMeshBVH(), 32);
    FEngineLoop::renderer.UpdateFrustumCull();
    //FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");
    // 이렇게 하면 안됄 것 같은데
    //FEngineLoop::renderer.BuildMergedMeshBuffers(this, GetEngine().GetLevelEditor()->GetActiveViewportClient());
    //FEngineLoop::renderer.BuildMergedMeshBuffers(this);

    //FManagerOBJ::CreateStaticMesh("Assets/SkySphere.obj");
    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->SubscribeCameraMoveEvent([&]() {
       // UE_LOG(LogLevel::Display, "Camera Move");
        });
}

void UWorld::CreateBaseObject(const SceneData& InSceneData)
{
    if (EditorPlayer == nullptr)
    {
        EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>();;
    }

    if (camera == nullptr)
    {
        // cameras에서 찾는다
        if (InSceneData.Camera != nullptr)
        {
            camera = static_cast<UCameraComponent*>(InSceneData.Camera);
        }
        // 그래도 없으면 생성
        else 
        {
            camera = FObjectFactory::ConstructObject<UCameraComponent>();
            camera->SetLocation(FVector(8.0f, 8.0f, 8.f));
            camera->SetRotation(FVector(0.0f, 45.0f, -135.0f));
        }
    }

    if (LocalGizmo == nullptr)
    {
        LocalGizmo = FObjectFactory::ConstructObject<UTransformGizmo>();
    }
}

void UWorld::ReleaseBaseObject()
{
    if (LocalGizmo)
    {
        delete LocalGizmo;
        LocalGizmo = nullptr;
    }

    if (worldGizmo)
    {
        delete worldGizmo;
        worldGizmo = nullptr;
    }

    if (camera)
    {
        delete camera;
        camera = nullptr;
    }

    if (EditorPlayer)
    {
        delete EditorPlayer;
        EditorPlayer = nullptr;
    }

}

void UWorld::Tick(float DeltaTime)
{
	camera->TickComponent(DeltaTime);
	EditorPlayer->Tick(DeltaTime);
	LocalGizmo->Tick(DeltaTime);

    // SpawnActor()에 의해 Actor가 생성된 경우, 여기서 BeginPlay 호출
    for (AActor* Actor : PendingBeginPlayActors)
    {
        Actor->BeginPlay();
    }
    PendingBeginPlayActors.Empty();

    // 매 틱마다 Actor->Tick(...) 호출
	for (AActor* Actor : ActorsArray)
	{
	    Actor->Tick(DeltaTime);
	}
}

void UWorld::Release()
{
	for (AActor* Actor : ActorsArray)
	{
		Actor->EndPlay(EEndPlayReason::WorldTransition);
        TSet<UActorComponent*> Components = Actor->GetComponents();
	    for (UActorComponent* Component : Components)
	    {
	        GUObjectArray.MarkRemoveObject(Component);
	    }
	    GUObjectArray.MarkRemoveObject(Actor);
	}
    ActorsArray.Empty();

	pickingGizmo = nullptr;
	ReleaseBaseObject();

    GUObjectArray.ProcessPendingDestroyObjects();
}

bool UWorld::DestroyActor(AActor* ThisActor)
{
    if (ThisActor->GetWorld() == nullptr)
    {
        return false;
    }

    if (ThisActor->IsActorBeingDestroyed())
    {
        return true;
    }

    // 액터의 Destroyed 호출
    ThisActor->Destroyed();

    if (ThisActor->GetOwner())
    {
        ThisActor->SetOwner(nullptr);
    }

    TSet<UActorComponent*> Components = ThisActor->GetComponents();
    for (UActorComponent* Component : Components)
    {
        Component->DestroyComponent();
    }

    // World에서 제거
    ActorsArray.Remove(ThisActor);

    // 제거 대기열에 추가
    GUObjectArray.MarkRemoveObject(ThisActor);
    return true;
}

void UWorld::SetPickingGizmo(UObject* Object)
{
	pickingGizmo = Cast<USceneComponent>(Object);
}