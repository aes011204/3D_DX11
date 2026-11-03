#include "UI_Time.h"

#include <UIImage.h>
#include <UIText.h>

#include "RotationModifier.h"
#include "UI_Controller.h"

CUI_Time::CUI_Time(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_Time::CUI_Time(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_Time::UIPannelActive(_uint time ,_bool isSleep)
{

	

	m_gameInstance.lock()->Play_Loop(L"Hold_Active");

	m_isSleep = isSleep;

	

	if(m_isSleep == true)
	{
		m_text->Set_Text(L"휴식...");
	}
	else
	{
		m_text->Set_Text(L"장비 설치중");

		m_startTod = m_gameInstance.lock()->Get_TOD01();
		m_targetTod = m_gameInstance.lock()->Get_TOD01() + time/24.f;

	}

	Set_ActiveForCustom();

	m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

	OnActive();

	for (auto& it : m_children)
	{
		it->UI_Active();
	}


}

HRESULT CUI_Time::OnInit(void* arg)
{



	UITimeDesc* desc = static_cast<UITimeDesc*>(arg);


	//CUIPanel::UIPANEL_DESC LoadingDesc;
	desc->isFullScreen = true;
	desc->textureProtoName = L"Prototype_Component_Texture_Black";
	desc->textureComLevel = ETOI(ELevel::STATIC);
	Set_Alpha(0.7f);

	//pDesc->NoDISTACH = true;

	 CUIPanel::OnInit(arg);


	CUIImage::UIImageDesc sandTimerDesc;
	sandTimerDesc.textureComLevel = ETOI(ELevel::STATIC);
	sandTimerDesc.textureProtoName = L"Prototype_Component_Texture_Timer";
	shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
	icon->Initialize(&sandTimerDesc);
	//pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
	Add_Child(icon, L"ICON_TIME", false);
	m_icon = icon;

	CUIText::TextDesc textDesc;
	//TextDesc.TextColor = _float4{ 1.f,0.f };
	textDesc.fontTag = L"Noto_Sans_CJK_SC_32";
	textDesc.text = L"휴식...";
	textDesc.fontAlign = CUIText::EFontAlign::DEFAULT;
	shared_ptr<CUIText> text = CUIText::Create(m_device, m_context);
	text->Initialize(&textDesc);
	Add_Child(text, L"text_TIME", false);

	m_text = text;

	CUIText::TextDesc textDesc1;
	//TextDesc.TextColor = _float4{ 1.f,0.f };
	textDesc1.fontTag = L"Noto_Sans_CJK_SC_24";
	textDesc1.text = L"X : 나가기";
	textDesc1.fontAlign = CUIText::EFontAlign::DEFAULT;
	shared_ptr<CUIText> text1 = CUIText::Create(m_device, m_context);
	text1->Initialize(&textDesc1);
	Add_Child(text1, L"text_Exit", false);



	//

	CUIImage::UIImageDesc circleDesc = {};
	circleDesc.textureComLevel = ETOI(ELevel::STATIC);
	circleDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	circleDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_device, m_context);
	Circle->Initialize(&circleDesc);
	Circle->Set_BaseColor(_float4{ 0.f,0.f,0.f, 1.f });
	Circle->Set_ZoneColor(_float4{ 1.f,0.f,0.f, 1.f });

	Add_Child(Circle, L"Circle", false);
	m_circle = Circle;
	m_circleShader = dynamic_pointer_cast<CShader>(Circle->Get_Component(L"Com_Shader"));


	m_targetAngle = -180;
	//
	Set_Interactive(false);
	return S_OK;
}

void CUI_Time::OnActive()
{
	ZeroMemory(m_zones, sizeof(m_zones));
	CUI_Controller::GetInstance()->InActiveHover();
	__super::OnActive();
}

void CUI_Time::OnInActive()
{
	__super::OnInActive();
	CUI_Controller::GetInstance()->ActiveHover();
	m_gameInstance.lock()->Set_TimeScale(1.f);

	m_gameInstance.lock()->Stop(L"Hold_Active");
	
}

void CUI_Time::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Time::OnUpdate(const _float& timeDelta)
{
	
	if(m_isRotationDone != true)
	{
	m_isRotationDone = m_icon->GetUITransform()->SetRotationTo(m_targetAngle, timeDelta);
		
	}

	if(m_isRotationDone==true)
	{
		m_acc += timeDelta;
		if(m_acc > 0.5f)
		{
		m_targetAngle -= 180.f;
		m_acc = 0.f;
		m_isRotationDone = false;
		}
	}


	if(m_isSleep==true)
	{
		
		m_gameInstance.lock()->Set_TimeScale(20.f);
	}
	else
	{
		m_gameInstance.lock()->Set_TimeScale(20.f);
		m_currentTod = m_gameInstance.lock()->Get_TOD01();
		_float ratio = (m_currentTod - m_startTod) / (m_targetTod - m_startTod);
		m_zones[0].end = ratio;
		
			if (ratio >= 1.f)
			{
				ratio = 1.f;

				UI_InActive();
				
				m_gameInstance.lock()->Play_Once(L"Hold_Complete");
					return; // 즉시 리컨
			}
	}
	__super::OnUpdate(timeDelta);
}

void CUI_Time::OnLateUpdate()
{
	__super::OnLateUpdate();
}

HRESULT CUI_Time::OnRender()
{
	if (m_circleShader != nullptr)
	{
		m_circleShader->Bind_RawValue("g_zones", m_zones, sizeof(_float4) * 8);
		m_circleShader->Bind_RawValue("g_zoneCount", &m_zoneCount, sizeof(float));

	}
	return __super::OnRender();
}

void CUI_Time::OnClear()
{
	__super::OnClear();
}



shared_ptr<CUI_Time> CUI_Time::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Time> instance(new CUI_Time(device, context), [](CUI_Time* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Time");

	}
	return instance;
}

void CUI_Time::Free()
{
	__super::Free();
}
