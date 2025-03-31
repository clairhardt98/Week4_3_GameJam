#pragma once
#include "Define.h"
#include "Container/Map.h"

#include "BVH.h"

class UObject;
struct SceneData {
    int32 Version;
    int32 NextUUID;
    TMap<int32, UObject*> Primitives;
    UObject* Camera = nullptr;
};
class FSceneMgr
{
public:
    static SceneData ParseSceneData(const FString& jsonStr);
    static FString LoadSceneFromFile(const FString& filename);
    static std::string SerializeSceneData(const SceneData& sceneData);
    static bool SaveSceneToFile(const FString& filename, const SceneData& sceneData);

    static FBoundingVolume* BuildStaticMeshBVH(const SceneData& sceneData);
    static FBoundingVolume* GetStaticMeshBVH() { return staticMeshBVH; }

private:
    static FBoundingVolume* staticMeshBVH;
};

