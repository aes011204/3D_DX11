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
    HRESULT Initialize(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void StateUI();
    void Update(float timeDelta);
    HRESULT Ready_UI();

    shared_ptr<CUI> Get_LoadingUI() { return m_loading; };

   // void Set_InvenCtrl(shared_ptr<class CInventory_Controller> invenCtrl);

    void ActiveTime(_uint time, _bool isSleep);
    void InActiveTime();

    void ActiveHover();
    void InActiveHover();
    void CloseToolTip();


    void End_StateUI();
    void CacheToolTip(const Evt_ItemHovered& e);
private:
    struct ToolTipRestoreDesc
    {
        _bool isValid = false;
        _bool isHold = false;
        _bool isPlayer = false;
        ItemInst itemInst = {};
    };
private:
    void RestoreToolTip();


    ToolTipRestoreDesc m_toolTipRestore = {};
private:
    shared_ptr<CUI> m_loading = {nullptr};
    shared_ptr < class CUI_Time > m_time = { nullptr };

    shared_ptr<class CUI_MiniGame> m_miniGame = { nullptr };

    Client::EPlayerState m_pendingUIState;
private:
    ComPtr<ID3D11Device> m_device = { nullptr };
    ComPtr<ID3D11DeviceContext> m_context = { nullptr };
    weak_ptr<class CGameInstance> m_gameInstance = { };
public:
    static shared_ptr<CUI_Controller> Create();
    virtual void Free() override;

};

NS_END
