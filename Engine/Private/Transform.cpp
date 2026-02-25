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
	if (nullptr == pArg)
		return S_OK;

	TRANSFOM_DESC* pDesc = static_cast<TRANSFOM_DESC*>(pArg);
	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRadianPerSec = XMConvertToRadians(pDesc->fDegreePerSec);


	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(shared_ptr<CShader> pShaderCom, const _char* pConstantName)
{
	return 	pShaderCom->Bind_Matrix(pConstantName, &m_WorldMatrix);;
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

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::UP);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector			vPosition = Get_State(STATE::POSITION);
	_vector			vRight = Get_State(STATE::UP);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

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
	_float3 vPosition;
	vPosition = _float3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);



	static _float3 vEditRotation = { 1.f, 1.f, 0.f };

	// ---- ImGui UI 그리기 ----

	// Position
	if (ImGui::DragFloat3("Position", (float*)&vPosition, 0.1f))
	{

		_vector vNextPos = XMLoadFloat3(&vPosition);
		vNextPos = XMVectorSetW(vNextPos, 1.f);
		Set_State(STATE::POSITION, vNextPos);
	}
	// Rotation (각도 단위)
	if (ImGui::DragFloat3("Rotation", (float*)&vEditRotation, 0.5f))
	{

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
	j["Position"] = { m_WorldMatrix._41,m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44 };
	j["Scale"] = { Get_Scaled().x,Get_Scaled().y, Get_Scaled().z};
	j["Rotation"] = {0.f,0.f,0.f,1.f};

	j["Move Speed"] = m_fSpeedPerSec;
	j["Turn Speed"] = m_fRadianPerSec;


}

void CTransform::Load_FromJson(nlohmann::json& j)
{

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	_float4 f4 = { j["Position"][0], j["Position"][1] ,j["Position"][2] ,j["Position"][3] };
	Set_State(STATE::POSITION,XMLoadFloat4(&f4));
	_float3 f3 = { j["Scale"][0], j["Scale"][1] ,j["Scale"][2] };
	SetUp_Scale(f3.x, f3.y, f3.z);
		//Rotation()
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


