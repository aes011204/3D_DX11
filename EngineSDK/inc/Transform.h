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
    struct TRANSFOM_DESC
    {
        //_float4x4				WorldMatrix = {};

        _float3 vPosition = { 0.f, 0.f, 0.f };
        _float3 vRotationDegree = { 0.f, 0.f, 0.f };
        _float3 vScale = { 1.f, 1.f, 1.f };

        _float					fSpeedPerSec = {};
        _float					fDegreePerSec = {};
    };
private:
    CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTransform(const CTransform& prototype);
public:
    virtual ~CTransform() = default;

public:
    _vector Get_State(STATE eState)
    {
        //return XMLoadFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOI(eState)][0]));
        switch(ETOI(eState))
        {
        case ETOI(STATE::POSITION):
            return XMLoadFloat3(&m_vPosition);
        case ETOI(STATE::LOOK):
            return XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMLoadFloat4(&m_vRotationQuat));
        case ETOI(STATE::RIGHT):
            return XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMLoadFloat4(&m_vRotationQuat));
        case ETOI(STATE::UP):
            return XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat4(&m_vRotationQuat));

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

        return m_vScale;
    }

    const _float4x4* Get_WorldMatrix()  {
        if (m_bIsDirty)
            Update_WorldMatrix();

        return &m_WorldMatrix;
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
    _vector Get_Position() { return XMLoadFloat3(&m_vPosition); }
    void Set_Position(_fvector vPos) { XMStoreFloat3(&m_vPosition, vPos); m_bIsDirty = true; }

    _vector Get_Quaternion() { return XMLoadFloat4(&m_vRotationQuat); }
    void Set_Quaternion(_fvector vQuat)
	{
    	XMStoreFloat4(&m_vRotationQuat, vQuat);

        _float4 q = {};
        XMStoreFloat4(&q, vQuat);
        m_vRotationDegree = QuaternionToEuler(q);
    	m_bIsDirty = true;
	}
    _float3 Get_RotationDegree() { return m_vRotationDegree; };
    void Set_RotationDegree(_float3 vRotation)
    {
        m_vRotationDegree = vRotation;

        _vector vRadian = XMVectorSet(
            XMConvertToRadians(m_vRotationDegree.x),
            XMConvertToRadians(m_vRotationDegree.y),
            XMConvertToRadians(m_vRotationDegree.z), 0.f);

        XMStoreFloat4(&m_vRotationQuat, XMQuaternionRotationRollPitchYawFromVector(vRadian));
        m_bIsDirty = true;
    }
    
    _float3 Get_Scale() { return m_vScale; }
    void Set_Scale(_float3 vScale) { m_vScale = vScale; m_bIsDirty = true; }

    void Set_ParentMatrix(const _float4x4* pParentMatrix) { m_pParentMatrix = pParentMatrix; m_bIsDirty = true; }

	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix;}

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader>pShaderCom, const _char* pConstantName );

    void Update_WorldMatrix();
public:
    void SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);//기존의 있는 크기에 배수가 아니라 스케일정보 바꿔줌
    void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);//저장된 크기의 배수로 키우기

	void Go_Forward(_float fTimeDelta);
    void Go_Backward(_float fTimeDelta);
    void Go_Right(_float fTimeDelta);
    void Go_Left(_float fTimeDelta);

    void Go_Up(_float fTimeDelta);
    void Go_Down(_float fTimeDelta);

   // void Rotation(_fvector vAxis, _float fDegree);//속도X 정해논 각도로 따라 항등상태에서 회전 하는거임
    void Turn(_fvector vAxis, _float fTimeDelta);//나한테 저장된 회전 속도 만큼 서서히 회전

    void LookAt(_fvector vAt);

    void Orbit(_fvector vTargetPos, _float3 vTargetQuat, _float fDistance, _float fPitch, _float fYaw);

    void OnGui() override;
    virtual void Save_ToJson(nlohmann::json& j) override;
    virtual void Load_FromJson(nlohmann::json& j) override;


    _float3 QuaternionToEuler(_float4 q);
private:
    _float4x4 m_WorldMatrix = {}; //저장소
    _bool m_bIsDirty = true;
    _float					m_fSpeedPerSec = {};
    _float					m_fRadianPerSec = {};

    _float3 m_vPosition = { 0.f, 0.f, 0.f };
    _float3 m_vRotationDegree = { 0.f, 0.f, 0.f };
    _float4 m_vRotationQuat = { 0.f, 0.f, 0.f , 1.f};
    _float3 m_vScale = { 1.f, 1.f, 1.f };

    const _float4x4* m_pParentMatrix = {};

    //_float3 m_vScale = {};
private:

public:
    static shared_ptr<CTransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CComponent> Clone(void* pArg)override;
    void Free();
};

NS_END