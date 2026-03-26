#include "UIText.h"

CUIText::CUIText(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUI(pDevice, pContext)
{
}

CUIText::CUIText(const CUIText& prototype)
	:CUI(prototype)
{
}

HRESULT CUIText::OnInit(void* pArg)
{
	auto* pDesc = static_cast<TEXT_DESC*>(pArg);
	m_vTextColor = pDesc->TextColor;
	m_strFontTag = pDesc->strFontTag;
	m_strText = pDesc->strText;
	m_FontAline = pDesc->fontaline;

	return CUI::OnInit(pDesc);
}

void CUIText::OnActive()
{
	CUI::OnActive();
}

void CUIText::OnInActive()
{
	CUI::OnInActive();
}

void CUIText::OnDisabled()
{
	CUI::OnDisabled();
}

void CUIText::OnUpdate(const _float& timeDelta)
{
	CUI::OnUpdate(timeDelta);
}

void CUIText::OnLateUpdate()
{
	CUI::OnLateUpdate();
}

HRESULT CUIText::OnRender()
{
	HRESULT hr = CUI::OnRender();
	if (m_strText.empty() == false)
	{
		Rect rc = m_pUITransformCom->Get_WorldRect();
		float CenterX = rc.Left() + (rc.Right() - rc.Left()) * 0.5f;
		float CenterY = rc.Top() + (rc.Bottom() - rc.Top()) * 0.5f;

		Rect canvas = m_pGameInstance.lock()->Get_WinSize();
		float screenHeight = canvas.Bottom();

		_float2 TextSize = m_pGameInstance.lock()->Measure_String(m_strFontTag, m_strText.c_str());
		_float2 DrawPos;


		

		switch (m_FontAline)
		{
		case FONTALINE::CENTER:
			DrawPos.x = CenterX - (TextSize.x * 0.5f);
			DrawPos.y = screenHeight - CenterY - (TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;

		case FONTALINE::RIGHT:
			DrawPos.x = rc.Right() - TextSize.x;
			DrawPos.y = screenHeight - CenterY - (TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;
		case FONTALINE::LEFT:
			DrawPos.x = rc.Right();
			DrawPos.y = screenHeight - CenterY - (TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;
		case FONTALINE::DEFAULT:
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), _float2(rc.Left(), rc.Top()), XMLoadFloat4(&m_vTextColor));
			break;

		
		}

	}

	return hr;
}

void CUIText::OnClear()
{
	CUI::OnClear();
}

void CUIText::Save_ToJson(nlohmann::json& j)
{
	CUI::Save_ToJson(j);
}

void CUIText::Load_FromJson(nlohmann::json& j)
{
	CUI::Load_FromJson(j);
}

void CUIText::OnGui()
{
	CUI::OnGui();
}


shared_ptr<CUIText> CUIText::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUIText> pInstance(new CUIText(pDevice, pContext), [](CUIText* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIText");
	}
	return pInstance;

}

void CUIText::Free()
{
	CUI::Free();
}
