#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CAssimp_Bone :
    public CBase
{
private:
    CAssimp_Bone();
public:
    virtual ~CAssimp_Bone() = default;

public:
    HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);

    void Update_CombinedTransformMatrix(const vector<shared_ptr<CAssimp_Bone>> Bones, _fmatrix PreTransformMatrix);
    void Update_TransformationMatrix(_fmatrix TransformMatrix);
public:
    _bool isCompare(const _char* pBoneName) {return !strcmp(pBoneName, m_name);}
    const _float4x4* Get_m_CombinedTransformationMatrixPtr() const { return &m_combinedTransformationMatrix; }
private:

    _char m_name[MAX_PATH] = {};
    _float4x4 m_transformationMatrix = {};
    _float4x4 m_combinedTransformationMatrix = {};
    _int m_parentBoneIndex = { -1 }; // -1 이면  root bone 임

public:
    static shared_ptr<CAssimp_Bone> Create(const aiNode* pAINode, _int iParentBoneIndex);
    void Free() override;


};

NS_END