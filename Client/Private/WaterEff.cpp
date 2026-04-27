#include "WaterEff.h"
#include "GameInstance.h"
#include "Model.h"
#include "PlayerBoat.h"
#include "VIBuffer_Particle_Point.h"
#include "Texture.h"


CWaterEff::CWaterEff(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CWaterEff::CWaterEff(const CWaterEff& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CWaterEff::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterEff::Initialize(void* pArg)
{

	EffDESC* pDesc = static_cast<EffDESC*>(pArg);

	eType = pDesc->eType;
	m_pPlayer = pDesc->Target;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Position( XMVectorSet(129.f * 0.5f, 30.f, 129.f * 0.5f, 1.f));
	//m_pTransformCom->Set_RotationDegree(_float3(0.f, 90.f, 0.f));
	//m_pSocketMatrix_AttackPoint = m_pModelCom->Get_BoneMatrixPtr(/*"l_botclaw_jnt"*/);
	//m_pTransformCom->Set_Position(XMVectorSet(50.f, 2.f, 1.f, 1.f));

	_vector vPos = m_pPlayer.lock()->Get_TransformCom()->Get_State(STATE::POSITION);
	//m_pTransformCom->Set_Position(vPos);
	_float3 pos3;
	XMStoreFloat3(&pos3, vPos);
	pos3.y = 0.2f;

	m_pVIBufferCom->Set_Pivot(pos3);

	return S_OK;
}

void CWaterEff::Priority_Update(_float fTimeDelta)
{
}

void CWaterEff::Update(_float fTimeDelta)
{
	if (!m_pPlayer.lock())
	{
		m_pVIBufferCom->Stop();
		m_pVIBufferCom->Spawn(fTimeDelta);
		if (m_pVIBufferCom->Is_Finished() == true)
		{
			Mark_Destroy();
		}
		return;
	}

	_vector vPos = m_pPlayer.lock()->Get_TransformCom()->Get_State(STATE::POSITION);
	//m_pTransformCom->Set_Position(vPos);
	_float3 pos3;
	XMStoreFloat3(&pos3, vPos);
	pos3.y = 0.2f;


	if (eType == EFFTYPE::PLAYER)
	{
		_vector vPrevPos = XMLoadFloat3(&PrevPos);
		_vector vCurrentPos = XMLoadFloat3(&pos3);

	//m_pTransformCom->Set_Position(XMLoadFloat3(&pos3));
	m_pVIBufferCom->Set_Pivot(pos3);

		_float fDistance = XMVectorGetX(XMVector3Length(vCurrentPos - vPrevPos));
		if (fDistance > 0.01f)
		{
			m_pVIBufferCom->Start();
		}
		else
		{
			m_pVIBufferCom->Stop();
		}
		PrevPos = pos3;
		m_pVIBufferCom->Spawn(fTimeDelta);
	}
	else
	{

		m_pVIBufferCom->Spawn(fTimeDelta);
	}

}

void CWaterEff::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::BLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CWaterEff::Render()
{


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;





	return S_OK;
}

void CWaterEff::OnGui()
{
}

void CWaterEff::RebindCom()
{

	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	//m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CWaterEff::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	/*const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CWaterEff::Ready_Components()
{


	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticlePoint"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_WaterEff"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_WaterEff"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
		return E_FAIL;




	return S_OK;
}

shared_ptr<CWaterEff> CWaterEff::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CWaterEff> pInstance(new CWaterEff(pDevice, pContext), [](CWaterEff* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterEff");
	}
	return pInstance;
}


shared_ptr<CGameObject> CWaterEff::Clone(void* pArg)
{
	shared_ptr<CWaterEff> pInstance(new CWaterEff(*this), [](CWaterEff* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterEff");
	}
	return pInstance;
}

void CWaterEff::Free()
{
	__super::Free();
}
