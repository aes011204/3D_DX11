#pragma once
#include "Client_Define.h"
#include "Camera.h"


NS_BEGIN(Client)
class CCamera_Play final:
    public CCamera
{

public:


    struct CameraPlayDesc : public CCamera::CameraDesc
    {
        _float	mouseSensor = {};
    	weak_ptr<CGameObject> target = { };
        _float3 dir = {};

    };
protected:
    CCamera_Play(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CCamera_Play(const CCamera& prototype);
public:
    virtual ~CCamera_Play() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual void Priority_Update(_float timeDelta);
    void Update_AfterTarget(_float timeDelta) override;
    void Change_CamMode(ECamMode m_changeMode, ECamMode m_nextMode);
    void SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance,
                   _float timeDelta);
    virtual void Update(_float timeDelta);
    virtual void Late_Update(_float timeDelta);
    virtual HRESULT Render();

    void Update_Follow(_float timeDelta, shared_ptr<CamDesc> desc);
    bool Update_Lerp(_float timeDelta, shared_ptr<CamDesc> desc);
    void Get_Target_PosLook(_float3& camPos, _float3& targetRot);
    _float3 Shake_Cam(_float timeDelta);
private:
    void Start_Targetting(_float4 startPos, _float degree, _float distance);

private:
    //shared_ptr<CAM_DESC> m_pCurrentDesc = nullptr; // 데이터 안에 모드 정보가 있음
    //shared_ptr<CAM_DESC> m_pNextDesc = nullptr; // 다음 예약석

    _float3 m_basePos; // 해당 프레임에 처음 위치

    _bool m_isShack = false;
    float m_shakeTime = {};
    float    m_shakePower = {};
    float    m_shakeDecay = {};

    //

    float  m_startFOVY = 0.f;
    float m_acc = 0.f;

    deque<shared_ptr<CamDesc>> m_camCommands;

    _bool m_isFinish = false;
    _bool m_isChange = false;

private:
    _float	m_mouseSensor = {};
    weak_ptr<CGameObject> m_target = {  };
    weak_ptr<CTransform> m_targetTransform = {  };


    _float m_minPitch = 5.f;
    _float m_maxPitch = 90.f;

    _float m_minDistance = 5.f;
    _float m_maxDistance = 90.f;

    _float m_yaw = {};
    _float m_pitch = {};
    _float m_distance = {};
    _float m_followTargetY = 0.f;
    _float m_time = {};
    //
    //_vector m_vTargetPos={};
    //_float3 m_vTargetQut={};
    //_float  m_fDuration={};
    _bool m_isFirstFlag = {false};
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    static shared_ptr<CCamera_Play> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    virtual shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;


   

};

NS_END