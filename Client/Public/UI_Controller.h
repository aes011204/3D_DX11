#pragma once
#include "Client_Define.h"
#include "Client_Enum.h"
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
    void StateUI();
    void Update(float TimeDelta);
    HRESULT Ready_UI();

    shared_ptr<CUI> Get_LoadingUI() { return m_Loading; };

   // void Set_InvenCtrl(shared_ptr<class CInventory_Controller> invenCtrl);

    void ActiveTime(_uint time, _bool isSleep);
    void InActiveTime();

    void ActiveHover();
    void InActiveHover();
    void CloseToolTip();


    void End_StateUI();
    void CacheToolTip(const Evt_ItemHovered& e);
private:
    struct TOOLTIP_RESTORE_DESC
    {
        _bool bValid = false;
        _bool isHold = false;
        _bool IsPlayer = false;
        Item_Inst itemInst = {};
    };
private:
    void RestoreToolTip();


    TOOLTIP_RESTORE_DESC m_ToolTipRestore = {};
private:
    shared_ptr<CUI> m_Loading = {nullptr};
    shared_ptr < class CUI_Time > m_pTime = { nullptr };

    shared_ptr<class CUI_MiniGame> m_miniGame = { nullptr };

    Client::E_PLAYERSTATE m_PendingUIState;
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };
public:
    static shared_ptr<CUI_Controller> Create();
    virtual void Free() override;

};

NS_END
