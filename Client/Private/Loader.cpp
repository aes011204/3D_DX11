#include "Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "Monster.h"
#include "Texture.h"
#include "PlayerBoat.h"
#include "Assimp_Model.h"
#include "Body_Player.h"
#include "Monster_Anim.h"
#include "Sea.h"
#include "Collider.h"
#include "Explosion.h"
#include "Fish.h"
#include "Mon_MonkFish.h"
#include "Mon_R.h"
#include "Mon_R_Act.h"
#include "Mon_Tentacle.h"
#include "VIBuffer_Sea.h"
#include "VIBuffer_Particle_Point.h"
#include "Snow.h"
#include "VIBuffer_Particle_Rect.h"
#include "Village.h"
#include "Box.h"
#include "ColliderObject.h"
#include "WaterEff.h"

CLoader::CLoader(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device(device), m_context(context),
	m_gameInstance(CGameInstance::GetInstance())
{

}

unsigned int APIENTRY ThreadMain(void* arg)
{
	// pArg로 pLoader 주소를 받아옴 근데  void* 형이라서 형변환후 사용 가능
	CLoader* loader = static_cast<CLoader*>(arg);
	if (nullptr == loader)
		return 1;

	// 쓰레드를 이용해서 로딩 클래스에 있는 로딩 함수를 돌린다
	if (FAILED(loader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(ELevel nextLevelID)
{
	// 실제 자원을 로드해주기 위한 스레드를 형성한다
	m_nextLevelID = nextLevelID;

	InitializeCriticalSection(&m_criticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	if(0 == m_hThread)
	{
		MSG_BOX("Failed to Created : m_hThread");
		return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CLoader::Loading()
{
	// 다른 쓰레드가 공유하는 메모리영역(임계영역) 접근시 막아주는(대기로 바꿔주는) 함수 (락)
	EnterCriticalSection(&m_criticalSection);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	HRESULT hr = {};

	switch (m_nextLevelID)
	{
	case ELevel::LOGO:
		hr = Loading_For_LogoLevel();
		m_isFinished = true;
		break;
	case ELevel::GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		m_isFinished = true;
		break;
	case ELevel::EDITOR:
		// 에디터는 스태틱 제외 모든 레벨을 로딩을 돌리고 들어가야함
		//hr = Loading_For_EditorLevel();
		if (Loading_For_GamePlayLevel() == S_OK && Loading_For_LogoLevel() == S_OK)
		{
			hr = S_OK;
			m_isFinished = true;
		}
		else
			hr = E_FAIL;
		break;
	default:
		hr = E_FAIL;
	}

	CoUninitialize();

	LeaveCriticalSection(&m_criticalSection);// 언락 - 이제 다른쓰레드도 접근가능

	return hr;
}

HRESULT CLoader::Loading_For_LogoLevel()
{

	lstrcpy(m_loadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_device, m_context, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	lstrcpy(m_loadingText, TEXT("셰이더를 로딩 중 입니다."));
	
	lstrcpy(m_loadingText, TEXT("사운드를 로딩 중 입니다."));

	lstrcpy(m_loadingText, TEXT("모델를 로딩 중 입니다."));
	
	lstrcpy(m_loadingText, TEXT("객체원형를 로딩 중 입니다."));
	/* Prototype_GameObject_BackGround */
	if(FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : CBackGround");
		return E_FAIL;
	}

	lstrcpy(m_loadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;



	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_loadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	///* Prototype_Component_Texture_Terrain */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_RGB.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : BackGround Texture");
	//	return E_FAIL;
	//}

	///* Prototype_Component_Texture_Snow*/
	//
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Snow"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Snow Texture");
	//	return E_FAIL;
	//}
	


	lstrcpy(m_loadingText, TEXT("셰이더를 로딩 중 입니다."));

	

	/* Prototype_Component_Shader_VtxParticleRect */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticleRect"),
		CShader::Create(m_device, m_context, TEXT("../Bin/ShaderFiles/Shader_VtxParticleRect.hlsl"), VTXPARTICLE_RECTINSTANCE_DESC::Elements, VTXPARTICLE_RECTINSTANCE_DESC::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxCube");
		return E_FAIL;
	}
	


	/* Prototype_Component_Shader_VtxParticlePoint */

	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticlePoint"),
		CShader::Create(m_device, m_context, TEXT("../Bin/ShaderFiles/Shader_VtxParticlePoint.hlsl"), VTXPARTICLE_POINTINSTANCE_DESC::Elements, VTXPARTICLE_POINTINSTANCE_DESC::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxParticlePoint");
		return E_FAIL;
	}

	lstrcpy(m_loadingText, TEXT("사운드를 로딩 중 입니다."));


	lstrcpy(m_loadingText, TEXT("모델를 로딩 중 입니다."));
	///* Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_1500.raw")))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : VIBuffer_Terrain");
	//	return E_FAIL;
	//}
	CVIBuffer_Particle_Rect::ParticleRectDesc		snowDesc{};
	snowDesc.numInstances = 5000;
	snowDesc.center = _float3(0.f, 0.f, 0.f);
	snowDesc.range = _float3(129.f, 1.f, 129.f);
	snowDesc.scale = _float2(0.2f, 0.5f);
	snowDesc.speed = _float2(9.0f, 11.0f);
	snowDesc.lifeTime = _float2(.5f, 4.0f);
	snowDesc.isLoop = true;
	/* Prototype_Component_VIBuffer_Particle_Rect_Snow */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Rect_Snow"),
		CVIBuffer_Particle_Rect::Create(m_device, m_context, &snowDesc))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
		return E_FAIL;
	}



	CVIBuffer_Particle_Point::ParticlePointDesc		exploDesc{};
	exploDesc.numInstances = 500;
	exploDesc.center = _float3(0.f, 0.f, 0.f);
	exploDesc.range = _float3(0.3f, 0.3f, 0.3f);
	exploDesc.scale = _float2(0.1f, 0.2f);
	exploDesc.speed = _float2(3.0f, 7.0f);
	exploDesc.lifeTime = _float2(0.3f, .6f);
	exploDesc.pivot = _float3(0.f, 0.f, 0.f);
	exploDesc.isLoop = true;


	/* Prototype_Component_VIBuffer_Particle_Point_Explosion */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_Explosion"),
		CVIBuffer_Particle_Point::Create(m_device, m_context, &exploDesc))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
		return E_FAIL;
	}

	CVIBuffer_Particle_Point::ParticlePointDesc		waterDesc{};
	waterDesc.numInstances = 10;
	waterDesc.center = _float3(0.f, 0.f, 0.f);
	waterDesc.range = _float3(1.5f, 0.f, 1.5f);
	waterDesc.sizeRange = _float2(0.3f, 0.6f);
	waterDesc.scale = _float2(0.1f, 0.2f);
	waterDesc.angle = _float3(0.f,0.f,0.0f);
	waterDesc.speed = _float2(3.0f, 7.0f);
	waterDesc.lifeTime = _float2(0.2f, 1.0f);
	waterDesc.pivot = _float3(0.f, 0.f, 0.f);
	waterDesc.isLoop = true;


	/* Prototype_Component_VIBuffer_Particle_Point_Explosion */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_WaterEff"),
		CVIBuffer_Particle_Point::Create(m_device, m_context, &waterDesc))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
		return E_FAIL;
	}



	//CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		WaterDescF{};
	//WaterDescF.iNumInstances = 15;
	//WaterDescF.vCenter = _float3(0.f, 0.f, 0.f);
	//WaterDescF.vRange = _float3(2.f, 0.f, 2.f);
	//WaterDescF.vSizeRange = _float2(0.3f, 0.6f);
	//WaterDescF.vScale = _float2(0.1f, 0.2f);
	//WaterDescF.Angle = _float3(0.f, 0.f, 0.0f);
	//WaterDescF.vSpeed = _float2(3.0f, 7.0f);
	//WaterDescF.vLifeTime = _float2(0.2f, 1.0f);
	//WaterDescF.vPivot = _float3(0.f, 0.2f, 0.f);
	//WaterDescF.isLoop = true;
	//
	//
	///* Prototype_Component_VIBuffer_Particle_Point_Explosion */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_WaterEff"),
	//	CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &WaterDescF))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
	//	return E_FAIL;
	//}



	_matrix preLocalTransformMatrix = { XMMatrixIdentity() };


	/* Prototype_Component_Model_FullBoatCrab */
	preLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_FullBoatCrab"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/FullBoatCrab/FullBoatCrab.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Tentacle */
	preLocalTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Tentacle"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Tentacle/Tentacle_Tex.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_R_Act */
	preLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(45.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/R/R.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}

	/* Prototype_Component_Model_R_Act_One */
	preLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_One"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/R_Act/R_Act_oNE.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_R_Act_Etc */
	preLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 5.f, 0.f);
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_Etc"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/R_Act/Rffffffff_Act_Fix.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}



	/* Prototype_Component_Model_Marrow_Mon */
 	preLocalTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) *XMMatrixRotationZ(XMConvertToRadians(180.f))*XMMatrixRotationX(XMConvertToRadians(-90.f)) *XMMatrixRotationY(XMConvertToRadians(-30.f)) ;
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Mon"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Marrow_Mon/Marrow_Mon_Tex.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Marrow_Boat */
	preLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Boat"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Marrow_Mon/Marrow_Boat_Tex.dat", EModelType::NONANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}

	/* Prototype_Component_Model_HalfBoat */
	preLocalTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_HalfBoat"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/HalfBoat/HalfBoat_Tex.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}









	preLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/*Prototype_Component_Model_Fiona_Anim*/
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona_Anim"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Fiona/Fiona.dat", EModelType::ANIM, preLocalTransformMatrix))))
	{ 
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	///* Prototype_Component_Model_ForkLift */
	//PreLocalTransformMatrix = XMMatrixScaling(0.01f,0.01f,0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/ForkLift/ForkLift.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Model_ForkLift");
	//	return E_FAIL;
	//}

	/* Prototype_Component_Model_PlayerBoat */
	preLocalTransformMatrix = XMMatrixScaling(0.008f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_PlayerBoat"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Boat/PlayerBoat4.dat", EModelType::NONANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : PlayerBoat");
		return E_FAIL;
	}

	/* Prototype_Component_Model_Town */
	preLocalTransformMatrix =  XMMatrixScaling(0.01f,0.01f,0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Town"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/GM_TOWN/GM_Town_tx.dat", EModelType::NONANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	

	/* Prototype_Component_Model_Fish */
	preLocalTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Fish"),
		CModel::Create(m_device, m_context, "../Bin/Resources/BinaryModels/Fish/Fish.dat", EModelType::NONANIM, preLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Fish");
		return E_FAIL;
	}

	lstrcpy(m_loadingText, TEXT("객체원형를 로딩 중 입니다."));////////////////////////////////
	///* Prototype_GameObject_Terrain */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//{
	//	MSG_BOX("Faild to Add_Prototype :GameObject_Terrain");
	//	return E_FAIL;
	//}

	

	/* Prototype_GameObject_Monster */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Monster");
		return E_FAIL;
	}


	/* Prototype_GameObject_Monster_Anim */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Anim"),
		CMonster_Anim::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Monster_Anim");
		return E_FAIL;
	}

	/* Prototype_GameObject_PlayerBoat */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_PlayerBoat"),
		CPlayerBoat::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : PlayerBoat");
		return E_FAIL;
	}

	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : Body_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Village */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Village"),
		CVillage::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : Villsge");
		return E_FAIL;
	}

	/* Prototype_GameObject_Fish */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		CFish::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : fish");
		return E_FAIL;
	}






	/* Prototype_GameObject_R */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_R"),
		CMon_R::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}

	/* Prototype_GameObject_R_Act */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_R_Act"),
		CMon_R_Act::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}

	/* Prototype_GameObject_Tentacle */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Tentacle"),
		CMon_Tentacle::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}



	/* Prototype_GameObject_MonkFish */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_MonkFish"),
		CMon_MonkFish::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}





	/* Prototype_GameObject_Box */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Box"),
		CBox::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}

	/* Prototype_GameObject_ColliderObject */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_ColliderObject"),
		CColliderObject::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}









	/* Prototype_GameObject_WaterEff */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_WaterEff"),
		CWaterEff::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_WaterEff");
		return E_FAIL;
	}

	/* Prototype_GameObject_Snow */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		CSnow::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Snow");
		return E_FAIL;
	}
	/* Prototype_GameObject_Explosion */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_device, m_context))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Explosion");
		return E_FAIL;
	}

	lstrcpy(m_loadingText, TEXT("충돌체를 로딩 중 입니다."));
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_device, m_context, EColliderType::AABB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_AABB");
		return E_FAIL;
	}
	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_device, m_context, EColliderType::OBB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_OBB");
		return E_FAIL;
	}
	/* Prototype_Component_Collider_Sphere*/
	if (FAILED(m_gameInstance.lock()->Add_Prototype(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_device, m_context, EColliderType::SPHERE))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_Sphere");
		return E_FAIL;
	}



	lstrcpy(m_loadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;

	return S_OK;

}
HRESULT CLoader::Loading_For_EditorLevel()
{
	lstrcpy(m_loadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;

	return S_OK;
}

#ifdef _DEBUG
void CLoader::Print_LoadText()
{
	SetWindowText(g_hWnd, m_loadingText);
}
#endif

shared_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, ELevel nextLevelID)
{
	shared_ptr<CLoader> instance ( new CLoader(device, context));

	if (FAILED(instance->Initialize(nextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
	}
	return instance;
}

void CLoader::Free()
{
	__super::Free();

	//쓰레드 사용하고 있음 갑자기 끝나면 무한루프로 쓰레드 끝날떄까지 기다리고 다 끝나면 지운다
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_criticalSection);
}
