#include "UIButton.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
#include "UITransform.h"
#include "Texture.h"
#include "Shader.h"
#include "IModifier.h"
#include "Log_Manager.h"

CUIButton::CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUI(pDevice, pContext)
{
}

CUIButton::CUIButton(const CUIButton& prototype)
	: CUI(prototype)
{
}

HRESULT CUIButton::OnInit(void* pArg)
{

    UIBUTTON_DESC* pDesc = static_cast<UIBUTTON_DESC*>(pArg);
    m_ClickEvent = pDesc->ClickEvent;
    m_OverlapStartEvent = pDesc->OverlapStartEvent;
    m_OverlapEndEvent = pDesc->OverlapEndEvent;
   // m_TextureComLevel = pDesc->TextureComLevel;


    if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
        return E_FAIL;

    if (m_pTextureCom)
    {
        Vector2 vTexSize = m_pTextureCom->Get_SizeFromSRV(0);
        m_pUITransformCom->SetSizeDelta(vTexSize);
    }


    return S_OK;

}

void CUIButton::OnActive()
{
}

void CUIButton::OnInActive()
{
}

void CUIButton::OnDisabled()
{
    m_UIState = BUTTON_STATE::DISABLE;


}

void CUIButton::OnUpdate(const _float& timeDelta)
{
    if (m_bIsDirtyCom) {
        RebindCom();      // "바뀐 것"만 한 번 갱신
        m_bIsDirtyCom = false;
    }

    ProcessInput();

    switch (m_UIState)
    {
    case BUTTON_STATE::CLICK:
        break;
    case BUTTON_STATE::HOVER:
        break;
    case BUTTON_STATE::NONE:
        break;
    case BUTTON_STATE::DISABLE: // 상점등에서 보이는데 돈이 없어서 클릭 할수 없는 상태
        break;

    }
}

void CUIButton::OnLateUpdate()
{


}

HRESULT CUIButton::OnRender()
{
    if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    
    _float4x4 Wm;
    XMStoreFloat4x4(&Wm,
        XMMatrixScaling(200.f, 200.f, 1.f) *  // 200x200 픽셀
        XMMatrixTranslation(200.f, 200.f, 0.f)  // (200,200) 위치
    );


    _float4x4		IdentityMatrix = {};
    XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

    auto win = m_pGameInstance.lock()->Get_WinSize();
    float W = win.w - win.x;
    float H = win.h - win.y;

    _float4x4 P;
    XMStoreFloat4x4(&P, XMMatrixOrthographicOffCenterLH(
        0.f, W,
        H, 0.f,
        0.f, 1.f
    ));


   // m_pShaderCom->Bind_Matrix("g_WorldMatrix", &Wm);
    //m_pShaderCom->Bind_Matrix("g_ViewMatrix", &IdentityMatrix);
    //
    //m_pShaderCom->Bind_Matrix("g_ProjMatrix", &P);


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
        return E_FAIL;

    return S_OK;
}

void CUIButton::OnClear()
{
}

void CUIButton::ProcessInput()
{
   /* m_bHovered = false;

    Vector2 mousePos = m_pGameInstance.lock()->Get_DInput_Manger()->Get_MousePos();
    if (true == m_pUITransformCom->GetWorldRect().Contains(mousePos.x, mousePos.y))
    {
        m_bHovered = true;
    }*/

    bool mouseDown = m_pGameInstance.lock()->Get_DInput_Manger()->MouseDown(DIMB::LBUTTON);
    bool mouseUp = m_pGameInstance.lock()->Get_DInput_Manger()->MouseUp(DIMB::LBUTTON);

    
        if (m_UIState == BUTTON_STATE::DISABLE)
        {
            return;
        }


    //CLog_Manager::GetInstance()->Add_Log_F(CLog_Manager::LOG_LEVEL::INFO, "CurState %d", ETOI(m_UIState));

    if (m_bHovered)
    {
        if (mouseDown&& m_UIState != BUTTON_STATE::CLICK) // 방금 클릭
        {
            m_ClickInside = true;
            m_UIState = BUTTON_STATE::CLICK;
            return;
        }
        if (mouseUp && m_ClickInside/*&& m_UIState == BUTTON_STATE::CLICK*/) // 안에서 클릭한 상태에서 안에서 뗏는지
        {
            m_ClickInside = false;
            m_UIState = BUTTON_STATE::HOVER;

            if (m_ClickEvent) // 콜백 실행
            {
                m_ClickEvent(this);
            }
            return;
        }
        if (!mouseDown && !mouseUp&& m_UIState != BUTTON_STATE::HOVER) // 호버링 중인가
        {
            m_UIState = BUTTON_STATE::HOVER;

        // CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::INFO, "Exited at");
            if (m_OverlapStartEvent) // 콜백 실행
            {
                m_OverlapStartEvent(this);
            }
            return;
        }
    }
    else
    {

        if (m_UIState != BUTTON_STATE::NONE)
        {
            if (mouseUp)
                m_ClickInside = false;

            m_UIState = BUTTON_STATE::NONE;

            if (m_OverlapEndEvent) // 콜백 실행
            {
                m_OverlapEndEvent(this);
            }

        }
    }


}

void CUIButton::ChangeState(BUTTON_STATE next)
{
}

HRESULT CUIButton::Ready_Components(_uint Level, _wstring protoName)
{
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"),&m_pTextureCom, nullptr)))
        return E_FAIL;

    return S_OK;
}

void CUIButton::RebindCom()
{
    // 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
    m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
    m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
    m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}


void CUIButton::Save_ToJson(nlohmann::json& j)
{


    __super::Save_ToJson(j);

}

void CUIButton::Load_FromJson(nlohmann::json& j)
{
    __super::Load_FromJson(j);
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
