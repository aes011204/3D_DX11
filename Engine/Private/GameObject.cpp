#include "GameObject.h"
#include "Transform.h"
#include "Shader.h"


CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEntity( pDevice ,pContext )
{
	
}

CGameObject::CGameObject(const CGameObject& prototype)
	:CEntity(prototype)
{
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*> (pArg);
	
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	

	//if (FAILED(__super::Initialize(pArg)))
	//	return E_FAIL;


	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->SetDefaultNameFromThisType();

	if (nullptr == Get_Component(g_strTransformTag))
	{
		m_Components.emplace(g_strTransformTag, m_pTransformCom);
		
	}


	// юс╫ц // ui ©К//

	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewPort = { 1 };
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPortDesc);

	m_fViewportWidth = ViewPortDesc.Width;
	m_fViewportHeight = ViewPortDesc.Height;

	m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_fX - m_fViewportWidth * 0.5f, -m_fY + m_fViewportHeight * 0.5f, 0.f, 1.f));


	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::VIEW)], XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], XMMatrixOrthographicLH(m_fViewportWidth, m_fViewportHeight, -100.f, 100.f));

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ETOI(eTransformState)]);
}

void CGameObject::Update_Transform()
{
	m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_fX - m_fViewportWidth * 0.5f, -m_fY + m_fViewportHeight * 0.5f, 0.f, 1.f));
}

void CGameObject::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
}
