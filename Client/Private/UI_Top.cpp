#include "UI_Top.h"

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
		CUIImage::UIIMAGE_DESC Time_BaseDesc = {};
		Time_BaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		Time_BaseDesc.TextureProtoName = L"Prototype_Component_Texture_HorizontalUITray";
		shared_ptr<CUIImage> Time_Base = CUIImage::Create(m_pDevice, m_pContext);
		Time_Base->Initialize(&Time_BaseDesc);
		Add_Child(Time_Base, L"Time_Base", false);

		{
			CUIImage::UIIMAGE_DESC TimeWheelDesc = {};
			TimeWheelDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			TimeWheelDesc.TextureProtoName = L"Prototype_Component_Texture_TimeOfDayWheel";
			shared_ptr<CUIPanel> Time_Wheel = CUIPanel::Create(m_pDevice, m_pContext);
			Time_Wheel->Initialize(&TimeWheelDesc);
			Add_Child(Time_Wheel, L"Time_Wheel", false);


			CUIImage::UIIMAGE_DESC TimePointerDesc = {};
			TimePointerDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			TimePointerDesc.TextureProtoName = L"Prototype_Component_Texture_TimeOfDayPointer";
			shared_ptr<CUIPanel> TimePointer = CUIPanel::Create(m_pDevice, m_pContext);
			TimePointer->Initialize(&TimePointerDesc);
			Time_Wheel->Add_Child(TimePointer, L"TimePointer", false);
		}

	}

	{
		CUIImage::UIIMAGE_DESC CompassDesc = {};
		CompassDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		CompassDesc.TextureProtoName = L"Prototype_Component_Texture_Compass";
		shared_ptr<CUIImage> Compass = CUIImage::Create(m_pDevice, m_pContext);
		Compass->Initialize(&CompassDesc);
		Add_Child(Compass, L"Compass", false);


		CUIImage::UIIMAGE_DESC CompassRingDesc = {};
		CompassRingDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		CompassRingDesc.TextureProtoName = L"Prototype_Component_Texture_CompassRing";
		shared_ptr<CUIPanel> CompassRing = CUIPanel::Create(m_pDevice, m_pContext);
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
