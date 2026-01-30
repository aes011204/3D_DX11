#include "BackGround.h"

Client::CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

Client::CBackGround::CBackGround(const CBackGround& rhs)
	:CGameObject(rhs)
{
}

HRESULT Client::CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Client::CBackGround::Initialize(void* pArg)
{
	return S_OK;
}

void Client::CBackGround::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void Client::CBackGround::Update(_float fTimeDelta)
{
	int a = 1;

}

void Client::CBackGround::Late_Update(_float fTimeDelta)
{
	int a = 1;

}

HRESULT Client::CBackGround::Render()
{
	return S_OK;
}

void Client::CBackGround::OnGui()
{
	ImGui::Begin("hi");
	ImGui::Text("this is backGround");
	ImGui::End();
}

Client::CBackGround* Client::CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Client::CBackGround* Client::CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CBackGround::Free()
{
	__super::Free();
}
