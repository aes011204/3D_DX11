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

    UIButtonDesc* pDesc = static_cast<UIButtonDesc*>(pArg);
    m_clickEvent = pDesc->ClickEvent;
    m_overlapStartEvent = pDesc->OverlapStartEvent;
    m_overlapEndEvent = pDesc->OverlapEndEvent;
    m_typeIndex = pDesc->typeIndex;
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

    if (m_UIState == EButtonState::HOVER || m_UIState == EButtonState::CLICK)
    {
        if (m_overlapEndEvent)
            m_overlapEndEvent(this);
    }

    m_UIState = EButtonState::NORMAL;
    m_isClickInside = false;

    __super::OnInActive();

  
}

void CUIButton::OnDisabled()
{
    m_UIState = EButtonState::DISABLE;
    __super::OnDisabled();

}

void CUIButton::OnUpdate(const _float& timeDelta)
{
    _bool bUseDark = Get_IsDark();

    ProcessInput();
  /*  if(!m_bUseSelect)
    {

        if(m_UIState == BUTTON_STATE::SELECT)
        {
            m_UIState = BUTTON_STATE::HOVER;
        }
    }*/

    if (m_isButtonState == true)
    {
        switch (m_UIState)
        {
        case EButtonState::CLICK:
            if (bUseDark)
                Set_Dark01(0.f);

            break;

        case EButtonState::SELECT:
            if (bUseDark)
                Set_Dark01(0.f);
        
            break;
        case EButtonState::HOVER:
            if (bUseDark)
                Set_Dark01(0.6f);

            break;
        case EButtonState::NORMAL:
            if (bUseDark)
                Set_Dark01(0.8f);
            break;
        case EButtonState::DISABLE: // 상점등에서 보이는데 돈이 없어서 클릭 할수 없는 상태
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

    bool mouseDown = m_gameInstance.lock()->Get_DInput_Manger()->MouseDown(EMouseButton::LBUTTON);
    bool mouseUp = m_gameInstance.lock()->Get_DInput_Manger()->MouseUp(EMouseButton::LBUTTON);

    
        if (m_UIState == EButtonState::DISABLE)
        {
            return;
        }
     /*   if (m_bHovered && mouseUp)
        {
            if (m_ClickEvent)
                m_ClickEvent(this);
        }*/

    //CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO, "CurState %d", ETOI(m_UIState));

    if (m_isHovered) // 젤최상위에서 처리
    {
        if (mouseUp && m_isClickInside) // 안에서 클릭한 상태에서 안에서 뗏는지
        {
            m_isClickInside = false;
            if (m_useSelect)
            {
                m_UIState = EButtonState::SELECT;
	            
            }
            else
            {
                if (m_overlapEndEvent) // 콜백 실행
                    m_overlapEndEvent(this);
	            
                m_UIState = EButtonState::NORMAL; // 또는 NORMAL
            }

            if (m_clickEvent) // 콜백 실행
            {
                m_clickEvent(this);

            }
          
            
            return;
        }
        if (mouseDown&&m_UIState != EButtonState::CLICK&& m_UIState != EButtonState::SELECT) // 방금 클릭
        {
            m_isClickInside = true;
            m_UIState = EButtonState::CLICK;
            return;
        }
        if (!mouseDown && !mouseUp && m_UIState == EButtonState::NORMAL /* && m_UIState != BUTTON_STATE::HOVER && m_UIState != BUTTON_STATE::SELECT*/) // 호버링 중인가
        {
            m_UIState = EButtonState::HOVER;

        // CLog_Manager::GetInstance()->Add_Log(LOG_LEVEL::INFO, "Exited at");
            if (m_overlapStartEvent) // 콜백 실행
            {
                m_overlapStartEvent(this);
            }
            return;
        }
    }
    else
    {

        if (m_UIState != EButtonState::NORMAL && m_UIState != EButtonState::SELECT)
        {
            if (mouseUp)
            {
                m_isClickInside = false;

               
            }

            m_UIState = EButtonState::NORMAL;
           
            if (m_overlapEndEvent) // 콜백 실행
            {
                m_overlapEndEvent(this);
            }

        }
    }


}

void CUIButton::ChangeState(EButtonState next)
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
    return m_typeIndex; 
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
