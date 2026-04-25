#include "ManagerWin.h"
#include "GameInstance.h"
#include "Engine_Helper.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "EventBus.h"

CManagerWin::CManagerWin() : CEditor_Win("MangerWin")
{
}

CManagerWin::~CManagerWin()
{
}

HRESULT CManagerWin::Initialize()
{

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<EvtTerrainPicking>([this](const EvtTerrainPicking& p)
		{   this-> m_PickMode = true;
			m_iSelectedLevel = p.iLevel;
			m_strSelectedTag = p.Tag;
		
		});



	return S_OK;
}

void CManagerWin::Update(float fTimeDelta)
{
}

void CManagerWin::Render()
{

	ImGui::Begin("MangerWin");

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);


	ImGui::Separator();

	auto& Manager = CGameInstance::GetInstance()->Get_ManagerClass();

	if (Manager.empty())
	{
		ImGui::TextDisabled("(Empty)");

	}
	else
	{
		for (auto& pair : Manager)
		{
			_string strLayerTag = W2S(pair.first);

			if (!pair.second)
				continue;

			if (ImGui::TreeNodeEx(strLayerTag.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{


				pair.second->OnGui();

				ImGui::TreePop();
			}
		}
	}
	//if (m_PickMode == true)
	//{
	//	
	//	_uint iCurrentLevel = CGameInstance::GetInstance()->Get_Current_LevelIdx();
	//	_float3 Out = {};

	//	if (CGameInstance::GetInstance()->Picking_Terrain(L"Layer_BackGround", 0, &Out))
	//	{
	//		LOG_F(LOG_LEVEL::INFO, "x: %.2f  y: %.2f  z: %.2f", Out.x, Out.y, Out.z);

	//		if (m_pPreview == nullptr)
	//		{
	//		m_pPreview = (CGameInstance::GetInstance()->Add_GameObject(m_iSelectedLevel, m_strSelectedTag, iCurrentLevel, L"Static"));
	//		if (nullptr == m_pPreview)
	//		{
	//			MSG_BOX("Failed to Add GameObject to Layer");
	//		}
	//		

	//		}
	//		Out.y += 0.1f;
	//		if (m_pPreview)
	//		{
	//			auto transform = dynamic_pointer_cast<CTransform>(m_pPreview->Get_Component(g_strTransformTag));
	//			if (transform != nullptr)
	//			{
	//			_vector vec4 = XMVectorSetW(XMLoadFloat3(&Out), 1.f);
	//			transform->Set_Position( vec4);

	//			}

	//		}

	//		// 만일 마우스 클릭을하면 끝
	//		if (CGameInstance::GetInstance()->Get_DInput_Manger()->MouseDown(DIMB::LBUTTON))
	//		{

	//			m_PickMode = false;
	//			m_pPreview = nullptr;
	//		}

	//	}
	//	if (CGameInstance::GetInstance()->Get_DInput_Manger()->MouseDown(DIMB::RBUTTON))
	//	{
	//		m_PickMode = false;
	//		m_pPreview = nullptr;
	//	}
	//}
	if (m_PickMode == true)
	{
		_uint iCurrentLevel = CGameInstance::GetInstance()->Get_Current_LevelIdx();

		if (m_pPreview == nullptr)
		{
			m_pPreview = CGameInstance::GetInstance()->Add_GameObject(
				m_iSelectedLevel,
				m_strSelectedTag,
				iCurrentLevel,
				L"Static"
			);

			if (m_PickMode == true)
			{
				_uint iCurrentLevel = CGameInstance::GetInstance()->Get_Current_LevelIdx();

				CGameInstance::GetInstance()->Add_GameObject(
					m_iSelectedLevel,
					m_strSelectedTag,
					iCurrentLevel,
					L"Static"
				);

				m_PickMode = false;
			}

			m_pPreview = nullptr;
		}
	}
	ImGui::End();
}



shared_ptr<CManagerWin> CManagerWin::Create()
{
	shared_ptr<CManagerWin> pInstance(new CManagerWin());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : CManagerWin");
	}
	return pInstance;
}

