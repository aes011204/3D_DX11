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
    CComponent(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CComponent(const CComponent& rhs);
public:
    virtual ~CComponent() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual HRESULT Render() { return S_OK; };

    HRESULT Set_Owner(shared_ptr<CEntity> owner);
    shared_ptr<CUI> Get_UOwner() { return m_UOwner.lock(); }
    shared_ptr<CGameObject> Get_GOwner() { return m_GOwner.lock(); }

    virtual void Save_ToJson(nlohmann::json& j) {};

protected:
    ComPtr<ID3D11Device> m_device = { nullptr };
    ComPtr<ID3D11DeviceContext> m_context = { nullptr };
    weak_ptr<class CGameInstance> m_gameInstance = {};


    _bool m_isCloned = { false };
protected:
    weak_ptr<CGameObject> m_GOwner = {};
    CTransform* m_GOwnerTransform = {nullptr};

    weak_ptr<CUI> m_UOwner = {};
    CUITransform* m_UOwnerUITransform = {nullptr};
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CComponent> Clone(void* arg) = 0;

	void Free() override;
};

NS_END

