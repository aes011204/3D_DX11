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

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
   shared_ptr<CLight>  pInstance = CLight::Create(LightDesc);
   if (pInstance == nullptr)
       return E_FAIL;

   m_Lights.push_back(pInstance);
    return S_OK;
}

HRESULT CLight_Manager::Render_Light(shared_ptr<CShader> pShared, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
    for(auto& pLight :m_Lights)
    {
	    if(nullptr != pLight)
	    {
            pLight->Render(pShared, pVIBuffer);
	    }
    }

    return S_OK;

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
