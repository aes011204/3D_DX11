#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CBone :
    public CBase
{
private:
    CBone();
public:
    virtual ~CBone() = default;

public:
    HRESULT Initialize(const Cvt_Bone& BoneDesc);

    void Update_CombinedTransformMatrix(const vector<shared_ptr<CBone>> Bones, _fmatrix PreTransformMatrix);
    void Update_TransformationMatrix(_fmatrix TransformMatrix);
public:
    _bool isCompare(const _char* pBoneName) {return !strcmp(pBoneName, m_szName);}
    const _float4x4* Get_m_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
private:

    _char m_szName[MAX_PATH] = {};
    _float4x4 m_TransformationMatrix = {};
    _float4x4 m_CombinedTransformationMatrix = {};
    _int m_iParentBoneIndex = { -1 }; // -1 ¿Ã∏È  root bone ¿”

public:
   
    static shared_ptr<CBone> Create(const Cvt_Bone& BoneDesc);
	shared_ptr<CBone> Clone();
    void Free() override;


};

NS_END