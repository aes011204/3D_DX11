#include "UIPanel.h"
#include "UISlot.h"
#include "UITransform.h"
#include "UICanvas.h"
#include "GameInstance.h"

CUIPanel::CUIPanel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUIRenderable(pDevice, pContext)
{
 
}

CUIPanel::CUIPanel(const CUIPanel& prototype)
    : CUIRenderable(prototype)
{
}

HRESULT CUIPanel::OnInit(void* pArg)
{

    Set_Interactive(false);

    // 밖에서 Root Canvas 연결해주기
    UIPANEL_DESC* pDesc = static_cast<UIPANEL_DESC*>(pArg);

    m_LayoutDesc = pDesc->LayoutDesc;
    //m_IsTransparent = pDesc->IsTransparent;
    m_IsFullScreen = pDesc->IsFullScreen;
    m_IsUseLayout = pDesc->IsUseLayout;

    __super::OnInit(pDesc);

   // m_IsUseLayout = pDesc->IsUseLayout;


    if (pDesc->IsFullScreen == true)
    {
        Rect m_CanvasSize = m_pGameInstance.lock()->Get_WinSize();

       // pDesc->vAnchorPoint = Vector2{ 0.5f, 0.5f }; // 중앙 기준
       // pDesc->vPivot = Vector2{ 0.5f, 0.5f }; // 중앙 기준
       // pDesc->vSizeDelta = Vector2{ m_CanvasSize.w, m_CanvasSize.h };
       // pDesc->vAnchoredPos = Vector2{ 0.f, 0.f };
       // pDesc->vScale = Vector2{ 1.f, 1.f };
        
        m_pUITransformCom->SetAnchorPoint(Vector2(0.5f, 0.5f));
        m_pUITransformCom->SetPivot(Vector2(0.5f, 0.5f));
        m_pUITransformCom->SetSizeDelta(Vector2(m_CanvasSize.w, m_CanvasSize.h));
        m_pUITransformCom->SetAnchoredPos(Vector2(0.f, 0.f));
        m_pUITransformCom->SetLocalScale(Vector2(1.f,1.f));
    }

    return S_OK;

}

void CUIPanel::OnActive()
{
    if(m_IsUseLayout == true)
		Layout();

    __super::OnActive();

}

void CUIPanel::OnInActive()
{
    __super::OnInActive();

}

void CUIPanel::OnDisabled()
{
    __super::OnDisabled();

}

void CUIPanel::OnUpdate(const _float& timeDelta)
{
    //if (m_bIsDirtyCom) {
    //    RebindCom();      // "바뀐 것"만 한 번 갱신
    //    m_bIsDirtyCom = false;
    //}



    __super::OnUpdate(timeDelta);
}

void CUIPanel::OnLateUpdate()
{
    __super::OnLateUpdate();
}

HRESULT CUIPanel::OnRender()
{

   /* if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;*/

    return __super::OnRender();

}

void CUIPanel::OnClear()
{
    __super::OnClear();

}

//HRESULT CUIPanel::Ready_Components(_uint Level, _wstring protoName)
//{
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
//        return E_FAIL;
//    if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
//        return E_FAIL;
//
//
//    if (FAILED(Add_Component(Level, protoName, TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
//        return E_FAIL;
//           if (m_pTextureCom && m_pUITransformCom) {
//           
//               m_pUITransformCom->SetSizeDelta(m_pTextureCom->Get_SizeFromSRV(0));
//           }
//    return S_OK;
//}
//
//HRESULT CUIPanel::Bind_ShaderResources()
//{
//    if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//        return E_FAIL;
//
//    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
//        return E_FAIL;
//
//    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
//        return E_FAIL;
//
//    if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
//        return E_FAIL;
//
//  /*  if (m_PassIndex == 1)
//    {
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_TexSize", &m_SliceDesc.TexSize, sizeof(_float2))))
//            return E_FAIL;
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &m_SliceDesc.UISize, sizeof(_float2))))
//            return E_FAIL;
//        if (FAILED(m_pShaderCom->Bind_RawValue("g_PxSliceLRTB", &m_SliceDesc.PxSliceLRTB, sizeof(_float4))))
//            return E_FAIL;
//    }*/
//
//    return S_OK;
//}

//void CUIPanel::RebindCom()
//{
//    // 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
//    m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
//    m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
//    m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
//}

void CUIPanel::Save_ToJson(nlohmann::json& j)
{
    __super::Save_ToJson(j);
    //j["IsTransparent"] = m_IsTransparent;
    j["IsFullScreen"] = m_IsFullScreen;
    j["IsUseLayout"] = m_IsUseLayout;
    if (m_IsUseLayout == true)
    {
        if (m_IsUseLayout)
        {
            nlohmann::json jLayout;
            jLayout["Col"] = m_LayoutDesc.m_Col;
            jLayout["Raw"] = m_LayoutDesc.m_Row;
            jLayout["SlotSize"] = m_LayoutDesc.m_SlotSize;
            jLayout["Padding"] = { m_LayoutDesc.m_Padding.x,m_LayoutDesc.m_Padding.y };
            jLayout["Spacing"] = { m_LayoutDesc.m_Spacing.x, m_LayoutDesc.m_Spacing.y };
            jLayout["Offset"] = { m_LayoutDesc.m_Offset.x, m_LayoutDesc.m_Offset.y };

            j["LayoutDesc"] = jLayout;
        }
    }
}

void CUIPanel::Load_FromJson(nlohmann::json& j)
{
    __super::Load_FromJson(j);
    //if (j.contains("IsTransparent"))
    //{
    //    m_IsTransparent = j["IsTransparent"];
    //}
    if (j.contains("IsFullScreen"))
    {
        m_IsFullScreen = j["IsFullScreen"];
    }
   /* if (j.contains("IsUseLayout"))
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
            m_LayoutDesc.m_Spacing.x = jLayout["Spacing"][0];
            m_LayoutDesc.m_Spacing.y = jLayout["Spacing"][1];
        }
        if (jLayout.contains("Offset"))
        {
            m_LayoutDesc.m_Offset.x = jLayout["Offset"][0];
            m_LayoutDesc.m_Offset.y = jLayout["Offset"][1];
        }
        Layout();*/
    //}




}

void CUIPanel::OnGui()
{
    ImGui::Checkbox("Use Layout", &m_IsUseLayout);
    if (m_IsUseLayout == true)
    {
        // 행과 열 (Step 버튼으로 조절 가능하게)
        ImGui::InputInt("Rows", &m_LayoutDesc.m_Row);
        ImGui::InputInt("Columns", &m_LayoutDesc.m_Col);

        ImGui::Separator(); // 구분선

        //// 슬롯 크기 (보통 가로세로 비율이 같으므로 float 하나로)
        //ImGui::DragFloat("Slot Size", &m_LayoutDesc.m_SlotSize, 0.5f, 1.0f, 500.0f, "%.1f");

        // 간격 (Spacing)
        ImGui::DragFloat2("Spacing", (float*)&m_LayoutDesc.m_Spacing, 0.5f, 0.0f, 100.0f, "%.1f");

        // 패딩 (Padding - 전체 영역 내부 여백)
        ImGui::DragFloat2("Padding", (float*)&m_LayoutDesc.m_Padding, 0.5f, 0.0f, 100.0f, "%.1f");

        // 오프셋 (Offset - 시작 위치 조정)
        ImGui::DragFloat2("Offset", (float*)&m_LayoutDesc.m_Offset, 0.5f, -1000.0f, 1000.0f, "%.1f");

        if (ImGui::Button("Apply Layout")) {
            // 여기에 슬롯들의 위치를 재계산하는 Arrange_Slots() 같은 함수 호출
            Layout();
        }
    }
    __super::OnGui();
}

shared_ptr<CUIPanel> CUIPanel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUIPanel> pInstance(new CUIPanel(pDevice, pContext), [](CUIPanel* p) {p->Free(); delete(p); });

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIPanel");
        return nullptr;
    }
    return pInstance;

}

void CUIPanel::Free()
{

    __super::Free();
}


void CUIPanel::Layout()
{

    //// 일단 left top
    //for (auto& child : m_Children)
    //{
    //    if (child->IsLayoutTarget() != true)
    //        return;

    //    int Raw = index % m_LayoutDesc.m_Col;
    //    int Col = index / m_LayoutDesc.m_Col;

    //    float posX = m_LayoutDesc.m_Padding.x + (slotSize + m_LayoutDesc.m_Spacing.x) * Raw;
    //    float posY = m_LayoutDesc.m_Padding.y + (slotSize + m_LayoutDesc.m_Spacing.y) * Col;

    //    child->GetUITransform()->SetAnchoredPos({ posX, posY });

    //    // child 가 슬랏일 경우 인덱스 를 넘긴다
    //    dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });


    //    index++;
    //}

    // 중점  ///////////////////////////////////////////////////////////////
    int index = 0;
    //LOG_F(LOG_LEVEL::INFO, "--- Layout Debug ---");
    //LOG_F(LOG_LEVEL::INFO, "Desc Raw/Col: %d, %d", m_LayoutDesc.m_Raw, m_LayoutDesc.m_Col);
    //LOG_F(LOG_LEVEL::INFO, "TotalSize: %f, %f", totalWidth, totalHeight);
    //LOG_F(LOG_LEVEL::INFO, "StartPos: %f, %f", startPos.x, startPos.y);
    //startPos.x += m_LayoutDesc.m_Offset.x;
    //startPos.y += m_LayoutDesc.m_Offset.y;

    

    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true|| child->Get_UIState()==UI_STATE::INACTIVE)
            continue;

        _float2 slotSize = child->GetUITransform()->Get_FinalSize();
    float totalWidth = (m_LayoutDesc.m_Col * slotSize.x) + ((m_LayoutDesc.m_Col - 1) * m_LayoutDesc.m_Spacing.x);
    float totalHeight = (m_LayoutDesc.m_Row * slotSize.y) + ((m_LayoutDesc.m_Row - 1) * m_LayoutDesc.m_Spacing.y);

    _float2 startPos = { -(totalWidth / 2) + (slotSize.x / 2)+m_LayoutDesc.m_Offset.x, (totalHeight / 2) - (slotSize.y / 2)+ m_LayoutDesc.m_Offset.y };


        int Col = index % m_LayoutDesc.m_Col;
        int Raw = index / m_LayoutDesc.m_Col;

        float posX = startPos.x + (slotSize.x + m_LayoutDesc.m_Spacing.x) * Col;
        float posY = startPos.y - (slotSize.y + m_LayoutDesc.m_Spacing.y) * Raw;

        LOG_F(LOG_LEVEL::INFO, "First Slot AnchoredPos: %f, %f", posX, posY); 
        child->GetUITransform()->SetAnchoredPos({ posX, posY });


      //  shared_ptr<CUISlot> tmp = dynamic_pointer_cast<CUISlot>(child);
      //  if(tmp)
      //      tmp->SetGridIndex({ (float)Col, (float)Raw });

        index++;

    }

}

HRESULT CUIPanel::Add_Layout_Child(shared_ptr<CUI> child, _wstring UITag, _bool KeepWorldRect/*, _uint Group*/)
{
    
    Add_Child(child, UITag, KeepWorldRect);

    child->Set_LayoutTarget(true);

    return S_OK;
}

