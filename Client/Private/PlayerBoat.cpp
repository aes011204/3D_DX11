
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

CPlayerBoat::CPlayerBoat(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CContainerObject{ device ,context }, m_seaManager(CSea_Manager::GetInstance())

{
}

CPlayerBoat::CPlayerBoat(const CPlayerBoat& prototype)
	: CContainerObject{ prototype }, m_seaManager(CSea_Manager::GetInstance())

{
}

HRESULT CPlayerBoat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerBoat::Initialize(void* arg)
{
	//m_pSea_Manager = CSea_Manager::GetInstance();

	PlayerBoatDesc desc = {};
	desc.speedPerSec = 10.f;
	desc.degreePerSec = 40.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 0,0 ?먯꽌 ?쒖옉?섎㈃ ?곕젅???덉쇅泥섎━ ?덊빐???곗쭚??
	_float3 tmp = { 5.f, 5.f, 5.f };
	m_transformCom->Set_Position(XMLoadFloat3(&tmp));

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	{
		Evt_InvenPlayerInitData e = {};
		e.invenPtr = m_invenCom;

		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_InvenPlayerInitData>(e);
	}

	{
		Evt_InvenStorageInitData e = {};
		e.invenPtr = m_storageCom;
		e.invenType = m_storageCom->Get_Inventype();
		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_InvenStorageInitData>(e);
	}

	LightDesc lightDesc = {};

	lightDesc.type = ELightType::POINT;
	lightDesc.position = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.range = 3.f;
	lightDesc.diffuse = _float4(1.0f, 0.8f, 0.5f, 1.f);
	lightDesc.ambient = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);

	m_lightObj = (m_gameInstance.lock()->Add_Light(lightDesc));
	if (m_lightObj == nullptr)
		return E_FAIL;
	m_lightObj->Set_Active(false);

	_float3 pos = _float3(1.987f, 0.f, -0.189f);
	m_transformCom->Set_Position(XMLoadFloat3(&pos));
	m_transformCom->Set_RotationDegree(_float3{0.f, 90.f,0.f});
	m_transformCom->Update_WorldMatrix();





	m_playerStateMachine = CPlayerStateMachine::Create(dynamic_pointer_cast<CPlayerBoat>(shared_from_this()));



	return S_OK;
}

void CPlayerBoat::Priority_Update(_float timeDelta)
{
	__super::Priority_Update(timeDelta);
}

void CPlayerBoat::Update(_float timeDelta)
{
	m_playerStateMachine->Update_StateMachine(timeDelta);

	if (m_invincibleTime > 0.f)
		m_invincibleTime -= timeDelta;

	if (m_inputBlockTime > 0.f)
		m_inputBlockTime -= timeDelta;

	CDInput_Manager* dinput = m_gameInstance.lock()->Get_DInput_Manger();
	//?뚯뒪??
	if (dinput->KeyPress(DIK_0))
	{
		Add_Money(m_money++);
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
	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));

	
	__super::Update(timeDelta);
	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayerBoat::Late_Update(_float timeDelta)
{
	__super::Late_Update(timeDelta);
	m_playerStateMachine->LateUpdate_StateMachine(timeDelta);


	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CPlayerBoat>(shared_from_this()));
}

HRESULT CPlayerBoat::Render()
{
	int i = 0;
#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_IsDebug() == false)
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

	

	CInventory::InvenDesc invenDesc = {};
	invenDesc.invenType = EInvenType::PLAYER;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Inven"), &m_invenCom, &invenDesc)))
		return E_FAIL;






	CInventory::InvenDesc storageDesc = {};
	storageDesc.invenType = EInvenType::STORAGE;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Storage"), &m_storageCom, &storageDesc)))
		return E_FAIL;
	

	CBounding_OBB::BoundingOBBDesc		obbDesc{};
	obbDesc.extents = _float3(0.5f, 0.6f, 1.5f);
	obbDesc.radians = _float3(0.f, 0.f, 0.f);
	obbDesc.center = _float3(0.f, 0.f, 0.f);
	obbDesc.myLayer = ECollisionLayer::PLAYER;
	obbDesc.otherMask = ECollisionLayer::TRIGGER | ECollisionLayer::MONSTERATT | ECollisionLayer::FISH| ECollisionLayer::ITEM;
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_colliderCom, &obbDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_colliderCom);







	return S_OK;
}

HRESULT CPlayerBoat::Ready_PartObjects()
{
	CBody_Player::BodyPlayerDesc bodyDesc{};
	bodyDesc.parentMatrix = m_transformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PartObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"), TEXT("Part_Body"), &bodyDesc)))
		return E_FAIL;



	return S_OK;
}

void CPlayerBoat::Add_Money(_float money)
{
	{
		m_money += money;

		Evt_AddMoney e = {};
		e.money = m_money;
		m_gameInstance.lock()->Get_EventBus()->Publish(e);
	}
	m_gameInstance.lock()->Play_Once(L"Money_Gained");
}

void CPlayerBoat::SetHPFull()
{
	{
        m_hp = m_maxHp;
        Evt_RepairCost e = {};
        e.cost = Get_DamageFixPrice();
        m_gameInstance.lock()->Get_EventBus()->Publish(e);
    };
	
}

bool CPlayerBoat::MinusMoney(int amount)
{
	  {    if (m_money < amount)
		return false;

	m_money -= amount;

	Evt_AddMoney e = {};
	e.money = m_money;
	m_gameInstance.lock()->Get_EventBus()->Publish(e);

	  m_gameInstance.lock()->Play_Once(L"Money_Spent");
	return true;
	}
}

void CPlayerBoat::Set_ShipStats(_uint boatSpeed, _uint fishingSpeed, _uint light, SeaMask seaMask, _float invenMoney)
{


int i = {};

	Evt_ShipStat e = {};
	e.engineSpeed = m_boatSpeed = boatSpeed;
	e.fishingSpeed = m_rodSpeed = fishingSpeed;
	e.lightIntensity = m_light = light;
	e.seaMask = m_seaMask = seaMask;

	e.invenMoney = m_invenMoney;

	m_gameInstance.lock()->Get_EventBus()->Publish(e);

}

void CPlayerBoat::Get_Damage(_uint damage)
{
	if (m_invincibleTime > 0.f)
		return;

	
	m_invincibleTime = 2.f;
	

	m_hp -= damage;

	// 梨꾨젰 ?섎굹 以꾧퀬
	if(m_hp <= 0)
	{
	//留뚯씪 梨꾨젰??0 ?대㈃ 二쎌쓬 ?곹깭
		Dead();
		m_wasDead = true;
	}


	int ran = m_gameInstance.lock()->RandomInt( 3, 5);
	wstring name = L"impact_" + to_wstring(ran);
	m_gameInstance.lock()->Play_Once(name);



	// 移대찓???먯씠??-> ?곕?吏 以履쎌뿉??
	Evt_Damage e = {};
	e.shakePower = 0.3f;
	e.shakeTime = 1.f;
	//e.DeAc = 0.1f;
	e.damageCount = m_maxHp - m_hp;
	m_gameInstance.lock()->Get_EventBus()->Publish(e);
	// ?몃객 ??移??뚮? ??젣


	Evt_RepairCost e1 = {};
	e1.cost = Get_DamageFixPrice();
	m_gameInstance.lock()->Get_EventBus()->Publish(e1);

}

void CPlayerBoat::Location_Sea(_float timeDelta, CDInput_Manager* dinput)
{
	_float4 upDir = { 0.f, 1.f, 0.f, 0.f };

	if (dinput->KeyPress(DIK_W))
	{
		m_transformCom->Go_Forward(timeDelta);
	}

	if (dinput->KeyPress(DIK_S))
	{
		m_transformCom->Go_Backward(timeDelta);
	}

	if (dinput->KeyPress(DIK_D))
	{
		m_transformCom->Turn(XMLoadFloat4(&upDir), timeDelta);
	}

	if (dinput->KeyPress(DIK_A))
	{
		m_transformCom->Turn(XMLoadFloat4(&upDir), -timeDelta);
	}
	auto sea = m_seaManager.lock();

	// ?꾩떆肄붾뱶 ///////////////// 3??-> 4?먯쑝濡??섏젙?덉젙 + 肄붾뱶 ?뺣━
	//
	_vector curPos = m_transformCom->Get_Position();

	//以묒젏?쇰줈 y ?꾩튂
	//{
	_float finalPosY = {};

	//m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fFinalPosY);
	_float3 finalPos = { XMVectorGetX(curPos), sea->Get_GlobalY(), XMVectorGetZ(curPos) };
	finalPos.y = sea->Calculate_GerstnerWavePosition(finalPos).y;
	m_transformCom->Set_Position(XMLoadFloat3(&finalPos));
	//	
	//}


	// 4???쇰줈 湲곗슱湲?+ 蹂닿컙
	{
		//_float3 FRBL[4] = { { 0.f,  0.f,1.f }, { 0.5f,0.f,0.f} , { 0.f, 0.f,-1.f }, { -0.5f, 0.f, 0.f, } };
		_float3 frbl[4] = {};
		_vector flatLook = XMVectorSetY(m_transformCom->Get_State(ETransformState::LOOK), 0.f);
		flatLook = XMVector3Normalize(flatLook);
		const _vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector flatRight = XMVector3Normalize(XMVector3Cross(worldUp, flatLook));

		XMStoreFloat3(&frbl[0], flatLook * 1.5f);
		XMStoreFloat3(&frbl[1], flatRight * 0.5f);
		XMStoreFloat3(&frbl[2], flatLook * -1.5f);
		XMStoreFloat3(&frbl[3], flatRight * -0.5f);

		_float3 pos[4];
		pos[0] = finalPos + frbl[0];
		pos[1] = finalPos + frbl[1];
		pos[2] = finalPos + frbl[2];
		pos[3] = finalPos + frbl[3];

		_float3 finalPosFrbl[4];
		for (_uint i = 0; i < 4; i++)
		{

			finalPosFrbl[i] = sea->Calculate_GerstnerWavePosition(pos[i]);
		}

		_vector forwardDir = XMLoadFloat3(&finalPosFrbl[0]) - XMLoadFloat3(&finalPosFrbl[2]);
		_vector rightDir = XMLoadFloat3(&finalPosFrbl[1]) - XMLoadFloat3(&finalPosFrbl[3]);

		forwardDir = XMVector3Normalize(forwardDir);
		rightDir = XMVector3Normalize(rightDir);

		_vector finalUpDir = XMVector3Normalize(XMVector3Cross(forwardDir, rightDir));

		_vector finalRightDir = XMVector3Normalize(XMVector3Cross(finalUpDir, flatLook));

		_vector finalLookDir = XMVector3Normalize(XMVector3Cross(finalRightDir, finalUpDir));

		_matrix newRotationMatrix;
		newRotationMatrix.r[0] = finalRightDir;
		newRotationMatrix.r[1] = finalUpDir;
		newRotationMatrix.r[2] = finalLookDir;
		newRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

		_vector targetQuat = XMQuaternionRotationMatrix(newRotationMatrix);
		targetQuat = XMQuaternionNormalize(targetQuat);
		_vector currentQuat = m_transformCom->Get_Quaternion();

		_vector smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, 4.f * timeDelta);

		m_transformCom->Set_Quaternion(smoothQuat);
	}


	/*_vector CurPo3 = m_pTransformCom->Get_Position();
	_float fOut3 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/

	m_transformCom->Update_WorldMatrix();


	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));

}

void CPlayerBoat::Dead()
{
	SetHPFull();
	m_inputBlockTime = 2.f;
	_float3 pos = _float3(0.f, 5.f, 0.f);
	m_transformCom->Set_Position(XMLoadFloat3(&pos));

}

void CPlayerBoat::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if(m_playerStateMachine)
	m_playerStateMachine->OnBeginOverlap(self, other);
	//CContainerObject::OnBeginOverlap(self, other);
}

void CPlayerBoat::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if (m_playerStateMachine)
		m_playerStateMachine->OnEndOverlap(self, other);


	//CContainerObject::OnEndOverlap(self, other);
}

void CPlayerBoat::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if (m_playerStateMachine)
		m_playerStateMachine->OnStayOverlap(self, other);


	//CContainerObject::OnStayOverlap(self, other);
}

shared_ptr<CPlayerBoat> CPlayerBoat::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CPlayerBoat> instance(new CPlayerBoat(device, context), [](CPlayerBoat* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerBoat");
	}
	return instance;
}


shared_ptr<CGameObject> CPlayerBoat::Clone(void* arg)
{
	shared_ptr<CPlayerBoat> instance(new CPlayerBoat(*this), [](CPlayerBoat* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerBoat");
	}
	return instance;
}

void CPlayerBoat::Free()
{
	__super::Free();
}
