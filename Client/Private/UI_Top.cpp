#include "UI_Top.h"

#include <UIText.h>

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"

CUI_Top::CUI_Top(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_Top::CUI_Top(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_Top::OnInit(void* pArg)
{

	{
		CUIImage::UIImageDesc Time_BaseDesc = {};
		Time_BaseDesc.textureComLevel = ETOI(ELevel::STATIC);
		Time_BaseDesc.textureProtoName = L"Prototype_Component_Texture_HorizontalUITray";
		shared_ptr<CUIImage> Time_Base = CUIImage::Create(m_device, m_context);
		Time_Base->Initialize(&Time_BaseDesc);
		Add_Child(Time_Base, L"Time_Base", false);

		{
			CUIText::TextDesc textDay_Desc = {};
			textDay_Desc.fontTag = L"Noto_Sans_CJK_SC";
			textDay_Desc.text = L"월요일 , 1일차";
			shared_ptr<CUIText> Text_Day = CUIText::Create(m_device, m_context);
			Text_Day->Initialize(&textDay_Desc);
			Time_Base->Add_Child(Text_Day, L"Text_Day", false);
			m_dayText = Text_Day;


			CUIText::TextDesc textTime_Desc = {};
			textTime_Desc.fontTag = L"Noto_Sans_CJK_SC_24";
			textTime_Desc.text = L"12:34";
			shared_ptr<CUIText> Text_Time = CUIText::Create(m_device, m_context);
			Text_Time->Initialize(&textTime_Desc);
			Time_Base->Add_Child(Text_Time, L"Text_Time", false);
			m_timeText = Text_Time;

		}

		CUIImage::UIImageDesc TimeWheelDesc = {};
		TimeWheelDesc.textureComLevel = ETOI(ELevel::STATIC);
		TimeWheelDesc.textureProtoName = L"Prototype_Component_Texture_TimeOfDayWheel";
		shared_ptr<CUIImage> Time_Wheel = CUIImage::Create(m_device, m_context);
		Time_Wheel->Initialize(&TimeWheelDesc);
		Add_Child(Time_Wheel, L"Time_Wheel", false);
		m_dayCircle = Time_Wheel;

		CUIImage::UIImageDesc TimePointerDesc = {};
		TimePointerDesc.textureComLevel = ETOI(ELevel::STATIC);
		TimePointerDesc.textureProtoName = L"Prototype_Component_Texture_TimeOfDayPointer";
		shared_ptr<CUIImage> TimePointer = CUIImage::Create(m_device, m_context);
		TimePointer->Initialize(&TimePointerDesc);
		Time_Wheel->Add_Child(TimePointer, L"TimePointer", false);


	}

	{
		CUIImage::UIImageDesc CompassDesc = {};
		CompassDesc.textureComLevel = ETOI(ELevel::STATIC);
		CompassDesc.textureProtoName = L"Prototype_Component_Texture_Compass";
		shared_ptr<CUIImage> Compass = CUIImage::Create(m_device, m_context);
		Compass->Initialize(&CompassDesc);
		Add_Child(Compass, L"Compass", false);


		CUIImage::UIImageDesc CompassRingDesc = {};
		CompassRingDesc.textureComLevel = ETOI(ELevel::STATIC);
		CompassRingDesc.textureProtoName = L"Prototype_Component_Texture_CompassRing";
		shared_ptr<CUIImage> CompassRing = CUIImage::Create(m_device, m_context);
		CompassRing->Initialize(&CompassRingDesc);
		Add_Child(CompassRing, L"CompassRing", false);

	}


	return __super::OnInit(pArg);
}

void CUI_Top::OnActive()
{
	__super::OnActive();
}

void CUI_Top::OnInActive()
{
	__super::OnInActive();
}

void CUI_Top::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Top::OnUpdate(const _float& timeDelta)
{
	_uint iDay = 0;
	_float fHour = 0;
	_float fMinute = 0;
	_float fSecond = 0;
	m_gameInstance.lock()->ComputeTime(iDay, fHour, fMinute, fSecond);
	if (fMinute != preMin)
	{

		m_timeText->Set_Text(format(L"{:02.0f}:{:02.0f}", fHour, fMinute));

	}
	if (preDay != iDay)
	{
		wstring strDay = L"";
		_uint iday = iDay % 7;
		switch (iday)
		{
		case 0:
			strDay = L"월요일";
			break;
		case 1:
			strDay = L"화요일";
			break;
		case 2:
			strDay = L"수요일";
			break;
		case 3:
			strDay = L"목요일";
			break;
		case 4:
			strDay = L"금요일";
			break;
		case 5:
			strDay = L"토요일";
			break;
		case 6:
			strDay = L"일요일";
			break;
		default:
			break;
		}
		m_dayText->Set_Text(format(L"{}, {}일 차", strDay, iDay));
	}

	preDay = iDay;
	preMin = fMinute;


	float circel = m_gameInstance.lock()->Get_TOD01() * -360.f;
	m_dayCircle->GetUITransform()->SetRotation(circel);


	__super::OnUpdate(timeDelta);
}

void CUI_Top::OnLateUpdate()
{
	__super::OnLateUpdate();
}

HRESULT CUI_Top::OnRender()
{
	return __super::OnRender();
}

void CUI_Top::OnClear()
{
	__super::OnClear();
}

shared_ptr<CUI_Top> CUI_Top::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Top> pInstance(new CUI_Top(pDevice, pContext), [](CUI_Top* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Top");

	}
	return pInstance;
}

void CUI_Top::Free()
{
	__super::Free();
}
