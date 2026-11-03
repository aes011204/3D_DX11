#include "UI_Time.h"

#include <UIImage.h>
#include <UIText.h>

#include "RotationModifier.h"
#include "UI_Controller.h"

CUI_Time::CUI_Time(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_Time::CUI_Time(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_Time::UIPannelActive(_uint time ,_bool IsSleep)
{

	

	m_gameInstance.lock()->Play_Loop(L"Hold_Active");

	m_IsSleep = IsSleep;

	

	if(m_IsSleep == true)
	{
		m_Text->Set_Text(L"휴식...");
	}
	else
	{
		m_Text->Set_Text(L"장비 설치중");

		m_StartTod = m_gameInstance.lock()->Get_TOD01();
		m_TargetTod = m_gameInstance.lock()->Get_TOD01() + time/24.f;

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

HRESULT CUI_Time::OnInit(void* pArg)
{



	UITIME_DESC* pDesc = static_cast<UITIME_DESC*>(pArg);


	//CUIPanel::UIPANEL_DESC LoadingDesc;
	pDesc->isFullScreen = true;
	pDesc->textureProtoName = L"Prototype_Component_Texture_Black";
	pDesc->textureComLevel = ETOI(LEVEL::STATIC);
	Set_Alpha(0.7f);

	//pDesc->NoDISTACH = true;

	 CUIPanel::OnInit(pArg);


	CUIImage::UIImageDesc SandTimerDesc;
	SandTimerDesc.textureComLevel = ETOI(LEVEL::STATIC);
	SandTimerDesc.textureProtoName = L"Prototype_Component_Texture_Timer";
	shared_ptr<CUIImage> pIcon = CUIImage::Create(m_device, m_context);
	pIcon->Initialize(&SandTimerDesc);
	//pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
	Add_Child(pIcon, L"ICON_TIME", false);
	m_iCon = pIcon;

	CUIText::TextDesc textDesc;
	//TextDesc.TextColor = _float4{ 1.f,0.f };
	textDesc.fontTag = L"Noto_Sans_CJK_SC_32";
	textDesc.text = L"휴식...";
	textDesc.fontAlign = CUIText::EFontAlign::DEFAULT;
	shared_ptr<CUIText> text = CUIText::Create(m_device, m_context);
	text->Initialize(&textDesc);
	Add_Child(text, L"text_TIME", false);

	m_Text = text;

	CUIText::TextDesc TextDesc1;
	//TextDesc.TextColor = _float4{ 1.f,0.f };
	TextDesc1.fontTag = L"Noto_Sans_CJK_SC_24";
	TextDesc1.text = L"X : 나가기";
	TextDesc1.fontAlign = CUIText::EFontAlign::DEFAULT;
	shared_ptr<CUIText> text1 = CUIText::Create(m_device, m_context);
	text1->Initialize(&TextDesc1);
	Add_Child(text1, L"text_Exit", false);



	//

	CUIImage::UIImageDesc CircleDesc = {};
	CircleDesc.textureComLevel = ETOI(LEVEL::STATIC);
	CircleDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	CircleDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_device, m_context);
	Circle->Initialize(&CircleDesc);
	Circle->Set_BaseColor(_float4{ 0.f,0.f,0.f, 1.f });
	Circle->Set_ZoneColor(_float4{ 1.f,0.f,0.f, 1.f });

	Add_Child(Circle, L"Circle", false);
	m_pCircle = Circle;
	m_CircleShader = dynamic_pointer_cast<CShader>(Circle->Get_Component(L"Com_Shader"));


	m_TargetAngle = -180;
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
	
	if(result != true)
	{
	result = m_iCon->GetUITransform()->SetRotationTo(m_TargetAngle, timeDelta);
		
	}

	if(result==true)
	{
		m_Acc += timeDelta;
		if(m_Acc > 0.5f)
		{
		m_TargetAngle -= 180.f;
		m_Acc = 0.f;
		result = false;
		}
	}


	if(m_IsSleep==true)
	{
		
		m_gameInstance.lock()->Set_TimeScale(20.f);
	}
	else
	{
		m_gameInstance.lock()->Set_TimeScale(20.f);
		m_CurrentTod = m_gameInstance.lock()->Get_TOD01();
		_float fRatio = (m_CurrentTod - m_StartTod) / (m_TargetTod - m_StartTod);
		m_zones[0].end = fRatio;
		
			if (fRatio >= 1.f)
			{
				fRatio = 1.f;

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
	if (m_CircleShader != nullptr)
	{
		m_CircleShader->Bind_RawValue("g_zones", m_zones, sizeof(_float4) * 8);
		m_CircleShader->Bind_RawValue("g_zoneCount", &m_zoneCount, sizeof(float));

	}
	return __super::OnRender();
}

void CUI_Time::OnClear()
{
	__super::OnClear();
}



shared_ptr<CUI_Time> CUI_Time::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Time> pInstance(new CUI_Time(pDevice, pContext), [](CUI_Time* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Time");

	}
	return pInstance;
}

void CUI_Time::Free()
{
	__super::Free();
}
