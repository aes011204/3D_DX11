#include "Camera_Manager.h"
#include "Camera.h"
#include "GameInstance.h"
#include "Engine_Helper.h"

CCamera_Manager::CCamera_Manager() :
	m_gameInstance{ CGameInstance::GetInstance() }
{
}

CCamera_Manager::~CCamera_Manager()
{
	Free();
}

HRESULT CCamera_Manager::Initialize(_uint numLevels)
{
	m_numLevel = numLevels;
	m_cameras = new map<const _wstring, shared_ptr<class CCamera>>[numLevels];

	return S_OK;

}

void CCamera_Manager::Update_AfterTarget(_float timeDelta)
{
	auto camera = m_curCam;
	if (camera)
		camera->Update_AfterTarget(timeDelta);
}

void CCamera_Manager::Apply_To_PipeLine()
{

	if (!m_curCam)
	{
		return;
	}

	m_gameInstance.lock()->Set_Transform(ED3DTS::VIEW,
		XMLoadFloat4x4(m_curCam->Get_CamTSMatrices(ED3DTS::VIEW)));

	m_gameInstance.lock()->Set_Transform(ED3DTS::PROJ,
		XMLoadFloat4x4(m_curCam->Get_CamTSMatrices(ED3DTS::PROJ)));
}

float CCamera_Manager::Get_Far()
{
	if (m_curCam != nullptr)
	{
		return m_curCam->Get_Far();
	}
	else
		return 500.f;
}

void CCamera_Manager::OnGui()
{
	

	
	// 2. 맵 주소들을 배열에 담기 (중괄호 세미콜론 주의!)

	for (_uint i = 0; i < m_numLevel; i++)
	{
		if (m_cameras[i].empty()) continue;

		for (auto& pair : m_cameras[i])
		{
			// wstring -> string 변환
			string  btnName = W2S(pair.first);

			// 활성화 하이라이트 체크
			bool isActive = (m_curCam == pair.second);

			if (isActive)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

			if (ImGui::Button(btnName.c_str()))
				CGameInstance::GetInstance()->Change_Camera(pair.first);

			if (isActive)
				ImGui::PopStyleColor();

			ImGui::SameLine();
		}
		// 구역 구분용 공백
        ImGui::Text("|");

    ImGui::NewLine();
	}
}

shared_ptr<class CCamera> CCamera_Manager::Find_Camera(_wstring key)
{
	for (_uint i = 0; i < m_numLevel; i++)
	{
	auto it = m_cameras[i].find(key);
	if (it != m_cameras[i].end())
	{
		return it->second;
	}

	}


	return nullptr;
}

HRESULT CCamera_Manager::Add_Camera(_uint camLevel, _wstring key, shared_ptr<CCamera> cam)
{

	auto findCam = Find_Camera(key);
	if (findCam != nullptr)
	{
		return E_FAIL;
		// 같은 이름을 가진객체가 있음
	}

	m_cameras[camLevel].emplace(key, cam);
	
		
	return S_OK;

}


_bool CCamera_Manager::Change_Camera(_wstring key)
{

	auto findCam = Find_Camera(key);
	if (findCam)
	{
		m_curCam = findCam;
		return true;
	}


	return true;
}

HRESULT CCamera_Manager::Clear_Camera(_uint level )
{

	m_cameras[level].clear();
	m_curCam = nullptr; // 일단 이렇게 free 카메라로 바꿔줄까??
	return S_OK;
}


unique_ptr<CCamera_Manager> CCamera_Manager::Create(_uint numLevels)
{
	unique_ptr<CCamera_Manager> instance(new CCamera_Manager());

	if (FAILED(instance->Initialize(numLevels)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Manager");
	}
	return instance;
}

void CCamera_Manager::Free()
{
	for (_uint i = 0; i <m_numLevel; i++)
	{
		m_cameras[i].clear();
	}
	delete[] m_cameras;
}
