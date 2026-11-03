#include "UIText.h"

CUIText::CUIText(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CUI(device, context)
{
}

CUIText::CUIText(const CUIText& prototype)
	:CUI(prototype)
{
}

HRESULT CUIText::OnInit(void* arg)
{
	auto* desc = static_cast<TextDesc*>(arg);
	m_textColor = desc->textColor;
	m_fontTag = desc->fontTag;
	m_text = desc->text;
	m_fontAline = desc->fontAlign;



	m_isInteractable = false;

	return CUI::OnInit(desc);

}

void CUIText::OnActive()
{

		/*m_TextSize = m_pGameInstance.lock()->Measure_String(m_strFontTag, m_strText.c_str());*/
		GetUITransform()->SetSizeDelta(m_textSize);


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
	if(m_isChangeText==true)
	{
	m_textSize = m_gameInstance.lock()->Measure_String(m_fontTag, m_text.c_str());
	GetUITransform()->SetSizeDelta(m_textSize);
    m_isChangeText = false;
	}
	CUI::OnUpdate(timeDelta);
}

void CUIText::OnPrepareRender()
{
    CUIText::OnUpdate(0.f);
}

void CUIText::OnLateUpdate()
{
	CUI::OnLateUpdate();
}

HRESULT CUIText::OnRender()
{
	HRESULT hr = CUI::OnRender();
	if (m_text.empty() == false)
	{
		//Rect rc = m_pUITransformCom->Get_WorldRect();
		//float CenterX = rc.Left() + (rc.Right() - rc.Left()) * 0.5f;
		//float CenterY = rc.Top() + (rc.Bottom() - rc.Top()) * 0.5f;

		//Rect canvas = m_pGameInstance.lock()->Get_WinSize();
		//float screenHeight = canvas.Bottom();

		//
		//_float2 DrawPos;

		//DrawPos.x = rc.Left();
		//DrawPos.y = screenHeight - CenterY - (m_TextSize.y * 0.5f);
		//m_pGameInstance.lock()->Font_Draw(
		//	m_strFontTag,
		//	m_strText.c_str(),
		//	DrawPos,
		//	XMLoadFloat4(&m_vTextColor));

		Rect rc = m_UITransformCom->Get_WorldRect();

		float centerY = rc.Top() + (rc.Bottom() - rc.Top()) * 0.5f;

		float screenHeight = m_gameInstance.lock()->Get_WinSize().Bottom();

		_float2 drawPos;
		drawPos.x = rc.Left(); 
		drawPos.y = screenHeight - centerY - (m_textSize.y * 0.5f);

		m_gameInstance.lock()->Font_Draw(
			m_fontTag,
			m_text.c_str(),
			drawPos,
			XMLoadFloat4(&m_textColor)
		);
		/*switch (m_FontAline)
		{
		case FONTALINE::CENTER:
			DrawPos.x = CenterX - (m_TextSize.x * 0.5f);
			DrawPos.y = screenHeight - CenterY - (m_TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;

		case FONTALINE::RIGHT:
			DrawPos.x = rc.Right() - m_TextSize.x;
			DrawPos.y = screenHeight - CenterY - (m_TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;
		case FONTALINE::LEFT:
			DrawPos.x = rc.Left();
			DrawPos.y = screenHeight - CenterY - (m_TextSize.y * 0.5f);
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), DrawPos, XMLoadFloat4(&m_vTextColor));
			break;
		case FONTALINE::DEFAULT:
			m_pGameInstance.lock()->Font_Draw(m_strFontTag, m_strText.c_str(), _float2(rc.Left(), rc.Top()), XMLoadFloat4(&m_vTextColor));
			break;

		
		}*/

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


shared_ptr<CUIText> CUIText::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUIText> instance(new CUIText(device, context), [](CUIText* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIText");
	}
	return instance;

}

void CUIText::Free()
{
	CUI::Free();
}
