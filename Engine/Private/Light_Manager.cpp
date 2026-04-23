#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

CLight_Manager::~CLight_Manager()
{
    Free();
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
    auto iter = m_Lights.begin();

    for (_uint i = 0; i < iIndex; i++)
    {
        ++iter;
    }

    return  (*iter)->Get_LightDesc();
}

 void CLight_Manager::Set_LightDesc(_uint iIndex, LIGHT_DESC Desc)
{
    auto iter = m_Lights.begin();

    for (_uint i = 0; i < iIndex; i++)
    {
        ++iter;
    }

    return  (*iter)->Set_LightDesc(Desc);
}

shared_ptr<CLight> CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
   shared_ptr<CLight>  pInstance = CLight::Create(LightDesc);
   if (pInstance == nullptr)
       return nullptr;

   m_Lights.push_back(pInstance);
    return pInstance;
}

HRESULT CLight_Manager::Render_Light(shared_ptr<CShader> pShared, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
    for(auto& pLight :m_Lights)
    {
	    if(nullptr != pLight&& pLight->Get_Active() == true)
	    {
            pLight->Render(pShared, pVIBuffer);
	    }
    }

    return S_OK;

}

void CLight_Manager::OnGui()
{
    CBase::OnGui();


    int count = m_Lights.size();

    for (int i = 0; i < count; ++i)
    {
        auto desc = *Get_LightDesc(i);

        ImGui::PushID(i);

        ImGui::Text("Light %d", i);

        int type = (int)desc.eType;
        ImGui::Combo("Type", &type, "Directional\0Point\0");
        desc.eType = (LIGHT)type;

        if (desc.eType == LIGHT::POINT)
        {
            ImGui::DragFloat3("Position", (float*)&desc.vPosition, 0.1f);
            ImGui::DragFloat("Range", &desc.fRange, 0.1f);
        }
        else
        {
            ImGui::DragFloat3("Direction", (float*)&desc.vDirection, 0.1f);
        }



        ImGui::ColorEdit3("Diffuse", (float*)&desc.vDiffuse);
        ImGui::ColorEdit3("Ambient", (float*)&desc.vAmbient);

        Set_LightDesc(i, desc);

        ImGui::Separator();
        ImGui::PopID();
    }

}

void CLight_Manager::clear_light()
{
    if(!m_Lights.empty())
    m_Lights.clear();

}

unique_ptr<CLight_Manager> CLight_Manager::Create()
{
    unique_ptr<CLight_Manager> pInstance(new CLight_Manager());
    return pInstance; // 이니셜라이즈 필요 없음
}

void CLight_Manager::Free()
{
    __super::Free();
}
