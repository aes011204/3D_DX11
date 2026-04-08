#include "Transform.h"
#include "Shader.h"

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
	//m_vScale = {1.f,1.f,1.f};

	return S_OK;


	//프로토 타입을 만들지 않음 모든 트렌스폼이 원본일 예정
}

HRESULT CTransform::Initialize(void* pArg)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	if (nullptr == pArg)
		return S_OK;

	TRANSFOM_DESC* pDesc = static_cast<TRANSFOM_DESC*>(pArg);
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRadianPerSec = XMConvertToRadians(pDesc->fDegreePerSec);


	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotationDegree = pDesc->vRotationDegree;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(shared_ptr<CShader> pShaderCom, const _char* pConstantName)
{
	if (m_bIsDirty)
		Update_WorldMatrix();

	return 	pShaderCom->Bind_Matrix(pConstantName, &m_WorldMatrix);;
}

void CTransform::SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	/*Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);*/

	m_vScale = { fScaleX, fScaleY, fScaleZ };

	m_bIsDirty = true;
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	/*Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
	Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);*/

	m_vScale.x *= fScaleX;
	m_vScale.y *= fScaleY;
	m_vScale.z *= fScaleZ;



	m_bIsDirty = true;
}
void CTransform::Go_Forward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}
void CTransform::Go_Forward_Distanace(_float fDistance)
{
	_vector vPosition = Get_Position();
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * fDistance;

	Set_Position(vPosition);
	m_bIsDirty = true;
}
void CTransform::Go_Backward_Distanace(_float fDistance)
{
	_vector vPosition = Get_Position();
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * fDistance;

	Set_Position(vPosition);
	m_bIsDirty = true;
}
void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::UP);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::UP);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	//Set_State(STATE::POSITION, vPosition);
	Set_Position(vPosition);
	m_bIsDirty = true;
}
void CTransform::Start_Lerp(_fvector vTargetPos, _float3 vTargetRotation, _float fDuration)
{
	m_vStartPos = Get_Position();
	m_vStartQuat = Get_Quaternion();
	m_LerpTime = fDuration;
	m_LerpAcc = 0.f;
	m_IsLerping = true;

	m_vTargetPos = vTargetPos;
	m_vTargetQuat = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(vTargetRotation.x),
		XMConvertToRadians(vTargetRotation.y),
		XMConvertToRadians(vTargetRotation.z)
	);
	if (XMVectorGetX(XMQuaternionDot(m_vStartQuat, m_vTargetQuat)) < 0.f)
	{
		m_vTargetQuat = XMVectorNegate(m_vTargetQuat);
	}
}
void CTransform::Lerp_To(_float fTimeDelta)
{
	if (m_IsLerping == false)
		return;

	m_LerpAcc += fTimeDelta;
	if (m_LerpAcc >= m_LerpTime)
	{
		m_IsLerping = false;
		m_LerpAcc = m_LerpTime;
	}
		float t01 = m_LerpAcc / m_LerpTime;
		t01 = min(max(t01, 0.f), 1.f);
		
		//_float fDist = XMVectorGetX(XMVector3Length(vTargetPos - vCurPos));

	//fSpeed = lerp(fSpeed, 0, t01);

	Set_Position(XMVectorLerp(m_vStartPos, m_vTargetPos, t01));
	Set_Quaternion(XMQuaternionSlerp(m_vStartQuat, m_vTargetQuat, t01));
	
}

//void CTransform::Rotation(_fvector vAxis, _float fDegree)
//{
//	//_float3 vScaled = Get_Scaled();
//	//
//	//// vector는 대입도 함수를 통해해야함 우리간 생각하는 구조가 아님
//	//_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
//	//_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
//	//_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;
//	//
//	//_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fDegree));
//	//
//	////XMVector3TransformNormal(); - w가 0 곱할떄 행렬의 이동(Translation) 성분을 무시
//	////XMVector3TransformCoord(); -w가 1 곱할떄 행렬의 이동(Translation) 성분이 적용
//	//// XMVector4Transform(); - w에 확실히 뭐가 잇는지 알고때만/ 4x4 행렬 곱하기 하는거임
//	//
//	//Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
//	//Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
//	//Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
//
//	_vector vQuat = XMQuaternionRotationAxis(vAxis, XMConvertToRadians(fDegree));
//
//	XMStoreFloat4(&m_vRotationQuat, vQuat);
//	m_bIsDirty = true;
//}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	/*_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);


	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRadianPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));*/

	_vector vQuat = XMLoadFloat4(&m_vRotationQuat);

	_vector vRotDelta = XMQuaternionRotationAxis(vAxis, m_fRadianPerSec * fTimeDelta);

	vQuat = XMQuaternionMultiply(vQuat, vRotDelta);

	XMStoreFloat4(&m_vRotationQuat, vQuat);
	m_bIsDirty = true;
}
void CTransform::LookAt(_fvector vAt)
{
	//_vector vPosition = Get_State(STATE::POSITION);
	//_float3 vScaled = Get_Scaled();

	//_vector		vLook = vAt - vPosition;

	//_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	//_vector		vUp = XMVector3Cross(vLook, vRight);

	//Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	//Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	//Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
	////외적 결과로 나온 벡터들은 길이가 제멋대로림 그대로 행렬에 넣으면 물체의 크기가 변함 정규화를 통해 길이를 1로 만들어 순수 방향만 남깁
	_vector vPosition = Get_Position();
	_vector		vLook = vAt - vPosition;

	if (XMVector3LengthSq(vLook).m128_f32[0] < 0.000001f)
		return;
	_matrix matLookAt = XMMatrixLookAtLH(vPosition, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_matrix matWorldRot = XMMatrixInverse(nullptr, matLookAt);

	_vector vQuat = XMQuaternionRotationMatrix(matWorldRot);
	XMStoreFloat4(&m_vRotationQuat, vQuat);
	m_bIsDirty = true;
}

auto CTransform::Orbit(_fvector vTargetPos, _float3 vTargetRotationDegree, _float fDistance, _float fPitch, _float fYaw) -> void
{

	// 1. 공전용 회전 쿼터니언 생성
	_vector qOrbit = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), 0.f);


	
	_vector MoveXQuat = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(vTargetRotationDegree.y), 0.f);

	_vector qFinalOrbit = XMQuaternionMultiply(qOrbit, MoveXQuat);


	_vector vOffset = XMVectorSet(0.f, 0.f, -fDistance, 0.f);
	vOffset = XMVector3Rotate(vOffset, qFinalOrbit);

	_vector vNewPos = vTargetPos + vOffset;


	Set_Position(vNewPos);       // 위치 갱신
	//Set_Quaternion(qFinalOrbit);      // 회전 갱신 (타겟을 바라보게 됨)
}



_float3 CTransform::QuaternionToEuler(_float4 q)
{
	_float3 euler;

	// Roll (X축 회전)
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (Y축 회전)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(XM_PI / 2.f, sinp);
	else
		euler.y = std::asin(sinp);

	// Yaw (Z축 회전)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	// Radian -> Degree 변환
	euler.x = XMConvertToDegrees(euler.x);
	euler.y = XMConvertToDegrees(euler.y);
	euler.z = XMConvertToDegrees(euler.z);

	return euler;
}



void CTransform::Update_WorldMatrix()
{
	
	_matrix matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	_matrix matRot = XMMatrixRotationQuaternion(XMLoadFloat4(&m_vRotationQuat));
	_matrix matTrans = XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	_matrix matWorld = matScale * matRot * matTrans;



	/*matWorld *= */

	if (m_pParentMatrix)
	{
		_matrix matParent = XMLoadFloat4x4(m_pParentMatrix);
		matWorld *= matParent;
	}

	XMStoreFloat4x4(&m_WorldMatrix, matWorld);

	m_bIsDirty = false;
}

void CTransform::OnGui()
{
	if (m_bIsDirty)
		Update_WorldMatrix();

	_float3 vPosition;
	vPosition = _float3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);



	static _float3 vEditRotation = { 1.f, 1.f, 0.f };

	// ---- ImGui UI 그리기 ----

	// Position
	if (ImGui::DragFloat3("Position", (float*)&vPosition, 0.1f))
	{

		_vector vNextPos = XMLoadFloat3(&vPosition);
		vNextPos = XMVectorSetW(vNextPos, 1.f);
		//Set_State(STATE::POSITION, vNextPos);
		Set_Position(vNextPos);
	}
	// Rotation (각도 단위)
	if (ImGui::DragFloat3("Rotation", (float*)&vEditRotation, 0.5f))
	{
		// 입력받은 Degree 각도를 쿼터니언으로 변환
		_vector vQuat = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(vEditRotation.x),
			XMConvertToRadians(vEditRotation.y),
			XMConvertToRadians(vEditRotation.z)
		);
		Set_Quaternion(vQuat); // 여기서 m_bIsDirty = true 가 됨
		m_bIsDirty = true;
	}


	_float3 vScale = Get_Scaled();

	if (ImGui::DragFloat3("Scale", (float*)&vScale, 0.1f, 0.01f, 100.f))
	{

		SetUp_Scale(vScale.x, vScale.y, vScale.z);
	}
	
	// 기타 정보 (속도 등)
	ImGui::Separator();
	ImGui::SliderFloat("Move Speed", &m_fSpeedPerSec, 0.f, 100.f);
	ImGui::SliderFloat("Turn Speed", &m_fRadianPerSec, 0.f, XM_PI);


	ImGui::Text("Full World Matrix (Raw Data)");

	// 행렬의 내용을 4x4 표 형태로 출력
	if (ImGui::BeginTable("MatrixTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	{
		for (int i = 0; i < 4; ++i)
		{
			ImGui::TableNextRow();
			for (int j = 0; j < 4; ++j)
			{
				ImGui::TableSetColumnIndex(j);
				// m_WorldMatrix.m[행][열] 데이터 출력
				float val = m_WorldMatrix.m[i][j];

				// 값이 너무 작거나 nan이면 빨간색으로 표시 (디버깅 꿀팁)
				if (isnan(val))
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "nan");
				else
					ImGui::Text("%.3f", val);
			}
		}
		ImGui::EndTable();
	}

}

void CTransform::Save_ToJson(nlohmann::json& j)
{
	j["Type"] = "Transform";
	j["Position"] = { m_vPosition.x, m_vPosition.y, m_vPosition.z };
	j["Scale"] = { m_vScale.x, m_vScale.y, m_vScale.z };

	
	j["Rotation"] = {  m_vRotationDegree.x, m_vRotationDegree.y, m_vRotationDegree.z };

	j["Move Speed"] = m_fSpeedPerSec;
	j["Turn Speed"] = m_fRadianPerSec;


}

void CTransform::Load_FromJson(nlohmann::json& j)
{
	if (j.contains("Position")) {
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
		_float3 f3 = { j["Position"][0], j["Position"][1] ,j["Position"][2]  };
		//Set_State(STATE::POSITION,XMLoadFloat4(&f4));
		Set_Position(XMLoadFloat3(&f3));
	}

	if (j.contains("Scale")) {
		_float3 Scale = { j["Scale"][0], j["Scale"][1] ,j["Scale"][2] };
		SetUp_Scale(Scale.x, Scale.y, Scale.z);
	}
		//Rotation()
	if (j.contains("Rotation")) {
		_float3 Rotaion = { j["Rotation"][0], j["Rotation"][1] ,j["Rotation"][2] };
		Set_RotationDegree(Rotaion);

	}

	m_fSpeedPerSec = j["Move Speed"];
	m_fRadianPerSec = j["Turn Speed"];

	
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


