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

	PLAYERBOAT_DESC pDesc = {};
	pDesc.fSpeedPerSec = 10.f;
	pDesc.fDegreePerSec = 25.f;

	if (FAILED(__super::Initialize(&pDesc)))
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

	//
	_float4 upDir = { 0.f, 1.f, 0.f, 0.f };

	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();


	if (dinput->KeyPress(DIK_UP))
	{
		m_pTransformCom->Go_Forward(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_RIGHT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir),fTimeDelta);
	}

	if (dinput->KeyPress(DIK_LEFT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), -fTimeDelta);
	}

	// 임시코드 ///////////////// 3점 -> 4점으로 수정예정 + 코드 정리 
	_vector CurPos = m_pTransformCom->Get_Position();

	_float fOut0 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut0);
	_float3 fianlPos = { XMVectorGetX(CurPos), fOut0, XMVectorGetZ(CurPos) };
	m_pTransformCom->Set_Position(XMLoadFloat3( &fianlPos));
	//_float3 fianlPos = { XMVectorGetX(CurPos), fOut0, XMVectorGetZ(CurPos) };
	_float3 fianlPos0 = { XMVectorGetX(CurPos), fOut0, XMVectorGetZ(CurPos) };

	_float3 Pos1 = { XMVectorGetX(CurPos)+0.5f, XMVectorGetY(CurPos), XMVectorGetZ(CurPos) };
	_float fOut1 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(XMLoadFloat3(&Pos1), &fOut1);
	_float3 fianlPos1 = { Pos1.x, fOut1, Pos1.z };


	_float3 Pos2 = { XMVectorGetX(CurPos), XMVectorGetY(CurPos), XMVectorGetZ(CurPos) + 0.5f };
	_float fOut2 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(XMLoadFloat3(&Pos2), &fOut2);
	_float3 fianlPos2 = { Pos2.x, fOut2, Pos2.z };


	_vector vVecForward = XMLoadFloat3(&fianlPos1) - XMLoadFloat3(&fianlPos0);
	_vector vVecRight = XMLoadFloat3(&fianlPos2) - XMLoadFloat3(&fianlPos1);

	_vector FinalUpDir = XMVector3Normalize(XMVector3Cross(vVecRight,vVecForward));

	_vector vOldShipForward = m_pTransformCom->Get_State(STATE::LOOK);

	_vector FinalRightDir = XMVector3Normalize(XMVector3Cross(FinalUpDir, vOldShipForward));

	_vector FinalLookDir = XMVector3Normalize(XMVector3Cross(FinalRightDir, FinalUpDir));

	_matrix NewRotationMatrix;
	NewRotationMatrix.r[0] = FinalRightDir;
	NewRotationMatrix.r[1] = FinalUpDir;
	NewRotationMatrix.r[2] = FinalLookDir;
	NewRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);


	_vector targetQuat = XMQuaternionRotationMatrix(NewRotationMatrix);
	_vector currentQuat = m_pTransformCom->Get_Quaternion();
	
	_vector smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, 0.1f);

	m_pTransformCom->Set_Quaternion(smoothQuat);

	/*_vector CurPo3 = m_pTransformCom->Get_Position();
	_float fOut3 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/





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
