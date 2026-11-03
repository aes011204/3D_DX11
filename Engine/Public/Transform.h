#pragma once
#include "Component.h"

/* 모델 정점들을 객체 기준으로 월드로 변환시켜주기위한 월드변환행렬을 보관하낟.  */
/* 이 행렬의 상태변환(월드에서의 상태변환)에 해당하는 인터페이스(기능)을 담당한다. */

// 컴포넌트중에 트렌스 폼만 특이하게 클론을 안쓸거임 , 그리고 모든 오드젝트가 무조건 적으로 가지고 있게 할것임

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final:
    public CComponent
{
public:
    struct TransformDesc
    {
        //_float4x4				WorldMatrix = {};

        _float3 position = { 0.f, 0.f, 0.f };
        _float3 rotationDegree = { 0.f, 0.f, 0.f };
        _float3 scale = { 1.f, 1.f, 1.f };

        _float					speedPerSec = {};
        _float					degreePerSec = {};
    };
private:
    CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTransform(const CTransform& prototype);
public:
    virtual ~CTransform() = default;

public:
    _vector Get_State(ETransformState eState)
    {
        //return XMLoadFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOI(eState)][0]));
        switch(ETOI(eState))
        {
        case ETOI(ETransformState::POSITION):
            return XMLoadFloat3(&m_position);
        case ETOI(ETransformState::LOOK):
            return XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMLoadFloat4(&m_rotationQuat));
        case ETOI(ETransformState::RIGHT):
            return XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMLoadFloat4(&m_rotationQuat));
        case ETOI(ETransformState::UP):
            return XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&m_rotationQuat));

            //// 핵심 데이터 접근 (Get/Set)
            //_vector Get_Position() { return XMLoadFloat3(&m_vPosition); }
            //void Set_Position(_fvector vPos) { XMStoreFloat3(&m_vPosition, vPos); m_bIsDirty = true; }

            //_vector Get_Quaternion() { return XMLoadFloat4(&m_vRotationQuat); }
            //void Set_Quaternion(_fvector vQuat) { XMStoreFloat4(&m_vRotationQuat, vQuat); m_bIsDirty = true; }

            //_float3 Get_Scale() { return m_vScale; }
            //void Set_Scale(_float3 vScale) { m_vScale = vScale; m_bIsDirty = true; }
        }
    }

    _float3 Get_Scaled()
    {
        /*return _float3(
            XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
            XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
            XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
        );*/

        return m_scale;
    }

    const _float4x4* Get_WorldMatrix()  {
        if (m_isDirty)
            Update_WorldMatrix();

        return &m_worldMatrix;
    }

    //void Set_State(STATE eState, _fvector vState)//
    //{
    //    XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOI(eState)][0]), vState);

    //    //if (eState == STATE::POSITION)
    //    //{
    //    //    m_WorldMatrix._44 = 1.f;
    //    //}

    //    m_bIsDirty = true;
    //}
    _vector Get_Position() { return XMLoadFloat3(&m_position); }
    void Set_Position(_fvector vPos) { XMStoreFloat3(&m_position, vPos); m_isDirty = true; }

    _vector Get_Quaternion() { return XMLoadFloat4(&m_rotationQuat);}
    void Set_Quaternion(_fvector vQuat)
	{
    	XMStoreFloat4(&m_rotationQuat, vQuat);

        _float4 q = {};
        XMStoreFloat4(&q, vQuat);
        //m_vRotationDegree = QuaternionToEuler(q);
    	m_isDirty = true;
	}
    _float3 Get_RotationDegree() { return QuaternionToEuler(m_rotationQuat); };
    void Set_RotationDegree(_float3 vRotation)
    {
        m_rotationDegree = vRotation;

        _vector vRadian = XMVectorSet(
            XMConvertToRadians(m_rotationDegree.x),
            XMConvertToRadians(m_rotationDegree.y),
            XMConvertToRadians(m_rotationDegree.z), 0.f);

        XMStoreFloat4(&m_rotationQuat, XMQuaternionRotationRollPitchYawFromVector(vRadian));
        m_isDirty = true;
    }
    
    _float3 Get_Scale() { return m_scale; }
    void Set_Scale(_float3 vScale) { m_scale = vScale; m_isDirty = true; }

    void Set_ParentMatrix(const _float4x4* pParentMatrix) { m_parentMatrix = pParentMatrix; m_isDirty = true; }

	const _float4x4* Get_WorldMatrixPtr() const { return &m_worldMatrix;}

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader>pShaderCom, const _char* pConstantName );

    void Update_WorldMatrix();
public:
    void SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);//기존의 있는 크기에 배수가 아니라 스케일정보 바꿔줌
    void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);//저장된 크기의 배수로 키우기
    void Go_Forward_Distanace(_float fDistance);
    void Go_Backward_Distanace(_float fDistance);

	void Go_Forward(_float fTimeDelta);
    void Go_Backward(_float fTimeDelta);

    void Go_Right(_float fTimeDelta);
    void Go_Left(_float fTimeDelta);

    void Go_Up(_float fTimeDelta);
    void Go_Down(_float fTimeDelta);

    void Start_Lerp(_fvector vTargetPos, _float3 vTargetRotation, _float fDuration, _bool IsTarget= false);

    void Lerp_To(_float fTimeDelta);

   // void Rotation(_fvector vAxis, _float fDegree);//속도X 정해논 각도로 따라 항등상태에서 회전 하는거임
    void Turn(_fvector vAxis, _float fTimeDelta);//나한테 저장된 회전 속도 만큼 서서히 회전

    void LookAt(_fvector vAt);

    void Orbit(_fvector vTargetPos, _float3 vTargetQuat, _float fDistance, _float fPitch, _float fYaw);

    void OnGui() override;
    virtual void Save_ToJson(nlohmann::json& j) override;
    virtual void Load_FromJson(nlohmann::json& j) override;

    _float Get_Speed() const { return m_speedPerSec; }
    void Set_Speed(_float fSpeed) { m_speedPerSec = fSpeed; }

    _float Get_RadianSpeed() const { return m_radianPerSec; }
    void Set_RadianSpeed(_float fRadian) { m_radianPerSec = fRadian; }


    _float3 QuaternionToEuler(_float4 q);
private:
    _float4x4 m_worldMatrix = {}; //저장소
    _bool m_isDirty = true;
    _float					m_speedPerSec = {};
    _float					m_radianPerSec = {};

    _float3 m_position = { 0.f, 0.f, 0.f };
    _float3 m_rotationDegree = { 0.f, 0.f, 0.f };
    _float4 m_rotationQuat = { 0.f, 0.f, 0.f , 1.f};
    _float3 m_scale = { 1.f, 1.f, 1.f };

    const _float4x4* m_parentMatrix = {};

    //_float3 m_vScale = {};

public:
    _bool GetIsLerp() { return m_isLerping; }
private:
    _float m_lerpAcc = 0.f;
    _float m_lerpTime = 0.f;
    _vector m_startPos = {};
    _vector m_startQuat = {};
    _vector m_targetPos = {};
    _vector m_targetQuat = {};
    _bool m_isLerping = false;
    _bool m_isTarget = false;

public:
    static shared_ptr<CTransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CComponent> Clone(void* pArg)override;
    void Free();
};

NS_END