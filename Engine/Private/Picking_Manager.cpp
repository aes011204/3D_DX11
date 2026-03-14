#include "Picking_Manager.h"
#include "GameInstance.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"
#include "Level.h"
#include "GameObject.h"



CPicking_Manager::CPicking_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, 
	m_pGameInstance{ CGameInstance::GetInstance() }
{
}

CPicking_Manager::~CPicking_Manager()
{
	Free();
}

HRESULT CPicking_Manager::Initialize()
{


    return S_OK;
}

void CPicking_Manager::Update()
{
	_float3 fNDC = {};
	_float2 Pos = m_pGameInstance.lock()->Get_MousePos();

	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewPort = { 1 };
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPortDesc);

	//NDC
	fNDC.x = Pos.x / (ViewPortDesc.Width * 0.5f) - 1.f;
	fNDC.y = Pos.y / -(ViewPortDesc.Height * 0.5f) + 1.f;
	fNDC.z = 0.f;

	_vector vNDC = XMLoadFloat3(&fNDC);

	//투영 -> 뷰스페이스
	const _float4x4* InverseProj = m_pGameInstance.lock()->Get_InverseTransfrom(D3DTS::PROJ);
	//XMMatrixInverse(&matProj, 0, &matProj);
	vNDC = XMVector3TransformCoord(vNDC, XMLoadFloat4x4(InverseProj));

	//뷰스페이스 -> 월드 

	const _float4x4* view = m_pGameInstance.lock()->Get_InverseTransfrom(D3DTS::VIEW);

	Ray ray = {};
	ray.position = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	ray.direction = vNDC - XMLoadFloat3(&ray.position);

	ray.position = XMVector3TransformCoord(ray.position, XMLoadFloat4x4(view));
	ray.direction = XMVector3TransformNormal(ray.direction, XMLoadFloat4x4(view));

	m_CurrentRay = ray;
}

_bool CPicking_Manager::Culaulate_Terrain(CVIBuffer_Terrain* pBuffer, CTransform* pTransform, _float3* pOutPos)
{
	if (pBuffer == nullptr || pTransform == nullptr)
		return false;

	Ray LocalRay = m_CurrentRay;

	// 월드-> 로컬
	const _float4x4* World = pTransform->Get_WorldMatrix();
	_matrix InvWorld = XMMatrixInverse(0, XMLoadFloat4x4(World));

	LocalRay.position = XMVector3TransformCoord(LocalRay.position,InvWorld);
	LocalRay.direction = XMVector3TransformNormal(LocalRay.direction,InvWorld);
	// 4번째 성분에 0을 곱한다 이동(Translation)을 무시하기 위해서
	
	LocalRay.direction = XMVector3Normalize(LocalRay.direction);
	// 길이를 1로 만듬 위에거랑 다른거임
	
		_ulong		dwVtxNumber[3]{};
	const _float3* pTerrainVtxPos = pBuffer->Get_VtxPos();
	_uint pTerrainVtxNumX = pBuffer->Get_NumVerticeX();
	_uint pTerrainVtxNumZ = pBuffer->Get_NumVerticeZ();
	_float		fU(0.f), fV(0.f), fDist(0.f);


	for (_ulong i = 0; i < pTerrainVtxNumZ - 1; ++i)
	{
		for (_ulong j = 0; j < pTerrainVtxNumX- 1; ++j)
		{
			_ulong dwIndex = i * pTerrainVtxNumX + j;

			// 오른쪽 위

			dwVtxNumber[0] = dwIndex + pTerrainVtxNumX;
			dwVtxNumber[1] = dwIndex + pTerrainVtxNumX + 1;
			dwVtxNumber[2] = dwIndex + 1;

			XMVECTOR v0 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[0]]);
			XMVECTOR v1 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[1]]);
			XMVECTOR v2 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[2]]);


			// V1 + U(V2 - V1) + V(V3 - V1)
			float fDist = 0.f;
			if (DirectX::TriangleTests::Intersects(LocalRay.position, LocalRay.direction,
				v0, v1, v2, fDist))
			{
				_float3 result = {};
				XMStoreFloat3(&result,
					LocalRay.position + (LocalRay.direction * fDist));
			
				*pOutPos = result;
				return true;

			}


			// 왼쪽 아래

			dwVtxNumber[0] = dwIndex + pTerrainVtxNumX;
			dwVtxNumber[1] = dwIndex + 1;
			dwVtxNumber[2] = dwIndex;

			v0 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[0]]);
			v1 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[1]]);
			v2 = XMLoadFloat3(&pTerrainVtxPos[dwVtxNumber[2]]);

			

			fDist = 0.f;
			if (DirectX::TriangleTests::Intersects(LocalRay.position, LocalRay.direction,
				v0, v1, v2, fDist))
			{
				_float3 result = {};
				XMStoreFloat3(&result,
					LocalRay.position + (LocalRay.direction * fDist));

				*pOutPos = result;
				return true;
			}
		}
	}



	return false;
}

_bool CPicking_Manager::Picking_Terrain(  _wstring layerTag, _uint TerrainIndex, _float3* Out )
{
	_uint levelIndex = m_pGameInstance.lock()->Get_Current_LevelIdx();
	auto terrain = m_pGameInstance.lock()->Get_GameObject(levelIndex, layerTag, TerrainIndex);
	auto transform = dynamic_pointer_cast<CTransform>( terrain->Get_Component(g_strTransformTag));
	auto buffer = dynamic_pointer_cast<CVIBuffer_Terrain>(terrain->Get_Component(L"Com_VIBuffer"));

	return Culaulate_Terrain(buffer.get(), transform.get(), Out);
}

unique_ptr<CPicking_Manager> CPicking_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CPicking_Manager> pInstance(new CPicking_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : CPicking_Manager");
	}
	return pInstance;
}

void CPicking_Manager::Free()
{
}
