#pragma once
#include "Client_Define.h"
#include "Camera.h"


NS_BEGIN(Client)
class CCamera_Play final:
    public CCamera
{

public:
    struct CAMERAPLAY_DESC : public CCamera::CAMERA_DESC
    {
        _float	fMouseSensor = {};
    	weak_ptr<CGameObject> target = { };

    };
protected:
    CCamera_Play(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCamera_Play(const CCamera& prototype);
public:
    virtual ~CCamera_Play() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    void SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance,
                   _float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();


private:
    void Start_Targetting(_float4 startPos, _float degree, _float distance);

private:
    _float	m_fMouseSensor = {};
    weak_ptr<CGameObject> m_pTarget = {  };
    weak_ptr<CTransform> m_pTargetTransform = {  };


    _float m_MinPitch = 5.f;
    _float m_MaxPitch = 90.f;

    _float m_MinDistance = 5.f;
    _float m_MaxDistance = 90.f;

    _float m_Yaw = {};
    _float m_Pitch = {};
    _float m_fDistance = {};
    _float m_time = {};
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    static shared_ptr<CCamera_Play> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;


};

NS_END