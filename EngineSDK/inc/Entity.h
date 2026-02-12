#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class CComponent;

class CEntity abstract:
    public CBase
{
protected:
    CEntity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CEntity(const CEntity& prototype);
public:
    virtual ~CEntity() = default;

public:
    //virtual HRESULT Initialize_Prototype() {};
    //virtual HRESULT Initialize(void* pArg) {};
    //virtual void Priority_Update(_float fTimeDelta) {};
    //virtual void Update(_float fTimeDelta) {};
    //virtual void Late_Update(_float fTimeDelta) {};
    virtual HRESULT Render() { return S_OK; }

    shared_ptr<CComponent> Get_Component(const _wstring& strComponentTag);

/// <IMGUI>
    map<const _wstring, class shared_ptr<CComponent> >& Get_ComponentMap() {return m_Components;};
/// </summary>
protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };

protected:
    // 원형객체를 복제하여 맵에도 추가하고 + 자식객체의 멤버변수에도 셋팅해준다
    template <typename T>
    HRESULT Add_Component(_uint iProtoLevelIndex, const _wstring& strProtoTag,
        const _wstring& strComponentTag, T** ppOut, void* pArg = nullptr);
protected:
    map<const _wstring, class shared_ptr<CComponent> > m_Components;

public:
    virtual void Free()override;
};

NS_END;