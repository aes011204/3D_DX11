#include "EmptyGameObject.h"
#include "BackGround.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"

#include "VIBuffer_Rect.h"
//#include "CShader.h"
//#include "CTexture.h"

CEmptyGameObject::CEmptyGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CEmptyGameObject::CEmptyGameObject(const CEmptyGameObject& prototype)
	:CGameObject(prototype),
	m_pShaderCom(prototype.m_pShaderCom),
	m_pVIBufferCom(prototype.m_pVIBufferCom),
	m_pTextureCom(prototype.m_pTextureCom)
{

}

HRESULT Client::CEmptyGameObject::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CEmptyGameObject::Initialize(void* pArg)
{

	/* 멤버를 채워넣어야한다면 여기서 채운다. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEmptyGameObject::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void CEmptyGameObject::Update(_float fTimeDelta)
{
	if (m_bIsDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_bIsDirtyCom = false;
	}
	int a = 1;
}

void CEmptyGameObject::Late_Update(_float fTimeDelta)
{
	int a = 1;
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CEmptyGameObject::Render()
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

void CEmptyGameObject::OnGui()
{

}

void CEmptyGameObject::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pVIBufferCom = Get_Component<VIBuffer>(L"Com_VIBuffer");
}

HRESULT CEmptyGameObject::Ready_Components()
{
	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), m_pVIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), m_pTextureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CEmptyGameObject> CEmptyGameObject::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CEmptyGameObject> pInstance(new CEmptyGameObject(pDevice, pContext), [](CEmptyGameObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEmptyGameObject");
	}
	return pInstance;
}


shared_ptr<CGameObject> CEmptyGameObject::Clone(void* pArg)
{
	shared_ptr<CEmptyGameObject> pInstance(new CEmptyGameObject(*this), [](CEmptyGameObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void CEmptyGameObject::Free()
{
	__super::Free();
}
