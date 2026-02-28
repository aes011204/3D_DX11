#pragma once
#include "UI.h"
// 패널의 역할 : 패널의 백 이미지, 레이아웃, 
NS_BEGIN(Engine)

class ENGINE_DLL CUIPanel :
    public CUI
{
public:
    struct UIPANEL_DESC : public CUI::UI_DESC
    {
        //_bool IsTrnasparent = {};
        //_bool IsFullScreen = {};
        //_bool IsUseLayout = {};

        LAYOUT_DESC LayoutDesc;
    };
protected:
    CUIPanel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUIPanel(const CUIPanel& prototype);
public:
    ~CUIPanel()= default;

public:
    //HRESULT Ready_GameObject() override;
    //_int    Update_GameObject(const _float& timeDelta) override;
    //void    LateUpdate_GameObject(const _float& timeDelta) override;
    //void    Render_GameObject() override;

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;


public:
    void Layout();
    void AddChild();
private:
    // 내부 계산 용
    int index = 0;

    //// 나중에 구조체
    //int m_Raw = {0};
    //int m_Col = { 0 };
    //float m_SlotSize = { 0 };
    //_float2 m_Spacing = { };
    //_float2 m_Padding = { };
    ////
    LAYOUT_DESC m_LayoutDesc ;


    _bool m_IsTrnasparent = {};
    _bool m_IsFullScreen = {};
    _bool m_IsUseLayout = {};

public:
      //static shared_ptr<CUIPanel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    void Free() override;
};

NS_END


