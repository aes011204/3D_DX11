#include "UI_Box.h"

#include <UIText.h>

#include "UIButton.h"

#include "UI_Inventory.h"
#include "UI_Storage.h"
#include "UIImage.h"
#include "EventBus.h"


CUI_Box::CUI_Box(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_Box::CUI_Box(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

void CUI_Box::UI_Active()
{

	//m_bEnabled = true;
	//m_bVisible = true;

	//m_bRenderReady = false;

	//if (m_pUITransformCom)
	//	m_pUITransformCom->UpdateLayoutIfDirty();

	////m_bInteractable = true;
	//OnActive();

	////for (auto& it : m_Children)
	////{
	////	it->UI_Active();
	////}


}

void CUI_Box::UI_PanelActive(_bool IsStorage, _bool IsBox, _uint BoxNum )
{
	if(IsStorage == true)
	{
		m_Name_Text->Set_Text(L"내 물품 보관함");

	}
	else
	{
		m_Name_Text->Set_Text(L"발견한 아이템");
	}


	//m_Children.clear();
	_uint numPanel = {};
	// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	//::OnActive() 에서 클릭한 거만 활성화
	
	m_Name_Text->UI_Active();
	m_Storage->UI_Active();



	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;

	//for (auto& it : m_Children)
	//{
	//	it->UI_Active();
	//}
	OnActive();

}

HRESULT CUI_Box::OnInit(void* pArg)
{




	HRESULT hr = E_FAIL;
	BOX_DESC* BoxDesc = static_cast<BOX_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	BoxDesc->IsFullScreen = false;
	BoxDesc->IsTransparent = false;
	BoxDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	BoxDesc->TextureProtoName = L"Prototype_Component_Texture_TabContainer";
	//BoxDesc->IsUseLayout = true;
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos

	hr = CUIPanel::OnInit(BoxDesc);


	CUIImage::UIIMAGE_DESC nameBaseDesc = {};
	nameBaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	nameBaseDesc.TextureProtoName = L"Prototype_Component_Texture_TitleBackground";
	shared_ptr<CUIImage> NameBase = CUIImage::Create(m_pDevice, m_pContext);
	NameBase->Initialize(&nameBaseDesc);
	Add_Child(NameBase, L"NameBase", false);

	{
		CUIText::TEXT_DESC nameDesc = {};
		nameDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
		nameDesc.strText = L"내 물품 보관함";
		shared_ptr<CUIText> Name_Text = CUIText::Create(m_pDevice, m_pContext);
		Name_Text->Initialize(&nameDesc);

		NameBase->Add_Child(Name_Text, L"Name_Text", false);
		m_Name_Text = Name_Text;
	}
	


	

		// 일단 테스트 인벤 3개
		CUI_Storage::STORAGE_DESC StorageDesc = {};
		StorageDesc.IsFullScreen = false;
		StorageDesc.IsTransparent = true;
		StorageDesc.bSetParentSize = true;

		shared_ptr<CUI_Storage> pStorage = CUI_Storage::Create(m_pDevice, m_pContext);
		pStorage->Initialize(&StorageDesc);


		Add_Child(pStorage, L"Storage2", false);
		m_Storage = pStorage;
		pStorage->UI_InActive();
	


	
	return hr;
}

void CUI_Box::OnActive()
{
	

	GetUITransform()->SetAnchoredPos(Vector2{ GetUITransform()->Get_FinalSize().x, 0.f });
	m_vecAni = Vector2{ GetUITransform()->Get_FinalSize().x, 0.f };
	m_bStart = true;
	m_fDuration = 1.5f;

	CUIPanel::OnActive();
}

void CUI_Box::OnInActive()
{
	m_bStart = false;
	m_TimeAcc = 0;
	
	CUIPanel::OnInActive();
}

void CUI_Box::OnDisabled()
{

	CUIPanel::OnDisabled();
}

void CUI_Box::OnUpdate(const _float& timeDelta)
{
	// 여기에서 안보였다가 와야함
	if (m_bStart == true)
	{
		m_TimeAcc += timeDelta;

		float t = m_TimeAcc / m_fDuration;

		if (t >= 1.f) t = 1.f;

		m_vecAni = Vector2{ lerp(-m_vecAni.x, 0.f, t),0.f };
		GetUITransform()->SetAnchoredPos(m_vecAni);
		//LOG_F(LOG_LEVEL::INFO, "m_vecAni%d", m_vecAni);

		if (t >= 1.f) {
			m_bStart = false;
			m_TimeAcc = 0;
		}
	}

	CUIPanel::OnUpdate(timeDelta);
}

void CUI_Box::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_Box::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_Box::OnClear()
{
	CUIPanel::OnClear();
}



shared_ptr<CUI_Box> CUI_Box::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Box> pInstance(new CUI_Box(pDevice, pContext), [](CUI_Box* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_Box::Free()
{
	/*for (int i = 0; i < 32; ++i)
	{
		m_TabContents[i].reset();
		
	}*/

	CUIPanel::Free();
}
