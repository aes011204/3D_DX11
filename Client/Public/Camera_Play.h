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
    void Change_CamMode(CAM_MODE m_ChangeMode, CAM_MODE m_NextMode);
    void SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance,
                   _float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    void Update_Follow(_float fTimeDelta, shared_ptr<CAM_DESC> pDesc);
    bool Update_Lerp(_float fTimeDelta, shared_ptr<CAM_DESC> pDesc);
    void Get_Target_PosLook(_float3& camPos, _float3& vTargetRot);
    void Shake_Cam(_float fTimeDelta);
private:
    void Start_Targetting(_float4 startPos, _float degree, _float distance);

private:
    //shared_ptr<CAM_DESC> m_pCurrentDesc = nullptr; // 데이터 안에 모드 정보가 있음
    //shared_ptr<CAM_DESC> m_pNextDesc = nullptr; // 다음 예약석

    _float3 m_BasePos; // 해당 프레임에 처음 위치

    _float3 m_FinalPos = {};

    float m_ShakeTime = {};
    float    m_ShakePower = {};
    float    m_DeAc = {};

    //

    float  m_fStartFovy = 0;
    float m_Acc = 0.f;

    deque<shared_ptr<CAM_DESC>> m_CamCommands;

    _bool m_bFinish = false;
    _bool m_bChange = false;

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
    //
    //_vector m_vTargetPos={};
    //_float3 m_vTargetQut={};
    //_float  m_fDuration={};
    _bool m_FirstFlag = {false};
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    static shared_ptr<CCamera_Play> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;


   

};

NS_END