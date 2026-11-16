#include "WaterEff.h"
#include "GameInstance.h"
#include "Model.h"
#include "PlayerBoat.h"
#include "VIBuffer_Particle_Point.h"
#include "Texture.h"


CWaterEff::CWaterEff(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
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

HRESULT CWaterEff::Initialize(void* arg)
{

	EffDesc* desc = static_cast<EffDesc*>(arg);

	m_type = desc->type;
	m_player = desc->target;

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Position( XMVectorSet(129.f * 0.5f, 30.f, 129.f * 0.5f, 1.f));
	//m_pTransformCom->Set_RotationDegree(_float3(0.f, 90.f, 0.f));
	//m_pSocketMatrix_AttackPoint = m_pModelCom->Get_BoneMatrixPtr(/*"l_botclaw_jnt"*/);
	//m_pTransformCom->Set_Position(XMVectorSet(50.f, 2.f, 1.f, 1.f));

	_vector pos = m_player.lock()->Get_TransformCom()->Get_State(ETransformState::POSITION);
	//m_pTransformCom->Set_Position(vPos);
	_float3 pos3;
	XMStoreFloat3(&pos3, pos);
	pos3.y = 0.2f;

	m_VIBufferCom->Set_Pivot(pos3);

	return S_OK;
}

void CWaterEff::Priority_Update(_float timeDelta)
{
}

void CWaterEff::Update(_float timeDelta)
{
	if (!m_player.lock())
	{
		m_VIBufferCom->Stop();
		m_VIBufferCom->Spawn(timeDelta);
		if (m_VIBufferCom->Is_Finished() == true)
		{
			Mark_Destroy();
		}
		return;
	}

	_vector pos = m_player.lock()->Get_TransformCom()->Get_State(ETransformState::POSITION);
	//m_pTransformCom->Set_Position(vPos);
	_float3 pos3;
	XMStoreFloat3(&pos3, pos);
	pos3.y = 0.2f;


	if (m_type == EEffType::PLAYER)
	{
		_vector vecPrevPos = XMLoadFloat3(&m_prevPos);
		_vector currentPos = XMLoadFloat3(&pos3);

	//m_pTransformCom->Set_Position(XMLoadFloat3(&pos3));
	m_VIBufferCom->Set_Pivot(pos3);

		_float distance = XMVectorGetX(XMVector3Length(currentPos - vecPrevPos));
		if (distance > 0.01f)
		{
			m_VIBufferCom->Start();
		}
		else
		{
			m_VIBufferCom->Stop();
		}
		m_prevPos = pos3;
		m_VIBufferCom->Spawn(timeDelta);
	}
	else
	{

		m_VIBufferCom->Spawn(timeDelta);
	}

}

void CWaterEff::Late_Update(_float timeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::BLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CWaterEff::Render()
{


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_VIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Render()))
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
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
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


	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticlePoint"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Texture_WaterEff"), TEXT("Com_Texture"), &m_textureCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_WaterEff"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;




	return S_OK;
}

shared_ptr<CWaterEff> CWaterEff::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CWaterEff> instance(new CWaterEff(device, context), [](CWaterEff* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterEff");
	}
	return instance;
}


shared_ptr<CGameObject> CWaterEff::Clone(void* arg)
{
	shared_ptr<CWaterEff> instance(new CWaterEff(*this), [](CWaterEff* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CWaterEff");
	}
	return instance;
}

void CWaterEff::Free()
{
	__super::Free();
}
