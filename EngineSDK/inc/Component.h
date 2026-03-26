#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CTransform;
class CUITransform;
class CUI;
class CGameObject;
class CEntity;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
    CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CComponent(const CComponent& rhs);
public:
    virtual ~CComponent() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

    HRESULT Set_Owner(shared_ptr<CEntity> pOwner);
    shared_ptr<CUI> Get_UOwner() { return m_pUOwner.lock(); }
    shared_ptr<CGameObject> Get_GOwner() { return m_pGOwner.lock(); }

    virtual void Save_ToJson(nlohmann::json& j) {};

protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = {};

protected:
    weak_ptr<CGameObject> m_pGOwner = {};
    CTransform* m_pGOwnerTransform = {nullptr};

    weak_ptr<CUI> m_pUOwner = {};
    CUITransform* m_pUOwnerUITransform = {nullptr};
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CComponent> Clone(void* pArg) = 0;

	void Free() override;
};

NS_END

