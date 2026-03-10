#include "Camera_Manager.h"
#include "Camera.h"
#include "GameInstance.h"
#include "Engine_Helper.h"

CCamera_Manager::CCamera_Manager() :
	m_pGameInstance{ CGameInstance::GetInstance() }
{
}

CCamera_Manager::~CCamera_Manager()
{
	Free();
}

HRESULT CCamera_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevel = iNumLevels;
	m_Cameras = new map<const _wstring, shared_ptr<class CCamera>>[iNumLevels];

	return S_OK;

}

void CCamera_Manager::Apply_To_PipeLine()
{

	if (!m_CurCam)
	{
		return;
	}

	m_pGameInstance.lock()->Set_Transform(D3DTS::VIEW,
		XMLoadFloat4x4(m_CurCam->Get_CamTSMatrices(D3DTS::VIEW)));

	m_pGameInstance.lock()->Set_Transform(D3DTS::PROJ,
		XMLoadFloat4x4(m_CurCam->Get_CamTSMatrices(D3DTS::PROJ)));
}

void CCamera_Manager::OnGui()
{
	

	
	// 2. 맵 주소들을 배열에 담기 (중괄호 세미콜론 주의!)

	for (_uint i = 0; i < m_iNumLevel; i++)
	{
		if (m_Cameras[i].empty()) continue;

		for (auto& pair : m_Cameras[i])
		{
			// wstring -> string 변환
			string  strBtnName = W2S(pair.first);

			// 활성화 하이라이트 체크
			bool bIsActive = (m_CurCam == pair.second);

			if (bIsActive)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

			if (ImGui::Button(strBtnName.c_str()))
				CGameInstance::GetInstance()->Change_Camera(pair.first);

			if (bIsActive)
				ImGui::PopStyleColor();

			ImGui::SameLine();
		}
		// 구역 구분용 공백
		ImGui::Text("|"); ImGui::SameLine();

	}
}

shared_ptr<class CCamera> CCamera_Manager::Find_Camera(_wstring key)
{
	for (_uint i = 0; i < m_iNumLevel; i++)
	{
	auto it = m_Cameras[i].find(key);
	if (it != m_Cameras[i].end())
	{
		return it->second;
	}

	}


	return nullptr;
}

HRESULT CCamera_Manager::Add_Camera(_uint camLevel, _wstring key, shared_ptr<CCamera> cam)
{

	auto FindCam = Find_Camera(key);
	if (FindCam != nullptr)
	{
		return E_FAIL;
		// 같은 이름을 가진객체가 있음
	}

	m_Cameras[camLevel].emplace(key, cam);
	
		
	return S_OK;

}


_bool CCamera_Manager::Change_Camera(_wstring key)
{

	auto FindCam = Find_Camera(key);
	if (FindCam)
	{
		m_CurCam = FindCam;
		return true;
	}


	return true;
}

HRESULT CCamera_Manager::Clear_Camera(_uint level )
{

	m_Cameras[level].clear();
	m_CurCam = nullptr; // 일단 이렇게 free 카메라로 바꿔줄까??
	return S_OK;
}


unique_ptr<CCamera_Manager> CCamera_Manager::Create(_uint iNumLevels)
{
	unique_ptr<CCamera_Manager> pInstance(new CCamera_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Manager");
	}
	return pInstance;
}

void CCamera_Manager::Free()
{
	for (_uint i = 0; i <m_iNumLevel; i++)
	{
		m_Cameras[i].clear();
	}
	delete[] m_Cameras;
}
