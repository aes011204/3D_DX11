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

#define MAX_TEXTURE_SLOT 28


//struct Cvt_MaterialInfo
//{
//    char szName[MAX_PATH]; // 재질 이름
//    char szTexturePaths[MAX_TEXTURE_SLOT][MAX_PATH];
//};

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

enum Cvt_TexType {
    TextureType_NONE = 0,


    TextureType_DIFFUSE = 1,


    TextureType_SPECULAR = 2,

    TextureType_AMBIENT = 3,

    TextureType_EMISSIVE = 4,


    TextureType_HEIGHT = 5,


    TextureType_NORMALS = 6,

    TextureType_SHININESS = 7,

    TextureType_OPACITY = 8,


    TextureType_DISPLACEMENT = 9,

    TextureType_LIGHTMAP = 10,

    TextureType_REFLECTION = 11,



    TextureType_BASE_COLOR = 12,
    TextureType_NORMAL_CAMERA = 13,
    TextureType_EMISSION_COLOR = 14,
    TextureType_METALNESS = 15,
    TextureType_DIFFUSE_ROUGHNESS = 16,
    TextureType_AMBIENT_OCCLUSION = 17,


    TextureType_UNKNOWN = 18,


    TextureType_SHEEN = 19,

    TextureType_CLEARCOAT = 20,


    TextureType_TRANSMISSION = 21,


    TextureType_MAYA_BASE = 22,
    TextureType_MAYA_SPECULAR = 23,
    TextureType_MAYA_SPECULAR_COLOR = 24,
    TextureType_MAYA_SPECULAR_ROUGHNESS = 25,


    TextureType_ANISOTROPY = 26,


    TextureType_GLTF_METALLIC_ROUGHNESS = 27
};