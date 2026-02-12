#include "BackGround.h"

#include "GameInstance.h"

Client::CBackGround::CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

Client::CBackGround::CBackGround(const CBackGround& prototype)
	:CGameObject(prototype)
{

}

HRESULT Client::CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Client::CBackGround::Initialize(void* pArg)
{
	BACKGROUND_DESC Desc{};
	Desc.fSpeedPerSec = 1.f;
	Desc.fSpeedPerSec = 1.f;

	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */

	if(FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}

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
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT Client::CBackGround::Render()
{
	return S_OK;
}

void Client::CBackGround::OnGui()
{

}

shared_ptr<Client::CBackGround> Client::CBackGround::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CBackGround> pInstance(new CBackGround(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
	}
	return pInstance;
}


shared_ptr<CGameObject> Client::CBackGround::Clone(void* pArg)
{
	shared_ptr<CBackGround> pInstance(new CBackGround(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void Client::CBackGround::Free()
{
	__super::Free();
}
