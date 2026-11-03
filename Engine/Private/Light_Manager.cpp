#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

CLight_Manager::~CLight_Manager()
{
    Free();
}

const LightDesc* CLight_Manager::Get_LightDesc(_uint iIndex)
{
    auto iter = m_lights.begin();

    for (_uint i = 0; i < iIndex; i++)
    {
        ++iter;
    }

    return  (*iter)->Get_LightDesc();
}

 void CLight_Manager::Set_LightDesc(_uint iIndex, LightDesc Desc)
{
    auto iter = m_lights.begin();

    for (_uint i = 0; i < iIndex; i++)
    {
        ++iter;
    }

    return  (*iter)->Set_LightDesc(Desc);
}

shared_ptr<CLight> CLight_Manager::Add_Light(const LightDesc& lightDesc)
{
   shared_ptr<CLight>  pInstance = CLight::Create(lightDesc);
   if (pInstance == nullptr)
       return nullptr;

   m_lights.push_back(pInstance);
    return pInstance;
}

HRESULT CLight_Manager::Render_Light(shared_ptr<CShader> pShared, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
    for(auto& pLight :m_lights)
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


    int count = m_lights.size();

    for (int i = 0; i < count; ++i)
    {
        auto desc = *Get_LightDesc(i);

        ImGui::PushID(i);

        ImGui::Text("Light %d", i);

        int type = (int)desc.type;
        ImGui::Combo("Type", &type, "Directional\0Point\0");
        desc.type = (ELightType)type;

        if (desc.type == ELightType::POINT)
        {
            ImGui::DragFloat3("Position", (float*)&desc.position, 0.1f);
            ImGui::DragFloat("Range", &desc.range, 0.1f);
        }
        else
        {
            ImGui::DragFloat3("Direction", (float*)&desc.direction, 0.1f);
        }



        ImGui::ColorEdit3("Diffuse", (float*)&desc.diffuse);
        ImGui::ColorEdit3("Ambient", (float*)&desc.ambient);

        Set_LightDesc(i, desc);

        ImGui::Separator();
        ImGui::PopID();
    }

}

void CLight_Manager::clear_light()
{
    if(!m_lights.empty())
    m_lights.clear();

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
