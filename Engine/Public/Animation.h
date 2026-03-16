#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CAnimation :
    public CBase
{
private:
    CAnimation();
public:
    virtual ~CAnimation() = default;

public:
    HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);

    _bool Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop);

public:
 
    _float m_fDuration = {};
    _float m_fTickPerSecond = {};

    _float m_fCurrentTrackPosition = {};

    _uint m_iNumChannels = {};
    vector<shared_ptr<class CChannel>> m_Channels;

public:
    static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, class CModel* pModel);
    void Free() override;


};

NS_END