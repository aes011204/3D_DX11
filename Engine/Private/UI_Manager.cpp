#include "UI_Manager.h"
#include "UI.h"
#include "UICanvas.h"
#include "Engine_Helper.h"

CUI_Manager::CUI_Manager()
{
}
CUI_Manager::~CUI_Manager()
{
	Free();
}
HRESULT CUI_Manager::Initialize(_uint width, _uint height)
{
	m_UICanvas = CUICanvas::Create(width, height);

	return S_OK;
}

shared_ptr<CUI> CUI_Manager::FindUIOnPool(wstring type)
{
	auto it = m_UIPool.find(type);

	if (it == m_UIPool.end())
	{
		OnFail();
		MSG_BOX("Failed OnLoad : No Same Key in UIPool");
		return nullptr;
	}

	return it->second; // UIpool에서 해당 레이어 찾음
}

void CUI_Manager::Update(float m_fDeltaTime)
{
	ProcessUIQ();

	bool bMouseHold = {false};

	//순서(OVERRIDE -> WINDOW -> STACK -> HUD)
	for (int i = 0; i < ETOI(UI_LAYER::END); i++)
	{
		for (auto it = m_UI[i].rbegin();it != m_UI[i].rend(); it++)
		{
			(*it)->Update(m_fDeltaTime, bMouseHold);
		}

	}

}

void CUI_Manager::LateUpdate(float m_fDeltaTime)
{
	//순서(OVERRIDE -> WINDOW -> STACK -> HUD)
	for (int i = 0; i < ETOI(UI_LAYER::END); i++)
	{
		for (auto it = m_UI[i].rbegin();it != m_UI[i].rend(); it++)
		{
			(*it)->Late_Update(m_fDeltaTime);
		}

	}
}

void CUI_Manager::Render()
{
}

void CUI_Manager::OnFail()
{
	m_ActiveUI = nullptr;
}

void CUI_Manager::Push(UI_LAYER layer, wstring name, void* pArg)
{
	RequestUI tmp{ name, layer, pArg };

	m_RequestUIQueue.push(tmp);
}

void CUI_Manager::Pop(UI_LAYER layer, wstring type)
{
	//일단 스택,오버레이 이면 pop_back만
	// 다른거는 그냥 제거

	auto pUIPair = FindUIOnPool(type);

	if(layer==UI_LAYER::HUD|| layer == UI_LAYER::WINDOW)
	{
		for(auto it = m_UI[ETOI(layer)].begin();it != m_UI[ETOI(layer)].end();)
		{
			if ((*it) == pUIPair)
			{

				m_UI[ETOI(layer)].erase(it);
				break;
			}
			else
				it++;
		}
	}
	else
	{
		if (!m_UI[ETOI(layer)].empty())
		{


		m_UI[ETOI(layer)].pop_back();
			
		}
	}


}

void CUI_Manager::Detach_All()
{
	//띄어져 있는 레이어에서제거
	for(int i =0; i < ETOI(UI_LAYER::END); i++)
	{
		while (!m_UI[i].empty())
		{
			// 여기서 레이어에서 삭제하기

			m_UI[i].pop_back();
		}
		
	}
}

void CUI_Manager::InsertToPool(wstring UIType, shared_ptr<CUI> UI)
{
	// 이미 있는지 확인
	auto it = m_UIPool.find(UIType);

	m_UIKey = UIType;

	if(it != m_UIPool.end())
	{
		return;
	}

	m_UIPool.emplace(UIType, UI);

	
}

Rect CUI_Manager::Get_m_UICanvasRect()
{
	{ return m_UICanvas->Get_CanvasSize(); }
}


void CUI_Manager::ProcessUIQ()
{
	if (!m_RequestUIQueue.empty())
	{
		RequestUI UIQ = m_RequestUIQueue.front();
		m_RequestUIQueue.pop();

		shared_ptr<CUI> addUI = FindUIOnPool(UIQ.UIKey);
		if(addUI == nullptr)
		{
			return;
		}
		/*OnComplete(UIQ.Layer, UIQ.UIKey, UIQ.pArg);*/

		if (addUI == nullptr)
			return;

		// 이미 있으면 젤위로 
		auto it = find(m_UI[ETOI(UIQ.Layer)].begin(), m_UI[ETOI(UIQ.Layer)].end(), addUI);
		if (it != m_UI[ETOI(UIQ.Layer)].end())
		{
			m_UI[ETOI(UIQ.Layer)].erase(it);
		}

		addUI->Initialize(UIQ.pArg); // 넣었다 뺏다 할떄 계속 불릴 위험있음 CBase 안에서 bool로 처리
		addUI->UI_Active();

		//// 이건 ui 매니져에 있는 활성화된 ui 모아두는 곳에 있음
		m_UI[ETOI(UIQ.Layer)].push_back(addUI);
		m_ActiveUI = addUI;
		
	}

}

unique_ptr<CUI_Manager> CUI_Manager::Create(_uint width, _uint height)
{
	unique_ptr<CUI_Manager> pInstance (new CUI_Manager());
	if (FAILED(pInstance->Initialize(width, height)))
	{
		MSG_BOX("failed prototype: CUI_Manager");
		
	}
	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	// 띄워져 있는 ui 내리기
	Detach_All();

	//pool 원본 삭제 
	//for (auto& pair : m_UIPool)
	//{
	//	Safe_Release(pair.second);
	//}
	m_UIPool.clear();

	m_UICanvas.reset();
}


void CUI_Manager::OnGui()
{
	if (ImGui::TreeNodeEx("UI Pool", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const auto& pool = m_UIPool;
		if (pool.empty()) {
			ImGui::TextDisabled("(Pool Empty)");
		}
		else {
			for (auto& pair : pool)
			{
				_string strKey = ConvertW2A(pair.first);
				auto& root = pair.second;
				if (!root) continue;

				const auto& children = root->GetChildren();

				_string imguiLabel = strKey + " [Prototype]##Pool" + std::to_string((uint64_t)root.get());

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

				if (m_pSelectedObject == root) flags |= ImGuiTreeNodeFlags_Selected;

				//  Leaf면 push 안 하게
				if (children.empty())
					flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				bool opened = ImGui::TreeNodeEx(imguiLabel.c_str(), flags);

				if (ImGui::IsItemClicked())
				{
					m_pSelectedObject = root;

					//Editor::EvtSelectEntity ev{};
					//ev.Entity = std::static_pointer_cast<Engine::CEntity>(root);

					//CEditorInstance::GetInstance()->GetEventBus()->Publish(ev);

				}

				//  자식이 있을 때만, opened면 TreePop 필수
				if (!children.empty() && opened)
				{
					for (auto& child : children)
						DrawUITree(child);

					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}

}

void CUI_Manager::DrawUITree(const shared_ptr<Engine::CUI>& ui)
{
	if (!ui) return;

	_string label = ConvertW2A(ui->Get_Name());
	if (label.empty()) label = "UI_Child";

	_string imguiLabel = label + "##UI" + std::to_string((uint64_t)ui.get());

	const auto& children = ui->GetChildren();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (m_pSelectedObject == ui) flags |= ImGuiTreeNodeFlags_Selected;

	//  Leaf면 push 안 하게
	if (children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	bool opened = ImGui::TreeNodeEx(imguiLabel.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		m_pSelectedObject = ui;

		//Editor::EvtSelectEntity ev{};
		//ev.Entity = std::static_pointer_cast<Engine::CEntity>(ui);

		//CEditorInstance::GetInstance()->GetEventBus()->Publish(ev);

	}

	//  자식이 있을 때만 TreePop
	if (!children.empty() && opened)
	{
		for (auto& child : children)
			DrawUITree(child);

		ImGui::TreePop();
	}
}