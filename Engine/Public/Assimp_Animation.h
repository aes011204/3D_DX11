#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CAssimp_Animation :
    public CBase
{
private:
    CAssimp_Animation();
public:
    virtual ~CAssimp_Animation() = default;

public:
    HRESULT Initialize(const aiAnimation* pAIAnimation, shared_ptr<class CAssimp_Model> pModel);

    _bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class CAssimp_Bone>>& Bones, _bool isLoop);
    //_bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class CAssimp_Bone>>& Bones, _bool isLoop)

public:
 
    _float m_fDuration = {};
    _float m_fTickPerSecond = {};

    _float m_fCurrentTrackPosition = {};

    _uint m_iNumChannels = {};
    vector<shared_ptr<class CAssimp_Channel>> m_Channels;

public:
    static shared_ptr<CAssimp_Animation> Create(const aiAnimation* pAIAnimation, shared_ptr<class CAssimp_Model> pModel);
    void Free() override;


};

NS_END