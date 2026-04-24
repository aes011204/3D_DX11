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
    struct UITIME_DESC : public CUIPanel::UIPANEL_DESC
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

    _bool m_IsSleep = {};
  
    shared_ptr<CUIImage>  m_iCon = {};
    shared_ptr<CUIText> m_Text = {};
    shared_ptr<CUIImage> m_pCircle = {};
    shared_ptr<CShader >m_CircleShader = {};

    Zone m_zones[8];
    float m_zoneCount = 1;
    float  m_TargetAngle = {};
    
        float   m_Acc = {};
        _bool  result = false;
    float   m_Seeed = {};

    //
    float m_CurrentTod = {};
    float m_StartTod = {};
    float m_TargetTod = {};
public:
    static shared_ptr<CUI_Time> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END