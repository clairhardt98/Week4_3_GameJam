#pragma once

#include "Define.h"
#include "Components/StaticMeshComponent.h"

//
//struct BVHNode
//{
//    BVHNode() :
//        MinBound(FLT_MAX, FLT_MAX, FLT_MAX),
//        MaxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX),
//        Left(nullptr), Right(nullptr)
//    {}
//    bool IsLeaf() const { return Left == nullptr && Right == nullptr; }
//
//    // 현재 Renderer class 내부에서 
//    BVHNode* BuildBVHTopDown(std::vector<StaticMeshComp*>& meshes, int depth = 0)
//    {
//        BVHNode* node = new BVHNode();
//
//        // 현재 노드의 AABB 계산
//        for (StaticMeshComp* mesh : meshes) {
//            // 이거 로컬로 되어 있을텐데 지금은? bvh할때는 월드로 바꿀 필요가 없나?
//            node->MinBound = FVector::Min(
//                FVector(node->MinBound.x, node->MinBound.y, node->MinBound.z), 
//                FVector(mesh->AABB.min.x, mesh->AABB.min.y, mesh->AABB.min.z)
//            );
//            node->MaxBound = FVector::Max(
//                FVector(node->MaxBound.x, node->MaxBound.y, node->MaxBound.z),
//                FVector(mesh->AABB.max.x, mesh->AABB.max.y, mesh->AABB.max.z)
//            );
//        }
//
//        // 리프 노드 조건 검사 (임계값 4)
//        if (meshes.size() <= 4) {
//            node->Meshes = meshes;
//            return node;
//        }
//
//        // 근데 이거 Loadfloat에서 값을 가져오는게 아니라 참조로 해결하고 있어서 simd를 하고 있는거 같은데, 이거 나중에 수정하고
//        // 로직만 맞는지 판단할 것.
//
//        // 가장 긴 축을 기준으로 정렬 후 나누기
//        //FVector extent = XMVectorSubtract(XMLoadFloat3(&node->MaxBound), XMLoadFloat3(&node->MinBound));
//        FVector extent = node->MaxBound - node->MinBound;
//        int axis = 0; // 0: x, 1: y, 2: z
//        if (extent.y > extent.x) axis = 1;
//        if (extent.z > FVector::GetByIndex(extent, axis)) axis = 2;
//
//        // 메쉬를 선택된 축(axis)을 기준으로 정렬
//        std::sort(meshes.begin(), meshes.end(), [axis](StaticMeshComp* a, StaticMeshComp* b) {
//            float centerA = (a->AABB.min.m128_f32[axis] + a->AABB.max.m128_f32[axis]) * 0.5f;
//            float centerB = (b->AABB.min.m128_f32[axis] + b->AABB.max.m128_f32[axis]) * 0.5f;
//            return centerA < centerB;
//            });
//
//        // 메쉬를 두 그룹으로 나누기
//        size_t half = meshes.size() / 2;
//        std::vector<StaticMeshComp*> left(meshes.begin(), meshes.begin() + half);
//        std::vector<StaticMeshComp*> right(meshes.begin() + half, meshes.end());
//
//        // 자식 노드 재귀 생성
//        node->Left = BuildBVHTopDown(left, depth + 1);
//        node->Right = BuildBVHTopDown(right, depth + 1);
//
//        return node;
//    }
//
//
//    FVector MinBound;
//    FVector MaxBound;
//
//    BVHNode* Left;
//    BVHNode* Right;
//
//    std::vector<StaticMeshComp*> Meshes;
//};

