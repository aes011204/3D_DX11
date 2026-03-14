#include "PlayerBoat.h"
#include "GameInstance.h"
#include "Model.h"
#include "DInput_Manager.h"

CPlayerBoat::CPlayerBoat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CPlayerBoat::CPlayerBoat(const CPlayerBoat& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CPlayerBoat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerBoat::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayerBoat::Priority_Update(_float fTimeDelta)
{
}

void CPlayerBoat::Update(_float fTimeDelta)
{
	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();


	if (dinput->KeyPress(DIK_UP))
	{
		m_pTransformCom->Go_Forward(fTimeDelta*50.f);
	}

	if (dinput->KeyPress(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}


	if (dinput->KeyPress(DIK_E))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}


	if (dinput->KeyPress(DIK_Q))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}




	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayerBoat::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CPlayerBoat::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

void CPlayerBoat::OnGui()
{
}

void CPlayerBoat::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	//m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}

HRESULT CPlayerBoat::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerBoat::Ready_Components()
{
	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_PlayerBoat"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;


	return S_OK;
}

shared_ptr<CPlayerBoat> CPlayerBoat::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CPlayerBoat> pInstance(new CPlayerBoat(pDevice, pContext), [](CPlayerBoat* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerBoat");
	}
	return pInstance;
}


shared_ptr<CGameObject> CPlayerBoat::Clone(void* pArg)
{
	shared_ptr<CPlayerBoat> pInstance(new CPlayerBoat(*this), [](CPlayerBoat* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerBoat");
	}
	return pInstance;
}

void CPlayerBoat::Free()
{
	__super::Free();
}
