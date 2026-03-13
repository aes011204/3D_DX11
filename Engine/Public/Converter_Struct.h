#pragma once
#pragma once
#include <cstdint>
#include <windows.h>

struct Cvt_Header
{
    uint32_t iMagic{}; // 
    uint32_t bIsAnim = {};
    uint32_t iNumMeshes = {};
    uint32_t iNumMaterial = {};

};

struct Cvt_MeshInfo
{
    char szName[64];
    uint32_t iMaterialIndex;
    uint32_t iNumVertices;
    uint32_t iNumIndices;

};

struct Cvt_VTXMESH {
    float vPos[3] = {};
    float vUV[8][2] = {};
    float vNormal[3] = {};
    float vTangent[3] = {};

    //int32_t vIndices[4] = { -1, -1, -1, -1 }; // Bone Indices
    //float vWeights[4] = { 0.f, 0.f, 0.f, 0.f }; // Bone Weights
};

struct Cvt_Material
{
    char szName[64];
    char szDiffusePath[_MAX_PATH];
    char szNormalPath[_MAX_PATH];
    char szSpecularPath[_MAX_PATH];
    char szOpacityPath[_MAX_PATH];

    /* Color  ambient;
     Color  diffuse;
     Color  specular;
     Color  emissive;*/
};

//
struct Cvt_VTXANIMMESH : public Cvt_VTXMESH
{

    // --- 추가 데이터 ---
    uint32_t iBlendIndex[4];  // 4바이트 * 4 = 16바이트
    float    fBlendWeight[4]; // 4바이트 * 4 = 16바이트
};