#include "BackGround.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"

//#include "CVIBuffer_Rect.h"
//#include "CShader.h"
//#include "CTexture.h"

CBackGround::CBackGround(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIObject(device, context)
{
}

CBackGround::CBackGround(const CBackGround& prototype)
	:CUIObject(prototype),
	m_shaderCom(prototype.m_shaderCom),
	m_VIBufferCom(prototype.m_VIBufferCom),
	m_textureCom(prototype.m_textureCom)
{

}

HRESULT Client::CBackGround::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CBackGround::Initialize(void* arg)
{


	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	BackGroundDesc			desc{};

	desc.degreePerSec = 90.f;
	desc.speedPerSec = 1.f;
	desc.x = 100.f;
	desc.y = 100.f;
	desc.sizeX = 200.f;
	desc.sizeY = 200.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Priority_Update(_float timeDelta)
{
	int a = 1;
}

void CBackGround::Update(_float timeDelta)
{

	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CBackGround::Late_Update(_float timeDelta)
{
	int a = 1;
	//m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI_WINDOW, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CBackGround::Render()
{

	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_shaderCom, "g_ViewMatrix", ED3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_shaderCom, "g_ProjMatrix", ED3DTS::PROJ)))
		return E_FAIL;


	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::OnGui()
{

}

HRESULT CBackGround::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),&m_VIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),&m_shaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(ELevel::LOGO), TEXT("Prototype_Component_Texture_BackGround"), TEXT("Com_Texture"),& m_textureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CBackGround> CBackGround::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CBackGround> instance(new CBackGround(device, context), [](CBackGround* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
	}
	return instance;
}


shared_ptr<CGameObject> CBackGround::Clone(void* arg)
{
	shared_ptr<CBackGround> instance(new CBackGround(*this), [](CBackGround* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return instance;
}

void CBackGround::Free()
{
	__super::Free();
}
