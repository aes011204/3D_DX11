#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

CLight_Manager::~CLight_Manager()
{
    Free();
}

const LightDesc* CLight_Manager::Get_LightDesc(_uint index)
{
    auto iter = m_lights.begin();

    for (_uint i = 0; i < index; i++)
    {
        ++iter;
    }

    return  (*iter)->Get_LightDesc();
}

 void CLight_Manager::Set_LightDesc(_uint index, LightDesc desc)
{
    auto iter = m_lights.begin();

    for (_uint i = 0; i < index; i++)
    {
        ++iter;
    }

    return  (*iter)->Set_LightDesc(desc);
}

shared_ptr<CLight> CLight_Manager::Add_Light(const LightDesc& lightDesc)
{
   shared_ptr<CLight>  instance = CLight::Create(lightDesc);
   if (instance == nullptr)
       return nullptr;

   m_lights.push_back(instance);
    return instance;
}

HRESULT CLight_Manager::Render_Light(shared_ptr<CShader> shared, shared_ptr<CVIBuffer_Rect> VIBuffer)
{
    for(auto& light :m_lights)
    {
	    if(nullptr != light&& light->Get_Active() == true)
	    {
            light->Render(shared, VIBuffer);
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
    unique_ptr<CLight_Manager> instance(new CLight_Manager());
    return instance; // 이니셜라이즈 필요 없음
}

void CLight_Manager::Free()
{
    __super::Free();
}
