#include "ManagerWin.h"
#include "GameInstance.h"
#include "Engine_Helper.h"

CManagerWin::CManagerWin() : CEditor_Win("MangerWin")
{
}

CManagerWin::~CManagerWin()
{
}

HRESULT CManagerWin::Initialize()
{
	return S_OK;
}

void CManagerWin::Update(float fTimeDelta)
{
}

void CManagerWin::Render()
{

    ImGui::Begin("MangerWin");

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
    ImGui::End();
}


shared_ptr<CManagerWin> CManagerWin::Create()
{
	return make_shared<CManagerWin>();
}

