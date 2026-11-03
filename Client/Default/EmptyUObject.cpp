#include "EmptyUObject.h"
#include "BackGround.h"

#include "GameInstance.h"
#include "Transform.h"
#include "PipeLine.h"
#include "Entity.h"

#include "VIBuffer_Rect.h"
//#include "CShader.h"
//#include "CTexture.h"

CEmptyUObject::CEmptyUObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CEmptyUObject::CEmptyUObject(const CEmptyUObject& prototype)
	:CGameObject(prototype),
	m_shaderCom(prototype.m_shaderCom),
	m_VIBufferCom(prototype.m_VIBufferCom),
	m_textureCom(prototype.m_textureCom)
{

}

HRESULT Client::CEmptyUObject::Initialize_Prototype()
{
	//m_bIsSavableClass = true;

	return S_OK;
}

HRESULT CEmptyUObject::Initialize(void* pArg)
{
	m_isSavableClass = true;
	/* 멤버를 채워넣어야한다면 여기서 채운다. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEmptyUObject::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void CEmptyUObject::Update(_float fTimeDelta)
{
	if (m_isDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_isDirtyCom = false;
	}
	int a = 1;
}

void CEmptyUObject::Late_Update(_float fTimeDelta)
{
	int a = 1;
	//m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CEmptyUObject::Render()
{


	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
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

void CEmptyUObject::OnGui()
{

}

void CEmptyUObject::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_VIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	m_shaderCom = Get_Component<CShader>(L"Com_Shader");
}

HRESULT CEmptyUObject::Ready_Components()
{
	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_textureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CEmptyUObject> CEmptyUObject::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CEmptyUObject> pInstance(new CEmptyUObject(pDevice, pContext), [](CEmptyUObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEmptyUObject");
	}
	return pInstance;
}


shared_ptr<CGameObject> CEmptyUObject::Clone(void* pArg)
{
	shared_ptr<CEmptyUObject> pInstance(new CEmptyUObject(*this), [](CEmptyUObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void CEmptyUObject::Free()
{
	__super::Free();
}
