
#include "HierarchyWin.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "Layer.h"
#include "UI.h"
#include "UI_Manager.h"
#include "EditorInstance.h"
#include "Event_Struct.h"
#include "Entity.h"
#include "Engine_Helper.h"

#include <cstring> // strncpy_s

using namespace Editor;

CHierarchyWin::CHierarchyWin()
	: CEditor_Win("Hierarchy")
{
	m_filterBuf[0] = '\0';
	m_renameBuf[0] = '\0';
}

CHierarchyWin::~CHierarchyWin() {}

HRESULT CHierarchyWin::Initialize()
{
	return S_OK;
}

void CHierarchyWin::Update(float fTimeDelta) {}

void CHierarchyWin::Render()
{
	ImGui::Begin("Hierarchy");

	//  (3) 검색(Filter)
	ImGui::InputTextWithHint("##HierarchyFilter", "Search...", m_filterBuf, sizeof(m_filterBuf));
	ImGui::Separator();

	UINT iCurrentLevel = CGameInstance::GetInstance()->Get_Current_LevelIdx();
	auto pGameObjects = CGameInstance::GetInstance()->Get_GameObjects(iCurrentLevel);

	if (pGameObjects.empty())
	{
		ImGui::TextDisabled("(Empty)");

	}
	else
	{
		for (auto& pair : pGameObjects)
		{
			_string strLayerTag = W2S(pair.first);

			if (ImGui::TreeNodeEx(strLayerTag.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto vecObjects = pair.second.get()->Get_GameObject();

				for (auto& pObj : vecObjects)
				{
					if (!pObj)
						continue;

					if (!PassFilter(pObj))
						continue;

					DrawObjectRow(pObj);
				}

				ImGui::TreePop();
			}
		}
	}


	ImGui::Separator();

	// 1. Active UI Layers (현재 화면에 배치된 UI)
	if (ImGui::TreeNodeEx("Active UI Layers", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto pUIManager = CGameInstance::GetInstance()->Get_UI_Manager();

		// UI_LAYER::END 만큼 반복하며 각 레이어 검사
		for (int i = 0; i < (int)EUILayer::END; ++i)
		{
			// 레이어 이름 (예: BACKGROUND, DEFAULT, POPUP 등)
			string layerName = (string)magic_enum::enum_name<EUILayer>((EUILayer)i);
			// TIP: UI_LAYER 이름을 문자열로 반환하는 함수가 있다면 그걸 사용하세요.

			if (ImGui::TreeNode(layerName.c_str()))
			{
				const auto& uiList = pUIManager->GetUIList((EUILayer)i); // 해당 레이어의 vector<shared_ptr<CUI>>

				if (uiList.empty()) {
					ImGui::TextDisabled("(Empty)");
				}
				else {
					for (auto& pUI : uiList)
					{
						if (!pUI) continue;

						// ImGui ID 충돌 방지를 위해 고유 주소 사용
						string label = W2S(pUI->Get_Name()) + "##" + std::to_string((uint64_t)pUI.get());

						ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
						if (m_selectedObject == pUI) flags |= ImGuiTreeNodeFlags_Selected;

						const auto& children = pUI->Get_mapChildren();
						if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

						bool opened = ImGui::TreeNodeEx(label.c_str(), flags);

						// 클릭 시 선택 이벤트 발생
						if (ImGui::IsItemClicked())
						{
							m_selectedObject = pUI;
							EvtSelectEntity ev{};
							ev.entity = static_pointer_cast<Engine::CEntity>(pUI);
							CGameInstance::GetInstance()->Get_EventBus()->Publish(ev);
						}

						// 자식 UI가 있다면 재귀적으로 출력
						if (!children.empty() && opened)
						{
							for (auto& pair : children)
							{
								if (!pair.second.lock()) return;

								_string label = /*W2S(ui->Get_Name());*/W2S(pair.first);
								if (label.empty()) label = "UI_Child";

								_string imguiLabel = label + "##UI" + std::to_string((uint64_t)pair.second.lock().get());
								DrawUITree(pair.second.lock(), imguiLabel); // 기존에 정의하신 재귀 함수 호출
							}

							ImGui::TreePop();
						}
					}
				}
				ImGui::TreePop(); // Layer Node Pop
			}
		}
		ImGui::TreePop(); // Active UI Layers Pop
	}
	//  빈 공간 클릭 시 선택 해제
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
		ImGui::IsMouseClicked(0) &&
		!ImGui::IsAnyItemHovered())
	{
		m_selectedObject.reset();
		CGameInstance::GetInstance()->Get_EventBus()->Publish(EvtClearSelection{});
	}
	ImGui::End();
}

void CHierarchyWin::DrawUITree(const shared_ptr<Engine::CUI>& ui, _string imguiLabel)
{
	/*if (!ui) return;

	_string label = W2S(ui->Get_Name());
	if (label.empty()) label = "UI_Child";

	_string imguiLabel = label + "##UI" + std::to_string((uint64_t)ui.get());*/

	const auto& children = ui->Get_mapChildren();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (m_selectedObject == ui) flags |= ImGuiTreeNodeFlags_Selected;

	//  Leaf면 push 안 하게
	if (children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	bool opened = ImGui::TreeNodeEx(imguiLabel.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		m_selectedObject = ui;
		EvtSelectEntity ev{};
		ev.entity = std::static_pointer_cast<Engine::CEntity>(ui);

		CGameInstance::GetInstance()->Get_EventBus()->Publish(ev);

	}

	//  자식이 있을 때만 TreePop
	if (!children.empty() && opened)
	{


		for (auto& pair : children)
		{
			if (!pair.second.lock()) return;

			_string label = /*W2S(ui->Get_Name());*/W2S(pair.first);
			if (label.empty()) label = "UI_Child";

			_string imguiLabel = label + "##UI" + std::to_string((uint64_t)pair.second.lock().get());

			DrawUITree(pair.second.lock(), imguiLabel);
		}

		ImGui::TreePop();
	}
}
bool CHierarchyWin::PassFilter(const shared_ptr<Engine::CGameObject>& pObj) const
{
	if (m_filterBuf[0] == '\0')
		return true;

	// 간단 포함 검색(대소문자 민감). 원하면 tolower로 개선 가능.
	// wstring -> utf8(string) 변환은 Render에서 ConvertW2A 사용하니까 여기선 단순화:
	// (필터 자체는 char*라서 obj name을 A로 만들어 비교)
	//  const 함수라 ConvertW2A가 const가 아니면 컴파일 경고날 수 있는데,
	//   그럼 PassFilter를 const 빼고 쓰거나 ConvertW2A를 const로 바꿔줘.
	return true; // PassFilter는 DrawObjectRow에서 처리해도 됨.
}

void CHierarchyWin::DrawObjectRow(const shared_ptr<Engine::CGameObject>& pObj)
{
	const bool is_selected = (m_selectedObject == pObj);
	const uint64_t id = PtrID(pObj);

	// 이름
	_string label = W2S(pObj->Get_Name());

	//  Filter 처리(여기서 하는 버전 - PassFilter const 문제 회피)
	if (m_filterBuf[0] != '\0')
	{
		if (label.find(m_filterBuf) == _string::npos)
			return;
	}

	//  (4) Active 토글
	//bool active = Engine_GetActive(pObj);
	//ImGui::PushID((int)id);

	//// 체크박스는 “보이지만 ID는 분리”
	//if (ImGui::Checkbox("##active", &active))
	//    Engine_SetActive(pObj, active);

   // ImGui::SameLine();

	//  (1) Rename (더블클릭/F2 + InputText)
	const bool renaming = (m_renameTargetPtr == id);

	if (renaming)
	{
		// 최초 진입 프레임: 버퍼 초기화 + 포커스
		if (m_renameBuf[0] == '\0')
		{
			strncpy_s(m_renameBuf, label.c_str(), sizeof(m_renameBuf) - 1);
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::SetNextItemWidth(-FLT_MIN);
		const ImGuiInputTextFlags flags =
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_AutoSelectAll;

		bool enter = ImGui::InputText("##rename", m_renameBuf, sizeof(m_renameBuf), flags);

		// Enter로 확정
		if (enter)
		{
			CommitRename(pObj, m_renameBuf);
			m_renameTargetPtr = 0;
			m_renameBuf[0] = '\0';
		}

		// ESC로 취소
		if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			m_renameTargetPtr = 0;
			m_renameBuf[0] = '\0';
		}

		// 포커스 잃으면 종료(취소 정책)
		if (!ImGui::IsItemActive() && !ImGui::IsItemFocused())
		{
			m_renameTargetPtr = 0;
			m_renameBuf[0] = '\0';
		}
	}
	else
	{
		//  ImGui ID 충돌 방지: "##id" 붙이기 (화면에는 label만 보임)
		_string imguiLabel = label + "##" + std::to_string((uint64_t)pObj.get());

		//if (ImGui::Selectable(imguiLabel.c_str(), is_selected))
		//    m_pSelectedObject = pObj;

		if (ImGui::Selectable(imguiLabel.c_str(), is_selected))
		{
			m_selectedObject = pObj; // Hierarchy 하이라이트 유지용(유지해도 됨)


			EvtSelectEntity ev{};
			ev.entity = std::static_pointer_cast<Engine::CEntity>(pObj);

			CGameInstance::GetInstance()->Get_EventBus()->Publish(ev);

		}

		// 더블클릭 → rename
		if (is_selected && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			m_renameTargetPtr = id;
			m_renameBuf[0] = '\0';
		}

		// F2 → rename
		if (is_selected && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			if (ImGui::IsKeyPressed(ImGuiKey_F2))
			{
				m_renameTargetPtr = id;
				m_renameBuf[0] = '\0';
			}
		}
	}

	//  (2) 우클릭 메뉴(Delete/Duplicate/Rename)
	if (ImGui::BeginPopupContextItem("##obj_ctx"))
	{
		if (ImGui::MenuItem("Rename", "F2"))
		{
			m_renameTargetPtr = id;
			m_renameBuf[0] = '\0';
		}



		if (ImGui::MenuItem("Delete"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				if (m_selectedObject == pObj)
					m_selectedObject.reset();

				//  Inspector 선택도 해제
				CGameInstance::GetInstance()->Get_EventBus()->Publish(EvtClearSelection{});

				Engine_DeleteObject(pObj);

				if (m_renameTargetPtr == id)
				{
					m_renameTargetPtr = 0;
					m_renameBuf[0] = '\0';
				}
			}
		}

		ImGui::EndPopup();
	}

	// ImGui::PopID();
}

uint64_t CHierarchyWin::PtrID(const shared_ptr<Engine::CGameObject>& pObj) const
{
	return (uint64_t)pObj.get(); // 임시: 포인터
}


void CHierarchyWin::CommitRename(const shared_ptr<Engine::CGameObject>& pObj, const char* newNameUtf8)
{
	if (!pObj || !newNameUtf8)
		return;

	// 빈 문자열 방지(원하면 정책 변경)
	if (newNameUtf8[0] == '\0')
		return;

	// utf8 -> wstring
	wstring w = S2W(_string(newNameUtf8));



	pObj->Set_Name(w);
}



shared_ptr<CHierarchyWin> CHierarchyWin::Create()
{
	return make_shared<CHierarchyWin>();
}