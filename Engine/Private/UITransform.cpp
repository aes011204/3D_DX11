#include "UITransform.h"
#include "GameInstance.h"

CUITransform::CUITransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CUITransform::CUITransform(const CUITransform& prototype)
	: CComponent(prototype)
{
}

HRESULT CUITransform::Initialize_Prototype()
{

 
    return S_OK;
}

HRESULT CUITransform::Initialize(void* Arg)
{
    UITRANSFORM_DESC* pDesc = static_cast<UITRANSFORM_DESC*>(Arg);

    m_AnchorPoint = pDesc->vAnchorPoint;
    m_Pivot = pDesc->vPivot;

    m_SizeDelta = pDesc->vSizeDelta;
    m_AnchoredPos = pDesc->vAnchoredPos;

    m_LocalScale = pDesc->vScale;

    m_bSetParentSize = pDesc->bSetParentSize;

    MarkDirtyRecursive();
    
    return S_OK;
}

void CUITransform::SetParent(weak_ptr<CUITransform> newParent, bool keepWorldRect)
{
    if (m_Parent.lock() == newParent.lock())
        return;

    UpdateLayoutIfDirty();
    Rect oldWorld = m_WorldRect;

    // 기존 부모가 있었으면 기존 부모의 자식 목록에서 자신 제거 하기 
    if (m_Parent.lock())
    {
        vector<shared_ptr<CUITransform>>& sibling = m_Parent.lock()->m_Children;
        for (auto it = sibling.begin(); it != sibling.end(); it++)
        {
            if ((*it) == static_pointer_cast<CUITransform>(shared_from_this()))
            {
                sibling.erase(it);
                break; // 찾아서 지웠으니 더 돌 필요 없음  else it++ 도 안해도 됨
            }
        }
    }

    m_Parent = newParent;

    if (m_Parent.lock())
    {
        m_Parent.lock()->GetChildren().push_back(static_pointer_cast<CUITransform>(shared_from_this()));
        if (!m_Canvas)
            m_Canvas = m_Parent.lock()->m_Canvas;
    }

    //    if (keepWorldRect)
    //    {
    //          // 일단 이기능은 패스
    //    }


    MarkDirtyRecursive();
}

void CUITransform::UpdateLayoutIfDirty()
{
    if (!m_Dirty)
        return;

    Computing_WorldRect();
	        if(m_bSetParentSize==true&& m_Parent.lock() !=nullptr)
	        {
                m_LocalScale = m_Parent.lock()->Get_LocalScale();
                m_SizeDelta = m_Parent.lock()->Get_SizeDelta();

	        }
    m_Dirty = false;

    for (auto& children : m_Children)
    {
        if (children)
        {

            children->UpdateLayoutIfDirty();
        }
    }
}

void CUITransform::OnGui()
{


        bool bChanged = false;

        // Anchored Position
        if (ImGui::DragFloat2("Pos (Anchored)", (float*)&m_AnchoredPos, 0.1f))
            bChanged = true;

        // UI의 크기
        if (ImGui::DragFloat2("Size", (float*)&m_SizeDelta, 0.1f))
            bChanged = true;

        ImGui::Separator();

        // 0.0 ~ 1.0 비율
        if (ImGui::SliderFloat2("Anchor", (float*)&m_AnchorPoint, 0.0f, 1.0f))
            bChanged = true;

        // 0.0 ~ 1.0 비율
        if (ImGui::SliderFloat2("Pivot", (float*)&m_Pivot, 0.0f, 1.0f))
            bChanged = true;

        ImGui::Separator();

        // Local Scale 
        if (ImGui::DragFloat2("Scale", (float*)&m_LocalScale, 0.01f, 0.0f, 10.0f))
            bChanged = true;
      /*  ImGui::Separator();
        if (ImGui::DragFloat("Rotation (Rad)", &m_RotationRadian, 0.01f, -6.28f, 6.28f))
        {
            bChanged = true;
        }

        if (ImGui::DragFloat("Rotation (Deg)", &m_RotationDegreeView, 1.0f, -360.0f, 360.0f))
        {
            m_RotationRadian = XMConvertToRadians(m_RotationDegreeView);
            bChanged = true;
        }*/

        
        ImGui::TextDisabled("World Rect Info");
        Rect world = Get_WorldRect(); 
        ImGui::Text("LT: (%.1f, %.1f)", world.x, world.y);
        ImGui::Text("Size: (%.1f, %.1f)", world.w - world.x, world.h - world.y);


        if (bChanged)
        {
            MarkDirtyRecursive();
        }
    


        if (ImGui::BeginTable("MatrixTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (int i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (int j = 0; j < 4; ++j)
                {
                    ImGui::TableSetColumnIndex(j);

                    float val = m_WorldMatrix.m[i][j];

                    if (isnan(val))
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "nan");
                    else
                        ImGui::Text("%.3f", val);
                }
            }
            ImGui::EndTable();
        }

}

void CUITransform::MarkDirtyRecursive()
{
    if (m_Dirty)
    {
        // 이미 바뀐 상태면 자식도 다시 마킹
        for (auto& children : m_Children)
        {
            if (children)
                children->MarkDirtyRecursive();
        }
    }
    m_Dirty = true;

    for (auto& children : m_Children)
    {
        if (children)
            children->MarkDirtyRecursive();
    }
}

Rect CUITransform::GetParent_WorldRect()
{
    if (m_Parent.lock())
    {
        m_Parent.lock()->UpdateLayoutIfDirty();
        return m_Parent.lock()->Get_WorldRect();
    }

    // 부모가 없을경우 캔버스 기준
    else
        return m_pGameInstance.lock()->Get_WinSize();

    //return Rect{ 0.f, 0.f, 1280.f, 720.f };

    //assert("errer CUITrnasform NO CANVAS");
    //return Rect{};
}

XMMATRIX CUITransform::GetParent_Mat()
{
    if (m_Parent.lock())
    {
        m_Parent.lock()->UpdateLayoutIfDirty();
        return m_Parent.lock()->Get_Mat();
    }
    else
    {
        return XMMatrixIdentity();
    }
}

void CUITransform::Computing_WorldRect()
{
    Rect parentRect = GetParent_WorldRect();
    Vector2 parentPos = parentRect.Pos();
    Vector2 parentSize = parentRect.Size();

    // 앵커 위치 ()
    Vector2 anchorPoint = parentPos + Hadamard(parentSize, m_AnchorPoint);

    // 자신의 크기
    Vector2 size = Hadamard(m_SizeDelta, m_LocalScale);

    //내 UI의 피벗점이 위치할곳 엥커에 오프셋(m_AnchoredPos) 더한거
    Vector2 pivotWorld = anchorPoint + m_AnchoredPos;

   // // 피벗점이 위치할곳에서 내 크기의 피벗 비율만큼 왼쪽 위로 이동 
   // Vector2 topLeft = pivotWorld - Hadamard(m_Pivot, size);

   // m_WorldRect = { topLeft.x, topLeft.y ,topLeft.x+size.x, topLeft.y+size.y };

   // //행렬로 회전이 포함된 상태 저장

   // XMVECTOR rotationMat = XMVectorSet( Hadamard(m_Pivot, size).x, Hadamard(m_Pivot, size).y, 0,0);


   //XMStoreFloat4x4(&m_WorldMatrix,XMMatrixTransformation2D(rotationMat, 0.f, XMVectorSet(1, 1, 1, 1), rotationMat, m_RotationRadian, XMVectorSet(topLeft.x, topLeft.y, 0, 0)));

    // 디버그용 Rect
    Vector2 topLeft = pivotWorld - Hadamard(m_Pivot, size);
    m_WorldRect = { topLeft.x, topLeft.y, topLeft.x + size.x, topLeft.y + size.y };

    float renderY = pivotWorld.y;

    // VIBuffer_Rect가 (-0.5 ~ 0.5) 중심 기준이라는 전제
    // pivot 보정
    Vector2 pivotOffset = {
        (0.5f - m_Pivot.x)* size.x,
        (0.5f - m_Pivot.y)* size.y
    };

    XMMATRIX S = XMMatrixScaling(size.x, size.y, 1.f);
    XMMATRIX R = XMMatrixRotationZ(m_RotationRadian);
    XMMATRIX T = XMMatrixTranslation(
        pivotWorld.x + pivotOffset.x,
        renderY + pivotOffset.y,
        0.f
    );

    //XMMATRIX Par = GetParent_Mat();

    //  핵심
    XMStoreFloat4x4(&m_WorldMatrix, S * R * T );

    
}

Vector2 CUITransform::Hadamard(const Vector2& a, const Vector2& b)
{
    return Vector2{ a.x * b.x, a.y * b.y };
}

void CUITransform::Save_ToJson(nlohmann::json& j)
{

    j["AnchorPoint"] = { m_AnchorPoint.x , m_AnchorPoint.y };
    j["Pivot"] = { m_Pivot.x , m_Pivot.y };
    j["SizeDelta"] = { m_SizeDelta.x , m_SizeDelta.y };
    j["AnchoredPos"] = { m_AnchoredPos.x , m_AnchoredPos.y };
    j["LocalScale"] = { m_LocalScale.x , m_LocalScale.y};
    j["RotationRadian"] = m_RotationRadian;

    j["SetParentSize"] = m_bSetParentSize;
}

void CUITransform::Load_FromJson(nlohmann::json& j)
{
    // 1. AnchorPoint 로드 (x, y)
    if (j.contains("AnchorPoint") && j["AnchorPoint"].is_array())
    {
        m_AnchorPoint.x = j["AnchorPoint"][0];
        m_AnchorPoint.y = j["AnchorPoint"][1];
    }

    // 2. Pivot 로드 (x, y)
    if (j.contains("Pivot") && j["Pivot"].is_array())
    {
        m_Pivot.x = j["Pivot"][0];
        m_Pivot.y = j["Pivot"][1];
    }

    // 3. SizeDelta 로드 (너비, 높이)
    if (j.contains("SizeDelta") && j["SizeDelta"].is_array())
    {
        m_SizeDelta.x = j["SizeDelta"][0];
        m_SizeDelta.y = j["SizeDelta"][1];
    }

    // 4. AnchoredPos 로드 (위치)
    if (j.contains("AnchoredPos") && j["AnchoredPos"].is_array())
    {
        m_AnchoredPos.x = j["AnchoredPos"][0];
        m_AnchoredPos.y = j["AnchoredPos"][1];
    }

    // 5. LocalScale 로드 (배율)
    if (j.contains("LocalScale") && j["LocalScale"].is_array())
    {
        m_LocalScale.x = j["LocalScale"][0];
        m_LocalScale.y = j["LocalScale"][1];
    }
    if (j.contains("RotationRadian"))
    {
        m_RotationRadian= j["RotationRadian"];
       
    }
    
    this->Computing_WorldRect();
}

HRESULT CUITransform::Bind_ShaderResource(shared_ptr<CShader> pShaderCom, const _char* pConstantName)
{
    return 	pShaderCom->Bind_Matrix(pConstantName, &m_WorldMatrix);;
}



shared_ptr<CUITransform> CUITransform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUITransform> pInstance(new CUITransform(pDevice, pContext), [](CUITransform* p) {p->Free();delete(p);});
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("failed prototype: CUITransform");
    }
    return pInstance;

}

shared_ptr<CComponent> CUITransform::Clone(void* pArg)
{
    shared_ptr<CUITransform> pInstance(new CUITransform(*this), [](CUITransform* p) {p->Free();delete(p);});
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("failed clone: CUITransform");
    }
    return pInstance;
}


void CUITransform::Free()
{
	__super::Free();
}
