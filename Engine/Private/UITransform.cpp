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
    m_Dirty = false;

    for (auto& children : m_Children)
    {
        if (children)
            children->UpdateLayoutIfDirty();
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

        // 결과값 확인 (Read Only) - 현재 계산된 최종 월드 좌표를 보여줌
        ImGui::TextDisabled("World Rect Info");
        Rect world = GetWorldRect(); // UpdateLayoutIfDirty가 내부에서 호출됨
        ImGui::Text("LT: (%.1f, %.1f)", world.x, world.y);
        ImGui::Text("Size: (%.1f, %.1f)", world.w - world.x, world.h - world.y);

        // 값이 하나라도 바뀌었다면 Dirty 플래그를 세워 모든 자식까지 갱신되게 함
        if (bChanged)
        {
            MarkDirtyRecursive();
        }
    

        // 행렬의 내용을 4x4 표 형태로 출력
        if (ImGui::BeginTable("MatrixTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (int i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (int j = 0; j < 4; ++j)
                {
                    ImGui::TableSetColumnIndex(j);
                    // m_WorldMatrix.m[행][열] 데이터 출력
                    float val = m_WorldMatrix.m[i][j];

                    // 값이 너무 작거나 nan이면 빨간색으로 표시 (디버깅 꿀팁)
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
        // 이미 바뀐 상태면 자식도 다시 마킹 // 방어적 설계
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
        return m_Parent.lock()->GetWorldRect();
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

    float renderY = m_pGameInstance.lock()->Get_WinSize().h - pivotWorld.y;

    // VIBuffer_Rect가 (-0.5 ~ 0.5) 중심 기준이라는 전제
    // pivot 보정
    Vector2 pivotOffset = {
        (0.5f - m_Pivot.x) * size.x,
        (m_Pivot.y - 0.5f ) * size.y
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
