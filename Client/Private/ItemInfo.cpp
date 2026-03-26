#include "ItemInfo.h"

CItemInfo::CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CItemInfo::CItemInfo(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CItemInfo::UI_PanelActive(_bool isHold, Item_Inst itemDef, LOCATIONSTATE locationState)
{


}

void CItemInfo::UI_Active()
{
	CUIPanel::UI_Active();
}

HRESULT CItemInfo::OnInit(void* pArg)
{
	HRESULT hr = E_FAIL;
	ITEMINFO_DESC* INFODesc = static_cast<ITEMINFO_DESC*>(pArg);
	hr = CUIPanel::OnInit(INFODesc);



	CUIImage::UIIMAGE_DESC img = {};
	img.TextureComLevel = ETOI(LEVEL::STATIC);
	img.TextureProtoName = L"Prototype_Component_Texture_TabDivider";
	img.bUseNineSlice = true;
	shared_ptr<CUIImage> Line = CUIImage::Create(m_pDevice, m_pContext);
	Line->Initialize(&img);
	Line->Set_Zorder(1);

	Add_Child(Line, L"Line", false);
	m_Line = Line;





	return CUIPanel::OnInit(pArg);
}

void CItemInfo::OnActive()
{
	CUIPanel::OnActive();
}

void CItemInfo::OnInActive()
{
	CUIPanel::OnInActive();
}

void CItemInfo::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CItemInfo::OnUpdate(const _float& timeDelta)
{
	CUIPanel::OnUpdate(timeDelta);
}

void CItemInfo::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CItemInfo::OnRender()
{
	return CUIPanel::OnRender();
}

void CItemInfo::OnClear()
{
	CUIPanel::OnClear();
}

shared_ptr<CItemInfo> CItemInfo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CItemInfo> pInstance(new CItemInfo(pDevice, pContext), [](CItemInfo* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemInfo");

	}
	return pInstance;
}

void CItemInfo::Free()
{
	CUIPanel::Free();
}
