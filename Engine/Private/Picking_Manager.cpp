#include "Picking_Manager.h"
#include "GameInstance.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"
#include "Level.h"
#include "GameObject.h"



CPicking_Manager::CPicking_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device{ device }, m_context{ context },
	m_gameInstance{ CGameInstance::GetInstance() }
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
	_float3 NDC = {};
	_float2 Pos = m_gameInstance.lock()->Get_MousePos();

	D3D11_VIEWPORT viewPortDesc{};
	_uint numViewPort = { 1 };
	m_context->RSGetViewports(&numViewPort, &viewPortDesc);

	//NDC
	NDC.x = Pos.x / (viewPortDesc.Width * 0.5f) - 1.f;
	NDC.y = Pos.y / -(viewPortDesc.Height * 0.5f) + 1.f;
	NDC.z = 0.f;

	_vector vecNDC = XMLoadFloat3(&NDC);

	//투영 -> 뷰스페이스
	const _float4x4* inverseProj = m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::PROJ);
	//XMMatrixInverse(&matProj, 0, &matProj);
	vecNDC = XMVector3TransformCoord(vecNDC, XMLoadFloat4x4(inverseProj));

	//뷰스페이스 -> 월드 

	const _float4x4* view = m_gameInstance.lock()->Get_InverseTransfrom(ED3DTS::VIEW);

	Ray ray = {};
	ray.position = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	ray.direction = vecNDC - XMLoadFloat3(&ray.position);

	ray.position = XMVector3TransformCoord(ray.position, XMLoadFloat4x4(view));
	ray.direction = XMVector3TransformNormal(ray.direction, XMLoadFloat4x4(view));

	m_currentRay = ray;
}

_bool CPicking_Manager::Culaulate_Terrain(CVIBuffer_Terrain* buffer, CTransform* transform, _float3* outPos)
{
	if (buffer == nullptr || transform == nullptr)
		return false;

	Ray localRay = m_currentRay;

	// 월드-> 로컬
	const _float4x4* World = transform->Get_WorldMatrix();
	_matrix invWorld = XMMatrixInverse(0, XMLoadFloat4x4(World));

	localRay.position = XMVector3TransformCoord(localRay.position, invWorld);
	localRay.direction = XMVector3TransformNormal(localRay.direction, invWorld);
	// 4번째 성분에 0을 곱한다 이동(Translation)을 무시하기 위해서

	localRay.direction = XMVector3Normalize(localRay.direction);
	// 길이를 1로 만듬 위에거랑 다른거임

	_ulong		vtxNumber[3]{};
	const _float3* terrainVtxPos = buffer->Get_VtxPos();
	_uint terrainVtxNumX = buffer->Get_NumVerticeX();
	_uint terrainVtxNumZ = buffer->Get_NumVerticeZ();
	_float		u(0.f), v(0.f), dist(0.f);


	for (_ulong i = 0; i < terrainVtxNumZ - 1; ++i)
	{
		for (_ulong j = 0; j < terrainVtxNumX - 1; ++j)
		{
			_ulong index = i * terrainVtxNumX + j;

			// 오른쪽 위

			vtxNumber[0] = index + terrainVtxNumX;
			vtxNumber[1] = index + terrainVtxNumX + 1;
			vtxNumber[2] = index + 1;

			XMVECTOR v0 = XMLoadFloat3(&terrainVtxPos[vtxNumber[0]]);
			XMVECTOR v1 = XMLoadFloat3(&terrainVtxPos[vtxNumber[1]]);
			XMVECTOR v2 = XMLoadFloat3(&terrainVtxPos[vtxNumber[2]]);


			// V1 + U(V2 - V1) + V(V3 - V1)
			float dist = 0.f;
			if (DirectX::TriangleTests::Intersects(localRay.position, localRay.direction,
				v0, v1, v2, dist))
			{
				_float3 result = {};
				XMStoreFloat3(&result,
					localRay.position + (localRay.direction * dist));

				*outPos = result;
				return true;

			}


			// 왼쪽 아래

			vtxNumber[0] = index + terrainVtxNumX;
			vtxNumber[1] = index + 1;
			vtxNumber[2] = index;

			v0 = XMLoadFloat3(&terrainVtxPos[vtxNumber[0]]);
			v1 = XMLoadFloat3(&terrainVtxPos[vtxNumber[1]]);
			v2 = XMLoadFloat3(&terrainVtxPos[vtxNumber[2]]);



			dist = 0.f;
			if (DirectX::TriangleTests::Intersects(localRay.position, localRay.direction,
				v0, v1, v2, dist))
			{
				_float3 result = {};
				XMStoreFloat3(&result,
					localRay.position + (localRay.direction * dist));

				*outPos = result;
				return true;
			}
		}
	}



	return false;
}

_bool CPicking_Manager::Picking_Terrain(_wstring layerTag, _uint terrainIndex, _float3* Out)
{
	_uint levelIndex = m_gameInstance.lock()->Get_Current_LevelIdx();
	auto terrain = m_gameInstance.lock()->Get_GameObject(levelIndex, layerTag, terrainIndex);
	auto transform = dynamic_pointer_cast<CTransform>(terrain->Get_Component(g_strTransformTag));
	auto buffer = dynamic_pointer_cast<CVIBuffer_Terrain>(terrain->Get_Component(L"Com_VIBuffer"));

	return Culaulate_Terrain(buffer.get(), transform.get(), Out);
}


_float CPicking_Manager::Calculate_HeightOnTerrain(const _fvector pos,

	const _fvector pointA,
	const _fvector pointB,
	const _fvector pointC)
{
	_vector plane = XMPlaneFromPoints(pointA, pointB, pointC);

	// ax + by + cz + d = 0;

	// by = -ax - cz - d

	// y = (-ax - cz - d) / b;

	return  (-XMVectorGetX(plane) * XMVectorGetX(pos) - XMVectorGetZ(plane) * XMVectorGetZ(pos) - XMVectorGetW(plane)) / XMVectorGetY(plane);
}

_bool CPicking_Manager::Compute_HeightOnTerrain(_wstring layerTag, _uint terrainIndex, const _fvector curPos,  _float* Out)

{

	_uint levelIndex = m_gameInstance.lock()->Get_Current_LevelIdx();
	auto terrain = m_gameInstance.lock()->Get_GameObject(levelIndex, layerTag, terrainIndex);
	auto transform = dynamic_pointer_cast<CTransform>(terrain->Get_Component(g_strTransformTag));
	auto buffer = dynamic_pointer_cast<CVIBuffer_Terrain>(terrain->Get_Component(L"Com_VIBuffer"));

	_uint cntX = buffer->Get_NumVerticeX();
	_float3* terrainVtxPos = buffer->Get_VtxPos();
	_float3 pos = {};
	XMStoreFloat3(&pos,curPos);

	_ulong		index = _ulong(pos.z) * cntX + _ulong(pos.x );

	_float		width = (pos.x - terrainVtxPos[index + cntX].x) ;
	_float		height = (terrainVtxPos[index + cntX].z - pos.z);

	_vector		plane;

	// ax + by + cz + d = 0;

	// by = -ax - cz - d

	// y = (-ax - cz - d) / b;

	// 오른쪽 위
	if (width >= height)
	{
		plane = XMPlaneFromPoints(
			XMLoadFloat3(&terrainVtxPos[index + cntX]),
			XMLoadFloat3(&terrainVtxPos[index + cntX + 1]),
			XMLoadFloat3(&terrainVtxPos[index + 1]));
		*Out = (-XMVectorGetX(plane) * XMVectorGetX(curPos) - XMVectorGetZ(plane) * XMVectorGetZ(curPos) - XMVectorGetW(plane)) / XMVectorGetY(plane);
		return true;
	}
	// 왼쪽 아래
	else if (width < height)
	{
		plane = XMPlaneFromPoints(
			XMLoadFloat3(&terrainVtxPos[index + cntX]),
			XMLoadFloat3(&terrainVtxPos[index + 1]),
			XMLoadFloat3(&terrainVtxPos[index]));
		*Out = (-XMVectorGetX(plane) * XMVectorGetX(curPos) - XMVectorGetZ(plane) * XMVectorGetZ(curPos) - XMVectorGetW(plane)) / XMVectorGetY(plane);
		return true;
	}
	else
		return false;

}


unique_ptr<CPicking_Manager> CPicking_Manager::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	unique_ptr<CPicking_Manager> instance(new CPicking_Manager(device, context));

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : CPicking_Manager");
	}
	return instance;
}

void CPicking_Manager::Free()
{
}
