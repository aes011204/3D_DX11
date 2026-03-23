#include "BackGround.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"

//#include "CVIBuffer_Rect.h"
//#include "CShader.h"
//#include "CTexture.h"

CBackGround::CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& prototype)
	:CUIObject(prototype),
	m_pShaderCom(prototype.m_pShaderCom),
	m_pVIBufferCom(prototype.m_pVIBufferCom),
	m_pTextureCom(prototype.m_pTextureCom)
{

}

HRESULT Client::CBackGround::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	BACKGROUND_DESC			Desc{};

	Desc.fDegreePerSec = 90.f;
	Desc.fSpeedPerSec = 1.f;
	Desc.fX = 100.f;
	Desc.fY = 100.f;
	Desc.fSizeX = 200.f;
	Desc.fSizeY = 200.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void CBackGround::Update(_float fTimeDelta)
{

	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CBackGround::Late_Update(_float fTimeDelta)
{
	int a = 1;
	//m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI_WINDOW, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CBackGround::Render()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::OnGui()
{

}

HRESULT CBackGround::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),&m_pVIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),&m_pShaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"), TEXT("Com_Texture"),& m_pTextureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CBackGround> CBackGround::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CBackGround> pInstance(new CBackGround(pDevice, pContext), [](CBackGround* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
	}
	return pInstance;
}


shared_ptr<CGameObject> CBackGround::Clone(void* pArg)
{
	shared_ptr<CBackGround> pInstance(new CBackGround(*this), [](CBackGround* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();
}
