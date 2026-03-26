#pragma once
#include "Client_Define.h"
#include "Base.h"



NS_BEGIN(Engine)
class CGameInstance;
class CUI;
NS_END

NS_BEGIN(Client)

class CUI_Controller :
    public CBase
{
    DECLARE_SINGLETON(CUI_Controller)
public:

    CUI_Controller();

public:
    virtual ~CUI_Controller();

public:
    HRESULT Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Update(float TimeDelta);
    HRESULT Ready_UI();

    shared_ptr<CUI> Get_LoadingUI() { return m_Loading; };

private:
    shared_ptr<CUI> m_Loading = {nullptr};

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };
public:
    static shared_ptr<CUI_Controller> Create();
    virtual void Free() override;

};

NS_END