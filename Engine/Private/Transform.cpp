#include "Transform.h"

CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& prototype)
	: CComponent(prototype)
{
}
// 생성할떄 create 안에서 Initialize_Prototype 부르고 밖에서 Initialize() 부음
HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;


	//프로토 타입을 만들지 않음 모든 트렌스폼이 원본일 예정
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFOM_DESC* pDesc = static_cast<TRANSFOM_DESC*>(pArg);
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRadianPerSec = XMConvertToRadians(pDesc->fDegreePerSec);


	return S_OK;
}

void CTransform::SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
	Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);
}
void CTransform::Go_Forward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fDegree)
{
	_float3 vScaled = Get_Scaled();

	// vector는 대입도 함수를 통해해야함 우리간 생각하는 구조가 아님
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fDegree));

	//XMVector3TransformNormal(); - w가 0 곱할떄 행렬의 이동(Translation) 성분을 무시
	//XMVector3TransformCoord(); -w가 1 곱할떄 행렬의 이동(Translation) 성분이 적용
	// XMVector4Transform(); - w에 확실히 뭐가 잇는지 알고때만/ 4x4 행렬 곱하기 하는거임

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);


	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRadianPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::LookAt(_fvector vAt)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_float3 vScaled = Get_Scaled();

	_vector		vLook = vAt - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
	//외적 결과로 나온 벡터들은 길이가 제멋대로림 그대로 행렬에 넣으면 물체의 크기가 변함 정규화를 통해 길이를 1로 만들어 순수 방향만 남깁
}

void CTransform::OnGui()
{
	// 1. 행렬에서 현재 데이터 추출 (위치, 회전, 스케일)
	_float3 vPosition, vRotation, vScale;

	// 위치 추출 (4행)
	vPosition = _float3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);

	// 스케일 추출 (각 축 벡터의 길이)
	vScale = Get_Scaled();

	// 회전 추출 (임시로 0,0,0으로 두거나, 별도의 m_vRotation 멤버가 있다면 그것을 사용)
	// 행렬에서 순수 회전각을 추출하는 것은 분해(Decompose) 과정이 필요합니다.
	// 여기서는 간단하게 조절값만 보여주는 예시입니다.
	static _float3 vEditRotation = { 0.f, 0.f, 0.f };

	// ---- ImGui UI 그리기 ----

	// Position
	if (ImGui::DragFloat3("Position", (float*)&vPosition, 0.1f))
	{
		Set_State(STATE::POSITION, XMLoadFloat3(&vPosition));
	}

	// Rotation (각도 단위)
	if (ImGui::DragFloat3("Rotation", (float*)&vEditRotation, 0.5f))
	{
		// 1. 기존 스케일 유지하며 회전 적용하려면 복잡하므로 
		// 2. 간단하게 구현하려면 Rotation 함수들을 활용하세요.
		// Rotation(XMVectorSet(1,0,0,0), XMConvertToRadians(vEditRotation.x));
		// ... (나머지 축)
	}

	// Scale
	if (ImGui::DragFloat3("Scale", (float*)&vScale, 0.1f))
	{
		SetUp_Scale(vScale.x, vScale.y, vScale.z);
	}

	// 기타 정보 (속도 등)
	ImGui::Separator();
	ImGui::SliderFloat("Move Speed", &m_fSpeedPerSec, 0.f, 100.f);
	ImGui::SliderFloat("Turn Speed", &m_fRadianPerSec, 0.f, XM_PI);
}


shared_ptr<CTransform> CTransform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTransform> pInstance (new CTransform(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
	}
	return pInstance;
}

shared_ptr<CComponent> CTransform::Clone(void* pArg)
{
	return nullptr;
	// 클론도 하지 않을것임
}

void CTransform::Free()
{
	__super::Free();
}


