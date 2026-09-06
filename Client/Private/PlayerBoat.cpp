
#include "PlayerBoat.h"
#include "Collider.h"

#include "Body_Player.h"
#include "GameInstance.h"
#include "Model.h"
#include "DInput_Manager.h"
#include "Inventory.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "PlayerStateMachine.h"
#include "Sea_Manager.h"

CPlayerBoat::CPlayerBoat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CContainerObject{ pDevice ,pContext }, m_pSea_Manager(CSea_Manager::GetInstance())

{
}

CPlayerBoat::CPlayerBoat(const CPlayerBoat& prototype)
	: CContainerObject{ prototype }, m_pSea_Manager(CSea_Manager::GetInstance())

{
}

HRESULT CPlayerBoat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerBoat::Initialize(void* pArg)
{
	//m_pSea_Manager = CSea_Manager::GetInstance();

	PLAYERBOAT_DESC pDesc = {};
	pDesc.fSpeedPerSec = 10.f;
	pDesc.fDegreePerSec = 40.f;

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 0,0 ?먯꽌 ?쒖옉?섎㈃ ?곕젅???덉쇅泥섎━ ?덊빐???곗쭚??
	_float3 tmp = { 5.f, 5.f, 5.f };
	m_pTransformCom->Set_Position(XMLoadFloat3(&tmp));

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	{
		Evt_InvenPlayerInit_Data e = {};
		e.Inven_ptr = m_pInvenCom;

		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_InvenPlayerInit_Data>(e);
	}

	{
		Evt_InvenStrageInit_Data e = {};
		e.Inven_ptr = m_pStorageCom;
		e.inventype = m_pStorageCom->Get_Inventype();
		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_InvenStrageInit_Data>(e);
	}

	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 3.f;
	LightDesc.vDiffuse = _float4(1.0f, 0.8f, 0.5f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_LightObj = (m_pGameInstance.lock()->Add_Light(LightDesc));
	if (m_LightObj == nullptr)
		return E_FAIL;
	m_LightObj->Set_Active(false);

	_float3 pos = _float3(1.987f, 0.f, -0.189f);
	m_pTransformCom->Set_Position(XMLoadFloat3(&pos));
	m_pTransformCom->Set_RotationDegree(_float3{0.f, 90.f,0.f});
	m_pTransformCom->Update_WorldMatrix();





	m_pPlayerStateMachine = CPlayerStateMachine::Create(dynamic_pointer_cast<CPlayerBoat>(shared_from_this()));



	return S_OK;
}

void CPlayerBoat::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayerBoat::Update(_float fTimeDelta)
{
	m_pPlayerStateMachine->Update_StateMachine(fTimeDelta);

	if (m_fInvincibleTime > 0.f)
		m_fInvincibleTime -= fTimeDelta;

	if (m_fInputBlockTime > 0.f)
		m_fInputBlockTime -= fTimeDelta;

	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();
	//?뚯뒪??
	if (dinput->KeyPress(DIK_0))
	{
		Add_Money(m_Money++);
	}

	//m_pTransformCom->Get_WorldMatrix();
	//

	//switch(m_Loacation)
	//{
	//case LOCATIONSTATE::SEA:
	//	Location_Sea(fTimeDelta, dinput);
	//	break;
	//case LOCATIONSTATE::VILLAGE:
	//	Location_Sea(fTimeDelta, dinput);
	//	break;
	//case LOCATIONSTATE::FISHING:
	//	Location_Sea(fTimeDelta, dinput);
	//	break;
	//}
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	
	__super::Update(fTimeDelta);
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayerBoat::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pPlayerStateMachine->LateUpdate_StateMachine(fTimeDelta);


	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CPlayerBoat>(shared_from_this()));
}

HRESULT CPlayerBoat::Render()
{
	int i = 0;
#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;

	
#endif

	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMesh; i++)
	//{
	//	m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
	//	//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}


	return S_OK;
}

void CPlayerBoat::OnGui()
{
}


HRESULT CPlayerBoat::Ready_Components()
{

	

	CInventory::INVEN_DESC inven_desc = {};
	inven_desc.invenType = INVENTYPE::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Inven"), &m_pInvenCom, &inven_desc)))
		return E_FAIL;






	CInventory::INVEN_DESC storage_desc = {};
	storage_desc.invenType = INVENTYPE::STORAGE;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Storage"), &m_pStorageCom, &storage_desc)))
		return E_FAIL;
	

	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};
	OBBDesc.vExtents = _float3(0.5f, 0.6f, 1.5f);
	OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	OBBDesc.MyLayer = COLLISION_LAYER::PLAYER;
	OBBDesc.OtherMask = COLLISION_LAYER::TRIGGER | COLLISION_LAYER::MONSTERATT | COLLISION_LAYER::FISH| COLLISION_LAYER::ITEM;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_pColliderCom, &OBBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);







	return S_OK;
}

HRESULT CPlayerBoat::Ready_PartObjects()
{
	CBody_Player::BodyPlayer_DESC bodyDesc{};
	bodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PartObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"), TEXT("Part_Body"), &bodyDesc)))
		return E_FAIL;



	return S_OK;
}

void CPlayerBoat::Add_Money(_float money)
{
	{
		m_Money += money;

		Evt_AddMoney e = {};
		e.money = m_Money;
		m_pGameInstance.lock()->Get_EventBus()->Publish(e);
	}
	m_pGameInstance.lock()->Play_Once(L"Money_Gained");
}

void CPlayerBoat::SetHPFull()
{
	{
        m_Hp = m_MAXHp;
        Evt_RepairCoat e = {};
        e.cost = Get_DemageFixPrice();
        m_pGameInstance.lock()->Get_EventBus()->Publish(e);
    };
	
}

bool CPlayerBoat::MinusMoney(int amount)
{
	  {    if (m_Money < amount)
		return false;

	m_Money -= amount;

	Evt_AddMoney e = {};
	e.money = m_Money;
	m_pGameInstance.lock()->Get_EventBus()->Publish(e);

	  m_pGameInstance.lock()->Play_Once(L"Money_Spent");
	return true;
	}
}

void CPlayerBoat::Set_ShipStats(_uint boatSpeed, _uint fishingSpeed, _uint light, SEA_MASK seaMask, _float InvenMoney)
{


int i = {};

	Evt_ShipStat e = {};
	e.EngineSpeed = m_BoatSpeed = boatSpeed;
	e.FishingSpeed = m_RodSpeed = fishingSpeed;
	e.LightIntensity = m_Light = light;
	e.SeaMask = m_SeaMask = seaMask;

	e.InvenMoney = m_InvenMoney;

	m_pGameInstance.lock()->Get_EventBus()->Publish(e);

}

void CPlayerBoat::Get_Demage(_uint Demage)
{
	if (m_fInvincibleTime > 0.f)
		return;

	
	m_fInvincibleTime = 2.f;
	

	m_Hp -= Demage;

	// 梨꾨젰 ?섎굹 以꾧퀬
	if(m_Hp <= 0)
	{
	//留뚯씪 梨꾨젰??0 ?대㈃ 二쎌쓬 ?곹깭
		Dead();
		m_WasDead = true;
	}


	int ran = m_pGameInstance.lock()->RandomInt( 3, 5);
	wstring name = L"impact_" + to_wstring(ran);
	m_pGameInstance.lock()->Play_Once(name);



	// 移대찓???먯씠??-> ?곕?吏 以履쎌뿉??
	Evt_Demage e = {};
	e.ShakePower = 0.3f;
	e.ShakeTime = 1.f;
	//e.DeAc = 0.1f;
	e.DemageCount = m_MAXHp - m_Hp;
	m_pGameInstance.lock()->Get_EventBus()->Publish(e);
	// ?몃객 ??移??뚮? ??젣


	Evt_RepairCoat e1 = {};
	e1.cost = Get_DemageFixPrice();
	m_pGameInstance.lock()->Get_EventBus()->Publish(e1);

}

void CPlayerBoat::Location_Sea(_float fTimeDelta, CDInput_Manager* dinput)
{
	_float4 upDir = { 0.f, 1.f, 0.f, 0.f };

	if (dinput->KeyPress(DIK_W))
	{
		m_pTransformCom->Go_Forward(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (dinput->KeyPress(DIK_D))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), fTimeDelta);
	}

	if (dinput->KeyPress(DIK_A))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), -fTimeDelta);
	}
	auto Sea = m_pSea_Manager.lock();

	// ?꾩떆肄붾뱶 ///////////////// 3??-> 4?먯쑝濡??섏젙?덉젙 + 肄붾뱶 ?뺣━
	//
	_vector CurPos = m_pTransformCom->Get_Position();

	//以묒젏?쇰줈 y ?꾩튂
	//{
	_float fFinalPosY = {};

	//m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fFinalPosY);
	_float3 fianlPos = { XMVectorGetX(CurPos), Sea->Get_GlobalY(), XMVectorGetZ(CurPos) };
	m_pTransformCom->Set_Position(XMLoadFloat3(&fianlPos));
	//	
	//}


	// 4???쇰줈 湲곗슱湲?+ 蹂닿컙
	{
		//_float3 FRBL[4] = { { 0.f,  0.f,1.f }, { 0.5f,0.f,0.f} , { 0.f, 0.f,-1.f }, { -0.5f, 0.f, 0.f, } };
		_float3 FRBL[4] = {};
		XMStoreFloat3(&FRBL[0], XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.5f);
		XMStoreFloat3(&FRBL[1], XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * 0.5f);
		XMStoreFloat3(&FRBL[2], XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * -1.5f);
		XMStoreFloat3(&FRBL[3], XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * -0.5f);

		_float3 Pos[4];
		Pos[0] = fianlPos + FRBL[0];
		Pos[1] = fianlPos + FRBL[1];
		Pos[2] = fianlPos + FRBL[2];
		Pos[3] = fianlPos + FRBL[3];

		_float3 fianlPosFRBL[4];
		for (_uint i = 0; i < 4; i++)
		{

			fianlPosFRBL[i] = Sea->Calculate_GerstnerWavePosition(Pos[i]);
		}

		_vector forwordDir = XMLoadFloat3(&fianlPosFRBL[0]) - XMLoadFloat3(&fianlPosFRBL[2]);
		_vector RightDir = XMLoadFloat3(&fianlPosFRBL[1]) - XMLoadFloat3(&fianlPosFRBL[3]);

		forwordDir = XMVector3Normalize(forwordDir);
		RightDir = XMVector3Normalize(RightDir);

		_vector FinalUpDir = XMVector3Normalize(XMVector3Cross(forwordDir, RightDir));

		_vector vOldShipForward = m_pTransformCom->Get_State(STATE::LOOK);

		_vector FinalRightDir = XMVector3Normalize(XMVector3Cross(FinalUpDir, vOldShipForward));

		_vector FinalLookDir = XMVector3Normalize(XMVector3Cross(FinalRightDir, FinalUpDir));

		_matrix NewRotationMatrix;
		NewRotationMatrix.r[0] = FinalRightDir;
		NewRotationMatrix.r[1] = FinalUpDir;
		NewRotationMatrix.r[2] = FinalLookDir;
		NewRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

		_vector targetQuat = XMQuaternionRotationMatrix(NewRotationMatrix);
		targetQuat = XMQuaternionNormalize(targetQuat);
		_vector currentQuat = m_pTransformCom->Get_Quaternion();

		_vector smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, 4.f * fTimeDelta);

		m_pTransformCom->Set_Quaternion(smoothQuat);
	}


	/*_vector CurPo3 = m_pTransformCom->Get_Position();
	_float fOut3 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/

	m_pTransformCom->Update_WorldMatrix();


	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CPlayerBoat::Dead()
{
	SetHPFull();
	m_fInputBlockTime = 2.f;
	_float3 pos = _float3(0.f, 5.f, 0.f);
	m_pTransformCom->Set_Position(XMLoadFloat3(&pos));

}

void CPlayerBoat::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if(m_pPlayerStateMachine)
	m_pPlayerStateMachine->OnBeginOverlap(self, other);
	//CContainerObject::OnBeginOverlap(self, other);
}

void CPlayerBoat::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if (m_pPlayerStateMachine)
		m_pPlayerStateMachine->OnEndOverlap(self, other);


	//CContainerObject::OnEndOverlap(self, other);
}

void CPlayerBoat::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if (m_pPlayerStateMachine)
		m_pPlayerStateMachine->OnStayOverlap(self, other);


	//CContainerObject::OnStayOverlap(self, other);
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
