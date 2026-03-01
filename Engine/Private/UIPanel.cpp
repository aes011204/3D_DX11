#include "UIPanel.h"
#include "UISlot.h"
#include "UITransform.h"
#include "UICanvas.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Shader.h"

CUIPanel::CUIPanel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{

}

CUIPanel::CUIPanel(const CUIPanel& prototype)
    : CUI(prototype)
{
}

HRESULT CUIPanel::OnInit(void* pArg)
{
    // 밖에서 Root Canvas 연결해주기
    UIPANEL_DESC* pDesc = static_cast<UIPANEL_DESC*>(pArg);

    m_LayoutDesc = pDesc->LayoutDesc;
    m_IsTransparent = pDesc->IsTrnasparent;
    m_IsFullScreen = pDesc->IsFullScreen;
    m_IsUseLayout = pDesc->IsUseLayout;

    if (m_IsTransparent==false)
    {
    if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
        return E_FAIL;

    }

   // m_IsUseLayout = pDesc->IsUseLayout;


    if (pDesc->IsFullScreen == true)
    {
        Rect m_CanvasSize = m_pGameInstance.lock()->Get_WinSize();

        //pDesc->vAnchorPoint = { 0.5f, 0.5f }; // 중앙 기준
        //pDesc->vPivot = { 0.5f, 0.5f }; // 중앙 기준
        //pDesc->vSizeDelta = { m_CanvasSize.w, m_CanvasSize.h };
        //pDesc->vAnchoredPos = { 0.f, 0.f };
        //pDesc->vScale = { 1.f, 1.f };
        
        m_pUITransformCom->SetAnchorPoint(Vector2(0.5f, 0.5f));
        m_pUITransformCom->SetPivot(Vector2(0.5f, 0.5f));
        m_pUITransformCom->SetSizeDelta(Vector2(m_CanvasSize.w, m_CanvasSize.h));
        m_pUITransformCom->SetAnchoredPos(Vector2(0.f, 0.f));
        m_pUITransformCom->SetLocalScale(Vector2(1.f,1.f));
    }

    // 이건 부모가 CUI라서 할필요 없는데 그냥 ㄱㄱ
    __super::OnInit(pDesc);

    return S_OK;
}

void CUIPanel::OnActive()
{
}

void CUIPanel::OnInActive()
{
}

void CUIPanel::OnDisabled()
{
}

void CUIPanel::OnUpdate(const _float& timeDelta)
{
    if (m_bIsDirtyCom) {
        RebindCom();      // "바뀐 것"만 한 번 갱신
        m_bIsDirtyCom = false;
    }
}

void CUIPanel::OnLateUpdate()
{
}

HRESULT CUIPanel::OnRender()
{
    if (m_IsTransparent == true)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CUIPanel::OnClear()
{
}

HRESULT CUIPanel::Ready_Components(_uint Level, _wstring protoName)
{
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
        return E_FAIL;
    if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIPanel::Bind_ShaderResources()
{
    if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

  /*  if (m_PassIndex == 1)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_TexSize", &m_SliceDesc.TexSize, sizeof(_float2))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &m_SliceDesc.UISize, sizeof(_float2))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_PxSliceLRTB", &m_SliceDesc.PxSliceLRTB, sizeof(_float4))))
            return E_FAIL;
    }*/

    return S_OK;
}

void CUIPanel::RebindCom()
{
    // 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
    m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
    m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
    m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}

void CUIPanel::Save_ToJson(nlohmann::json& j)
{
    j["IsTransparent"] = m_IsTransparent;
    j["IsFullScreen"] = m_IsFullScreen;
    j["IsUseLayout"] = m_IsUseLayout;
    if (m_IsUseLayout == true)
    {
        if (m_IsUseLayout)
        {
            nlohmann::json jLayout;
            jLayout["Col"] = m_LayoutDesc.m_Col;
            jLayout["Raw"] = m_LayoutDesc.m_Raw;
            jLayout["SlotSize"] = { m_LayoutDesc.m_SlotSize};
            jLayout["Padding"] = { m_LayoutDesc.m_Padding.x,m_LayoutDesc.m_Padding.y };
            jLayout["Spacing"] = { m_LayoutDesc.m_Spacing.x, m_LayoutDesc.m_Spacing.y };

            j["LayoutDesc"] = jLayout;
        }
    }
    __super::Save_ToJson(j);
}

void CUIPanel::Load_FromJson(nlohmann::json& j)
{
    if (j.contains("IsTransparent"))
    {
        m_IsTransparent = j["IsTransparent"];
    }
    if (j.contains("IsFullScreen"))
    {
        m_IsFullScreen = j["IsFullScreen"];
    }
    if (j.contains("IsUseLayout"))
    {
        m_IsUseLayout = j["IsUseLayout"];
    }
    if (j.contains("LayoutDesc"))
    {
        auto& jLayout = j["LayoutDesc"];
        m_LayoutDesc.m_Col = jLayout["Col"];
        m_LayoutDesc.m_Raw = jLayout["Raw"];
        m_LayoutDesc.m_SlotSize = jLayout["SlotSize"];

        if (jLayout.contains("Padding"))
        {
            m_LayoutDesc.m_Padding.x = jLayout["Padding"][0];
            m_LayoutDesc.m_Padding.y = jLayout["Padding"][1];
        }
        if (jLayout.contains("Spacing"))
        {
            m_LayoutDesc.m_Spacing.x = jLayout["PadSpacingSpacingding"][0];
            m_LayoutDesc.m_Spacing.y = jLayout["Spacing"][1];
        }
    }

    __super::Load_FromJson(j);



}

void CUIPanel::OnGui()
{
}

void CUIPanel::Free()
{

    __super::Free();
}


void CUIPanel::Layout()
{

    // 일단 left top
    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true)
            return;

        int Raw = index % m_LayoutDesc.m_Col;
        int Col = index / m_LayoutDesc.m_Col;

        float posX = m_LayoutDesc.m_Padding.x + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.x) * Raw;
        float posY = m_LayoutDesc.m_Padding.y + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.y) * Col;

        child->GetUITransform()->SetAnchoredPos({ posX, posY });

        // child 가 슬랏일 경우 인덱스 를 넘긴다
        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });


        index++;
    }

    // 중점  ///////////////////////////////////////////////////////////////

    float totalWidth = (m_LayoutDesc.m_Raw * m_LayoutDesc.m_SlotSize) + ((m_LayoutDesc.m_Raw - 1) * m_LayoutDesc.m_Spacing.x);
    float totalHeight = (m_LayoutDesc.m_Raw * m_LayoutDesc.m_SlotSize) + ((m_LayoutDesc.m_Col - 1) * m_LayoutDesc.m_Spacing.y);

    _float2 startPos = { -(totalWidth / 2) - (m_LayoutDesc.m_SlotSize / 2), (totalHeight / 2) + (m_LayoutDesc.m_SlotSize / 2) };



    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true)
            return;

        int Raw = index % m_LayoutDesc.m_Col;
        int Col = index / m_LayoutDesc.m_Col;

        float posX = startPos.x + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.x) * Raw;
        float posY = startPos.y + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.y) * Col;

        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });

        index++;

    }

}

void CUIPanel::Add_Layout_Child()
{
}

