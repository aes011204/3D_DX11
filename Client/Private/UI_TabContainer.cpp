#include "UI_TabContainer.h"

CUI_TabContainer::CUI_TabContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_TabContainer::CUI_TabContainer(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_TabContainer::OnInit(void* pArg)
{

	TABCONTAINER_DESC* pDesc = static_cast<TABCONTAINER_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	pDesc->IsFullScreen = false;
	pDesc->IsTrnasparent = false;
	pDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	pDesc->TextureProtoName = L"Prototype_Component_Texture_TabContainer";
	//pDesc.vAnchorPoint = 일단 패스
	//	pDesc.vPivot
	//	pDesc.vAnchoredPos
	return CUIPanel::OnInit(pDesc);
}

void CUI_TabContainer::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_TabContainer::OnInActive()
{
	CUIPanel::OnInActive();
}

void CUI_TabContainer::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_TabContainer::OnUpdate(const _float& timeDelta)
{
	CUIPanel::OnUpdate(timeDelta);
}

void CUI_TabContainer::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_TabContainer::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_TabContainer::OnClear()
{
	CUIPanel::OnClear();
}

shared_ptr<CUI_TabContainer> CUI_TabContainer::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_TabContainer> pInstance(new CUI_TabContainer(pDevice, pContext), [](CUI_TabContainer* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_TabContainer::Free()
{
	CUIPanel::Free();
}
