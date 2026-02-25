#pragma once
#include "Transform.h" // 이 안에 베이스 포함되있음
#include "Entity.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CEntity
{
public:
    struct GAMEOBJECT_DESC : public CTransform::TRANSFOM_DESC
    {
        _uint iFlag = {};

        //_float fX = {};
        //_float fY = {};
        //_float fSizeX = {};
        //_float fSizeY = {};

    };
protected:
    CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CGameObject(const CGameObject& prototype);
public:
    virtual ~CGameObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    //void Set_ProtoTag(_wstring str) { m_strProtoTag = str; }
    //void Set_ProtoLevel(_uint Level) { m_strProtoLevel = Level; }
    //_wstring Get_ProtoTag() { return m_strProtoTag; }
    //_uint Get_ProtoLevel() { return m_strProtoLevel; }

   // HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, D3DTS eTransformState);

    //void Update_Transform();

protected:
    //_wstring m_strProtoTag = {};
    //_uint m_strProtoLevel = {};
    class shared_ptr<CTransform> m_pTransformCom = { nullptr };
    //_float						m_fViewportWidth{}, m_fViewportHeight{};
    //_float						m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
    //_float4x4					m_TransformationMatrices[ETOI(D3DTS::END)];

protected:
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* pArg) = 0 ;
    void Free() override;
};



NS_END
