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

CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{

}

unsigned int APIENTRY ThreadMain(void* pArg)
{
	// pArg로 pLoader 주소를 받아옴 근데  void* 형이라서 형변환후 사용 가능
	CLoader* pLoader = static_cast<CLoader*>(pArg);
	if (nullptr == pLoader)
		return 1;

	// 쓰레드를 이용해서 로딩 클래스에 있는 로딩 함수를 돌린다
	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	// 실제 자원을 로드해주기 위한 스레드를 형성한다
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

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
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	HRESULT hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_LogoLevel();
		m_bFinished = true;
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		m_bFinished = true;
		break;
	case LEVEL::EDITOR:
		// 에디터는 스태틱 제외 모든 레벨을 로딩을 돌리고 들어가야함
		//hr = Loading_For_EditorLevel();
		if (Loading_For_GamePlayLevel() == S_OK && Loading_For_LogoLevel() == S_OK)
		{
			hr = S_OK;
			m_bFinished = true;
		}
		else
			hr = E_FAIL;
		break;
	default:
		hr = E_FAIL;
	}

	CoUninitialize();

	LeaveCriticalSection(&m_CriticalSection);// 언락 - 이제 다른쓰레드도 접근가능

	return hr;
}

HRESULT CLoader::Loading_For_LogoLevel()
{

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));
	
	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));
	
	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	/* Prototype_GameObject_BackGround */
	if(FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : CBackGround");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_RGB.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Snow*/

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Snow Texture");
		return E_FAIL;
	}
	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : BackGround Texture");
	//	return E_FAIL;
	//}


	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));

	// 이거 전부 main app 에

	///* Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
	//	return E_FAIL;
	//}

	///* Prototype_Component_Shader_VtxMesh */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
	//	return E_FAIL;
	//}

	///* Prototype_Component_Shader_VtxAnimMesh */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
	//	return E_FAIL;
	//}
	/* Prototype_Component_Shader_VtxParticleRect */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticleRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleRect.hlsl"), VTXPARTICLE_RECTINSTANCE_DESC::Elements, VTXPARTICLE_RECTINSTANCE_DESC::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxCube");
		return E_FAIL;
	}
	


	/* Prototype_Component_Shader_VtxParticlePoint */

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticlePoint"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticlePoint.hlsl"), VTXPARTICLE_POINTINSTANCE_DESC::Elements, VTXPARTICLE_POINTINSTANCE_DESC::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxParticlePoint");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_1500.raw")))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Terrain");
		return E_FAIL;
	}
	CVIBuffer_Particle_Rect::PARTICLE_RECT_DESC		SnowDesc{};
	SnowDesc.iNumInstances = 5000;
	SnowDesc.vCenter = _float3(0.f, 0.f, 0.f);
	SnowDesc.vRange = _float3(129.f, 1.f, 129.f);
	SnowDesc.vScale = _float2(0.2f, 0.5f);
	SnowDesc.vSpeed = _float2(3.0f, 7.0f);
	SnowDesc.vLifeTime = _float2(3.f, 5.0f);
	SnowDesc.isLoop = true;
	/* Prototype_Component_VIBuffer_Particle_Rect_Snow */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Rect_Snow"),
		CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &SnowDesc))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
		return E_FAIL;
	}



	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC		ExploDesc{};
	ExploDesc.iNumInstances = 500;
	ExploDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ExploDesc.vRange = _float3(0.3f, 0.3f, 0.3f);
	ExploDesc.vScale = _float2(0.1f, 0.2f);
	ExploDesc.vSpeed = _float2(3.0f, 7.0f);
	ExploDesc.vLifeTime = _float2(1.f, 2.0f);
	ExploDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ExploDesc.isLoop = false;


	/* Prototype_Component_VIBuffer_Particle_Point_Explosion */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_Explosion"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ExploDesc))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Particle_Point");
		return E_FAIL;
	}

	///* Prototype_Component_VIBuffer_Sea */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Sea"),
	//	CVIBuffer_Sea::Create(m_pDevice, m_pContext,128,4))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : VIBuffer_Sea");
	//	return E_FAIL;
	//}


	_matrix PreLocalTransformMatrix = { XMMatrixIdentity() };


	/* Prototype_Component_Model_FullBoatCrab */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FullBoatCrab"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/FullBoatCrab/FullBoatCrab.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Tentacle */
	PreLocalTransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Tentacle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Tentacle/Tentacle_Tex.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_R_Act */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(45.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/R/R.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}

	/* Prototype_Component_Model_R_Act_One */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_One"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/R_Act/R_Act_oNE.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_R_Act_Etc */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 15.f, 0.f);
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_Etc"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/R_Act/Rffffffff_Act_Fix.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}



	/* Prototype_Component_Model_Marrow_Mon */
 	PreLocalTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) *XMMatrixRotationZ(XMConvertToRadians(180.f))*XMMatrixRotationX(XMConvertToRadians(-90.f)) *XMMatrixRotationY(XMConvertToRadians(-30.f)) ;
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Mon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Marrow_Mon/Marrow_Mon_Tex.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Marrow_Boat */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Boat"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Marrow_Mon/Marrow_Boat_Tex.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : FullBoatCrab");
		return E_FAIL;
	}











	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/*Prototype_Component_Model_Fiona_Anim*/
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona_Anim"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Fiona/Fiona.dat", MODEL::ANIM, PreLocalTransformMatrix))))
	{ 
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f,0.01f,0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/ForkLift/ForkLift.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_ForkLift");
		return E_FAIL;
	}

	/* Prototype_Component_Model_PlayerBoat */
	PreLocalTransformMatrix = XMMatrixScaling(0.008f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_PlayerBoat"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Boat/PlayerBoat3.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : PlayerBoat");
		return E_FAIL;
	}

	/* Prototype_Component_Model_Town */
	PreLocalTransformMatrix =  XMMatrixScaling(0.01f,0.01f,0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Town"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/GM_TOWN/GM_Town_tx.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}

	/* Prototype_Component_Model_Island_Collector */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) /** XMMatrixRotationY(XMConvertToRadians(180.f))*/;
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Island_Collector"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Island/Island_collector.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}

	/* Prototype_Component_Model_Fish */
	PreLocalTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fish"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Fish/Fish.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Fish");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));////////////////////////////////
	/* Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype :GameObject_Terrain");
		return E_FAIL;
	}

	///* Prototype_GameObject_Sea */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sea"),
	//	CSea::Create(m_pDevice, m_pContext))))
	//{
	//	MSG_BOX("Faild to Add_Prototype :GameObject_Sea");
	//	return E_FAIL;
	//}

	///* Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : VIBuffer_Cube");
	//	return E_FAIL;
	//}


	/* Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Monster");
		return E_FAIL;
	}


	/* Prototype_GameObject_Monster_Anim */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Anim"),
		CMonster_Anim::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Monster_Anim");
		return E_FAIL;
	}

	/* Prototype_GameObject_PlayerBoat */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PlayerBoat"),
		CPlayerBoat::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : PlayerBoat");
		return E_FAIL;
	}

	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Body_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Village */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Village"),
		CVillage::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Villsge");
		return E_FAIL;
	}

	/* Prototype_GameObject_Fish */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		CFish::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : fish");
		return E_FAIL;
	}






	/* Prototype_GameObject_R */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_R"),
		CMon_R::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}

	/* Prototype_GameObject_R_Act */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_R_Act"),
		CMon_R_Act::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}

	/* Prototype_GameObject_Tentacle */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Tentacle"),
		CMon_Tentacle::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}



	/* Prototype_GameObject_MonkFish */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MonkFish"),
		CMon_MonkFish::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : R");
		return E_FAIL;
	}




















	///* Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, m_pContext))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : GameObject_Sky");
	//	return E_FAIL;
	//}

	/* Prototype_GameObject_Snow */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		CSnow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Snow");
		return E_FAIL;
	}
	/* Prototype_GameObject_Explosion */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Explosion");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("충돌체를 로딩 중 입니다."));
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_AABB");
		return E_FAIL;
	}
	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_OBB");
		return E_FAIL;
	}
	/* Prototype_Component_Collider_Sphere*/
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_Sphere");
		return E_FAIL;
	}



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;

	return S_OK;

}
HRESULT CLoader::Loading_For_EditorLevel()
{
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//m_bFinished = true;

	return S_OK;
}

#ifdef _DEBUG
void CLoader::Print_LoadText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}
#endif

shared_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID)
{
	shared_ptr<CLoader> pInstance ( new CLoader(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
	}
	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	//쓰레드 사용하고 있음 갑자기 끝나면 무한루프로 쓰레드 끝날떄까지 기다리고 다 끝나면 지운다
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);
}
