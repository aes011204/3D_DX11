#include "UI_Top.h"

#include <UIText.h>

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"

CUI_Top::CUI_Top(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_Top::CUI_Top(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_Top::OnInit(void* arg)
{

	{
		CUIImage::UIImageDesc timeBaseDesc = {};
		timeBaseDesc.textureComLevel = ETOI(ELevel::STATIC);
		timeBaseDesc.textureProtoName = L"Prototype_Component_Texture_HorizontalUITray";
		shared_ptr<CUIImage> timeBase = CUIImage::Create(m_device, m_context);
		timeBase->Initialize(&timeBaseDesc);
		Add_Child(timeBase, L"Time_Base", false);

		{
			CUIText::TextDesc textDayDesc = {};
			textDayDesc.fontTag = L"Noto_Sans_CJK_SC";
			textDayDesc.text = L"월요일 , 1일차";
			shared_ptr<CUIText> textDay = CUIText::Create(m_device, m_context);
			textDay->Initialize(&textDayDesc);
			timeBase->Add_Child(textDay, L"Text_Day", false);
			m_dayText = textDay;


			CUIText::TextDesc textTimeDesc = {};
			textTimeDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			textTimeDesc.text = L"12:34";
			shared_ptr<CUIText> textTime = CUIText::Create(m_device, m_context);
			textTime->Initialize(&textTimeDesc);
			timeBase->Add_Child(textTime, L"Text_Time", false);
			m_timeText = textTime;

		}

		CUIImage::UIImageDesc timeWheelDesc = {};
		timeWheelDesc.textureComLevel = ETOI(ELevel::STATIC);
		timeWheelDesc.textureProtoName = L"Prototype_Component_Texture_TimeOfDayWheel";
		shared_ptr<CUIImage> timeWheel = CUIImage::Create(m_device, m_context);
		timeWheel->Initialize(&timeWheelDesc);
		Add_Child(timeWheel, L"Time_Wheel", false);
		m_dayCircle = timeWheel;

		CUIImage::UIImageDesc timePointerDesc = {};
		timePointerDesc.textureComLevel = ETOI(ELevel::STATIC);
		timePointerDesc.textureProtoName = L"Prototype_Component_Texture_TimeOfDayPointer";
		shared_ptr<CUIImage> timePointer = CUIImage::Create(m_device, m_context);
		timePointer->Initialize(&timePointerDesc);
		timeWheel->Add_Child(timePointer, L"TimePointer", false);


	}

	{
		CUIImage::UIImageDesc compassDesc = {};
		compassDesc.textureComLevel = ETOI(ELevel::STATIC);
		compassDesc.textureProtoName = L"Prototype_Component_Texture_Compass";
		shared_ptr<CUIImage> compass = CUIImage::Create(m_device, m_context);
		compass->Initialize(&compassDesc);
		Add_Child(compass, L"Compass", false);


		CUIImage::UIImageDesc compassRingDesc = {};
		compassRingDesc.textureComLevel = ETOI(ELevel::STATIC);
		compassRingDesc.textureProtoName = L"Prototype_Component_Texture_CompassRing";
		shared_ptr<CUIImage> compassRing = CUIImage::Create(m_device, m_context);
		compassRing->Initialize(&compassRingDesc);
		Add_Child(compassRing, L"CompassRing", false);

	}


	return __super::OnInit(arg);
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
	_uint day = 0;
	_float hour = 0;
	_float minute = 0;
	_float second = 0;
	m_gameInstance.lock()->ComputeTime(day, hour, minute, second);
	if (minute != m_preMin)
	{

		m_timeText->Set_Text(format(L"{:02.0f}:{:02.0f}", hour, minute));

	}
	if (m_preDay != day)
	{
		wstring dayText = L"";
		_uint iday = day % 7;
		switch (iday)
		{
		case 0:
			dayText = L"월요일";
			break;
		case 1:
			dayText = L"화요일";
			break;
		case 2:
			dayText = L"수요일";
			break;
		case 3:
			dayText = L"목요일";
			break;
		case 4:
			dayText = L"금요일";
			break;
		case 5:
			dayText = L"토요일";
			break;
		case 6:
			dayText = L"일요일";
			break;
		default:
			break;
		}
		m_dayText->Set_Text(format(L"{}, {}일 차", dayText, day));
	}

	m_preDay = day;
	m_preMin = minute;


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

shared_ptr<CUI_Top> CUI_Top::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Top> instance(new CUI_Top(device, context), [](CUI_Top* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Top");

	}
	return instance;
}

void CUI_Top::Free()
{
	__super::Free();
}
