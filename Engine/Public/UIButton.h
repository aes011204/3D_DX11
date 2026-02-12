#pragma once
#include "UI.h"


NS_BEGIN(Engine)

class ENGINE_DLL CUIButton :
    public CUI
{
protected:
    CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIButton(const CUIButton& prototype);
public:
    virtual ~CUIButton() = default;

protected:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    void OnInit()override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    void OnRender()override;
    void OnClear()override;

    void ProcessInput();
    void ChangeState(BUTTON_STATE next);

private:
    bool  m_ClickInside = { false };

    BUTTON_STATE m_UIState = BUTTON_STATE::NONE;

    function<void()> m_ClickEvent= {nullptr};

    //vector<IUIButtonBehavior*> m_behavior; // 인터페이스 클래스

public:
    static shared_ptr<CUIButton> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	//virtual shared_ptr<CUI> Clone(void* pArg);
    void Free()override;

};

NS_END