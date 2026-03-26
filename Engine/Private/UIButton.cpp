#include "UIButton.h"
#include "GameInstance.h"
#include "DInput_Manager.h"


CUIButton::CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIRenderable(pDevice, pContext)
{
}

CUIButton::CUIButton(const CUIButton& prototype)
	: CUIRenderable(prototype)
{
}

HRESULT CUIButton::OnInit(void* pArg)
{

    UIBUTTON_DESC* pDesc = static_cast<UIBUTTON_DESC*>(pArg);
    m_ClickEvent = pDesc->ClickEvent;
    m_OverlapStartEvent = pDesc->OverlapStartEvent;
    m_OverlapEndEvent = pDesc->OverlapEndEvent;
    m_TypeIndex = pDesc->TypeIndex;
   // m_TextureComLevel = pDesc->TextureComLevel;


   /* if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
        return E_FAIL;

    if (m_pTextureCom)
    {
        Vector2 vTexSize = m_pTextureCom->Get_SizeFromSRV(0);
        m_pUITransformCom->SetSizeDelta(vTexSize);
    }*/

    

    return __super::OnInit(pDesc);;

}

void CUIButton::OnActive()
{
    __super::OnActive();

}

void CUIButton::OnInActive()
{
    m_UIState = BUTTON_STATE::NORMAL;
    __super::OnInActive();
}

void CUIButton::OnDisabled()
{
    m_UIState = BUTTON_STATE::DISABLE;
    __super::OnInActive();

}

void CUIButton::OnUpdate(const _float& timeDelta)
{
    _bool bUseDark = Get_IsDark();

    ProcessInput();


    if(m_bButtonState== true)
    {
    switch (m_UIState)
    {
    case BUTTON_STATE::CLICK:
        if (bUseDark)
        Set_Dark01 ( 0.f);

        break;
    case BUTTON_STATE::SELECT:
        if (bUseDark)
            Set_Dark01(0.f);

        break;
    case BUTTON_STATE::HOVER:
        if (bUseDark)
            Set_Dark01(0.6f);

        break;
    case BUTTON_STATE::NORMAL:
        if (bUseDark)
            Set_Dark01(0.8f);
    	break;
    case BUTTON_STATE::DISABLE: // 상점등에서 보이는데 돈이 없어서 클릭 할수 없는 상태
        break;

    }
	    
    }
    __super::OnUpdate(timeDelta);
}

void CUIButton::OnLateUpdate()
{

    __super::OnLateUpdate();
}

HRESULT CUIButton::OnRender()
{
   /* if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
  
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    
    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;*/

   
    return __super::OnRender();

}

void CUIButton::OnClear()
{
    __super::OnClear();

}

void CUIButton::ProcessInput()
{
   /* m_bHovered = false;

    Vector2 mousePos = m_pGameInstance.lock()->Get_DInput_Manger()->Get_MousePos();
    if (true == m_pUITransformCom->Get_WorldRect().Contains(mousePos.x, mousePos.y))
    {
        m_bHovered = true;
    }*/

    bool mouseDown = m_pGameInstance.lock()->Get_DInput_Manger()->MouseDown(DIMB::LBUTTON);
    bool mouseUp = m_pGameInstance.lock()->Get_DInput_Manger()->MouseUp(DIMB::LBUTTON);

    
        if (m_UIState == BUTTON_STATE::DISABLE)
        {
            return;
        }


    //CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO, "CurState %d", ETOI(m_UIState));

    if (m_bHovered) // 젤최상위에서 처리
    {
        if (mouseDown&& m_UIState != BUTTON_STATE::CLICK&& m_UIState != BUTTON_STATE::SELECT) // 방금 클릭
        {
            m_ClickInside = true;
            m_UIState = BUTTON_STATE::CLICK;
            return;
        }
        if (mouseUp && m_ClickInside/*&& m_UIState == BUTTON_STATE::CLICK*/) // 안에서 클릭한 상태에서 안에서 뗏는지
        {
            m_ClickInside = false;
            m_UIState = BUTTON_STATE::SELECT;

            if (m_ClickEvent) // 콜백 실행
            {
                m_ClickEvent(this);
            }
            return;
        }
        if (!mouseDown && !mouseUp&& m_UIState != BUTTON_STATE::HOVER && m_UIState != BUTTON_STATE::SELECT) // 호버링 중인가
        {
            m_UIState = BUTTON_STATE::HOVER;

        // CLog_Manager::GetInstance()->Add_Log(LOG_LEVEL::INFO, "Exited at");
            if (m_OverlapStartEvent) // 콜백 실행
            {
                m_OverlapStartEvent(this);
            }
            return;
        }
    }
    else
    {

        if (m_UIState != BUTTON_STATE::NORMAL && m_UIState != BUTTON_STATE::SELECT)
        {
            if (mouseUp)
                m_ClickInside = false;

            m_UIState = BUTTON_STATE::NORMAL;

            if (m_OverlapEndEvent) // 콜백 실행
            {
                m_OverlapEndEvent(this);
            }

        }
    }


}

void CUIButton::ChangeState(BUTTON_STATE next)
{
    m_UIState = next;
}

//HRESULT CUIButton::Ready_Components(_uint Level, _wstring protoName)
//{
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
//        return E_FAIL;
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
//        return E_FAIL;
//    if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"),&m_pTextureCom, nullptr)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//void CUIButton::RebindCom()
//{
//    // 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
//    m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
//    m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
//    m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
//}


void CUIButton::Save_ToJson(nlohmann::json& j)
{


    __super::Save_ToJson(j);

}

void CUIButton::Load_FromJson(nlohmann::json& j)
{
    __super::Load_FromJson(j);
}

_uint CUIButton::Get_TypeIndex()
{
    return m_TypeIndex; 
}


shared_ptr<CUIButton> CUIButton::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUIButton> pInstance (new CUIButton(pDevice, pContext), [](CUIButton* p) {p->Free();delete(p);});

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIButton");
    }
    return pInstance;
}

void CUIButton::Free()
{
    __super::Free();
}
