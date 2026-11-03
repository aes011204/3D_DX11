#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIText;
	class CUIImage;
}

NS_BEGIN(Client)

	class CUI_Time :
    public CUIPanel

{
public:
    struct UiTimeDesc : public CUIPanel::UIPanelDesc
    {
    };
protected:
    CUI_Time(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Time(const CUIPanel& prototype);
public:
    ~CUI_Time() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    void UIPannelActive(_uint time, _bool IsSleep);
public:

private:

    _bool m_isSleep = {};
  
    shared_ptr<CUIImage>  m_icon = {};
    shared_ptr<CUIText> m_text = {};
    shared_ptr<CUIImage> m_circle = {};
    shared_ptr<CShader >m_circleShader = {};

    Zone m_zones[8];
    float m_zoneCount = 1;
    float  m_targetAngle = {};
    
        float   m_acc = {};
        _bool  result = false;
    float   m_seed = {};

    //
    float m_currentTod = {};
    float m_startTod = {};
    float m_targetTod = {};
public:
    static shared_ptr<CUI_Time> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END