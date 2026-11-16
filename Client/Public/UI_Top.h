#pragma once


#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIText;
    class CUIImage;
}

NS_BEGIN(Client)

	class CUI_Top final :
    public CUIPanel
{
public:
    struct TopDesc : public CUIPanel::UIPanelDesc
    {
    };
protected:
    CUI_Top(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUI_Top(const CUIPanel& prototype);
public:
    ~CUI_Top() = default;

public:

    HRESULT OnInit(void* arg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

public:

private:
    shared_ptr<CUIText> m_dayText = {};
    shared_ptr<CUIText> m_timeText = {};
    shared_ptr<CUIImage> m_dayCircle = {};

    _uint m_preDay = {};
    _float m_preMin = {};
public:
    static shared_ptr<CUI_Top> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Free() override;
};

NS_END