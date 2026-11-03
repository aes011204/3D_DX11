#include "UITransform.h"
#include "GameInstance.h"

CUITransform::CUITransform(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CComponent(device, context)
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
    UITransformDesc* desc = static_cast<UITransformDesc*>(Arg);

    m_anchorPoint = desc->anchorPoint;
    m_pivot = desc->pivot;

    m_sizeDelta = desc->sizeDelta;
    m_anchoredPos = desc->anchoredPos;

    m_localScale = desc->scale;

    m_isSetParentSize = desc->isSetParentSize;

    MarkDirtyRecursive();
    
    return S_OK;
}

void CUITransform::SetParent(weak_ptr<CUITransform> newParent, bool keepWorldRect)
{
    if (m_parent.lock() == newParent.lock())
        return;

    UpdateLayoutIfDirty();
    Rect oldWorld = m_worldRect;

    // 기존 부모가 있었으면 기존 부모의 자식 목록에서 자신 제거 하기 
    if (m_parent.lock())
    {
        vector<shared_ptr<CUITransform>>& sibling = m_parent.lock()->m_children;
        for (auto it = sibling.begin(); it != sibling.end(); it++)
        {
            if ((*it) == static_pointer_cast<CUITransform>(shared_from_this()))
            {
                sibling.erase(it);
                break; // 찾아서 지웠으니 더 돌 필요 없음  else it++ 도 안해도 됨
            }
        }
    }

    m_parent = newParent;

    if (m_parent.lock())
    {
        m_parent.lock()->GetChildren().push_back(static_pointer_cast<CUITransform>(shared_from_this()));
        if (!m_canvas)
            m_canvas = m_parent.lock()->m_canvas;
    }

    //    if (keepWorldRect)
    //    {
    //          // 일단 이기능은 패스
    //    }


    MarkDirtyRecursive();
}

_bool CUITransform::SetRotationTo(_float targetDegree, _float timeDelta)
{
    _float ratio = 0.1f; // 보간 속도 (0.0 ~ 1.0)
    _float degree = XMConvertToDegrees(m_rotationRadian);
    if (degree != targetDegree)
    {
        _float Diff = targetDegree - degree;

       
        if (abs(Diff) < 0.1f)
        {
            degree = targetDegree;
        	SetRotation(degree);
            return true;
        }
        else
        {
            degree += Diff * ratio;
        }
    }

   
    SetRotation(degree);
    return false;

}

void CUITransform::UpdateLayoutIfDirty()
{
    if (!m_isDirty)
        return;

    Computing_WorldRect();
	        if(m_isSetParentSize==true&& m_parent.lock() !=nullptr)
	        {
                m_localScale = m_parent.lock()->Get_LocalScale();
                m_sizeDelta = m_parent.lock()->Get_SizeDelta();

	        }
    m_isDirty = false;

    for (auto& children : m_children)
    {
        if (children)
        {

            children->UpdateLayoutIfDirty();
        }
    }
}

void CUITransform::OnGui()
{


        bool isChanged = false;
        bool isPopPivotColor = false;
        bool isPopColor = false;
        if (m_isKeepSizeRatio)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
            isPopColor = true;
        }

        // 2. 토글 버튼
        if (ImGui::Button(m_isKeepSizeRatio ? "Ratio: Locked" : "Ratio: Unlocked", ImVec2(-1, 0)))
        {
            m_isKeepSizeRatio = !m_isKeepSizeRatio;
        }

        // 3. 색상 복구 (Pop) - 반드시 Push한 만큼만 정확히!
        if (isPopColor)
        {
            ImGui::PopStyleColor(3);
        }

        if (m_isFreePivot)
        {
            // 활성화 시 노란색이나 주황색 계열로 강조 (주의 표시)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.2f, 1.0f));
            isPopPivotColor = true;
        }

        if (ImGui::Button(m_isFreePivot ? "Pivot: Free (Unlocked)" : "Pivot: Standard (0-1)"))
        {
            m_isFreePivot = !m_isFreePivot;
        }

        if (isPopPivotColor) ImGui::PopStyleColor(2);

        // Anchored Position
        if (ImGui::DragFloat2("Pos (Anchored)", (float*)&m_anchoredPos, 0.1f))
            isChanged = true;

        //// UI의 크기
        //if (ImGui::DragFloat2("Size", (float*)&m_SizeDelta, 0.1f))
        //    bChanged = true;

        _float2 prevSize = m_sizeDelta; // 이전 사이즈 저장

        if (ImGui::DragFloat2("Size", (float*)&m_sizeDelta, 0.1f))
        {
            isChanged = true;

            // 비율 유지가 켜져 있고, 사이즈가 0이 아닐 때
            if (m_isKeepSizeRatio && prevSize.x != 0.f && prevSize.y != 0.f)
            {
                // 어느 축이 변했는지 확인하여 비율 계산
                if (m_sizeDelta.x != prevSize.x) // X가 변했다면
                {
                    float ratio = prevSize.y / prevSize.x;
                    m_sizeDelta.y = m_sizeDelta.x * ratio;
                }
                else if (m_sizeDelta.y != prevSize.y) // Y가 변했다면
                {
                    float ratio = prevSize.x / prevSize.y;
                    m_sizeDelta.x = m_sizeDelta.y * ratio;
                }
            }
        }
        ImGui::Separator();

        // 0.0 ~ 1.0 비율
        if (ImGui::SliderFloat2("Anchor", (float*)&m_anchorPoint, 0.0f, 1.0f))
            isChanged = true;

        // 0.0 ~ 1.0 비율
        if (m_isFreePivot)
        {
            // 범위 제한 없는 드래그 (0.01f 속도로 미세 조절)
            if (ImGui::DragFloat2("Pivot (Free)", (float*)&m_pivot, 0.01f))
            {
                MarkDirtyRecursive();
                isChanged = true;
            }
        }
        else
        {
            // 표준 0.0 ~ 1.0 슬라이더
            if (ImGui::SliderFloat2("Pivot (Fixed)", (float*)&m_pivot, 0.0f, 1.0f))
            {
                MarkDirtyRecursive();
                isChanged = true;
            }
        }

        ImGui::Separator();

        // Local Scale 
        if (ImGui::DragFloat2("Scale", (float*)&m_localScale, 0.01f, 0.0f, 10.0f))
            isChanged = true;
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
        if (ImGui::DragFloat("Rotation Degree", &m_rotationDegreeView, 0.5f))
        {
            m_rotationRadian = XMConvertToRadians(m_rotationDegreeView);
            MarkDirtyRecursive();
            isChanged = true;
        }
        
        ImGui::TextDisabled("World Rect Info");
        Rect world = Get_WorldRect(); 
        ImGui::Text("LT: (%.1f, %.1f)", world.x, world.y);
        ImGui::Text("Size: (%.1f, %.1f)", world.w - world.x, world.h - world.y);


        if (isChanged)
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

                    float val = m_worldMatrix.m[i][j];

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
    if (m_isDirty)
    {
        // 이미 바뀐 상태면 자식도 다시 마킹
        for (auto& children : m_children)
        {
            if (children)
                children->MarkDirtyRecursive();
        }
    }
    m_isDirty = true;

    for (auto& children : m_children)
    {
        if (children)
            children->MarkDirtyRecursive();
    }
}

Rect CUITransform::GetParent_WorldRect()
{
    if (m_parent.lock())
    {
        m_parent.lock()->UpdateLayoutIfDirty();
        return m_parent.lock()->Get_WorldRect();
    }

    // 부모가 없을경우 캔버스 기준
    else
        return m_gameInstance.lock()->Get_WinSize();

    //return Rect{ 0.f, 0.f, 1280.f, 720.f };

    //assert("errer CUITrnasform NO CANVAS");
    //return Rect{};
}

XMMATRIX CUITransform::GetParent_Mat()
{
    if (m_parent.lock())
    {
        m_parent.lock()->UpdateLayoutIfDirty();
        return m_parent.lock()->Get_Mat();
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
    Vector2 anchorPoint = parentPos + Hadamard(parentSize, m_anchorPoint);

    // 자신의 크기
    Vector2 size = Hadamard(m_sizeDelta, m_localScale);

    //내 UI의 피벗점이 위치할곳 엥커에 오프셋(m_AnchoredPos) 더한거
    Vector2 pivotWorld = anchorPoint + m_anchoredPos;

   // // 피벗점이 위치할곳에서 내 크기의 피벗 비율만큼 왼쪽 위로 이동 
   // Vector2 topLeft = pivotWorld - Hadamard(m_Pivot, size);

   // m_WorldRect = { topLeft.x, topLeft.y ,topLeft.x+size.x, topLeft.y+size.y };

   // //행렬로 회전이 포함된 상태 저장

   // XMVECTOR rotationMat = XMVectorSet( Hadamard(m_Pivot, size).x, Hadamard(m_Pivot, size).y, 0,0);


   //XMStoreFloat4x4(&m_WorldMatrix,XMMatrixTransformation2D(rotationMat, 0.f, XMVectorSet(1, 1, 1, 1), rotationMat, m_RotationRadian, XMVectorSet(topLeft.x, topLeft.y, 0, 0)));

    // 디버그용 Rect
    Vector2 topLeft = pivotWorld - Hadamard(m_pivot, size);
    m_worldRect = { topLeft.x, topLeft.y, topLeft.x + size.x, topLeft.y + size.y };

    float renderY = pivotWorld.y;

    // VIBuffer_Rect가 (-0.5 ~ 0.5) 중심 기준이라는 전제
    // pivot 보정
    Vector2 pivotOffset = {
        (0.5f - m_pivot.x)* size.x,
        (0.5f - m_pivot.y)* size.y
    };

    XMMATRIX S = XMMatrixScaling(size.x, size.y, 1.f);
    XMMATRIX matPivot = XMMatrixTranslation(pivotOffset.x, pivotOffset.y, 0.f);
    XMMATRIX R = XMMatrixRotationZ(m_rotationRadian);
   /* XMMATRIX T = XMMatrixTranslation(
        pivotWorld.x + pivotOffset.x,
        renderY + pivotOffset.y,
        0.f*/
    XMMATRIX T = XMMatrixTranslation(pivotWorld.x, pivotWorld.y, 0.f
    );

    //XMMATRIX Par = GetParent_Mat();

    //  핵심
    XMStoreFloat4x4(&m_worldMatrix, S *matPivot * R * T );

    
}

Vector2 CUITransform::Hadamard(const Vector2& a, const Vector2& b)
{
    return Vector2{ a.x * b.x, a.y * b.y };
}

void CUITransform::Save_ToJson(nlohmann::json& j)
{

    j["AnchorPoint"] = { m_anchorPoint.x , m_anchorPoint.y };
    j["Pivot"] = { m_pivot.x , m_pivot.y };
    j["SizeDelta"] = { m_sizeDelta.x , m_sizeDelta.y };
    j["AnchoredPos"] = { m_anchoredPos.x , m_anchoredPos.y };
    j["LocalScale"] = { m_localScale.x , m_localScale.y};
    j["RotationRadian"] = m_rotationRadian;

    j["SetParentSize"] = m_isSetParentSize;
}

void CUITransform::Load_FromJson(nlohmann::json& j)
{
    // 1. AnchorPoint 로드 (x, y)
    if (j.contains("AnchorPoint") && j["AnchorPoint"].is_array())
    {
        m_anchorPoint.x = j["AnchorPoint"][0];
        m_anchorPoint.y = j["AnchorPoint"][1];
    }

    // 2. Pivot 로드 (x, y)
    if (j.contains("Pivot") && j["Pivot"].is_array())
    {
        m_pivot.x = j["Pivot"][0];
        m_pivot.y = j["Pivot"][1];
    }

    // 3. SizeDelta 로드 (너비, 높이)
    if (j.contains("SizeDelta") && j["SizeDelta"].is_array())
    {
        m_sizeDelta.x = j["SizeDelta"][0];
        m_sizeDelta.y = j["SizeDelta"][1];
    }

    // 4. AnchoredPos 로드 (위치)
    if (j.contains("AnchoredPos") && j["AnchoredPos"].is_array())
    {
        m_anchoredPos.x = j["AnchoredPos"][0];
        m_anchoredPos.y = j["AnchoredPos"][1];
    }

    // 5. LocalScale 로드 (배율)
    if (j.contains("LocalScale") && j["LocalScale"].is_array())
    {
        m_localScale.x = j["LocalScale"][0];
        m_localScale.y = j["LocalScale"][1];
    }
    if (j.contains("RotationRadian"))
    {
        m_rotationRadian= j["RotationRadian"];
       
    }
    
    this->Computing_WorldRect();
}

HRESULT CUITransform::Bind_ShaderResource(shared_ptr<CShader> shaderCom, const _char* constantName)
{
    UpdateLayoutIfDirty();
    return 	shaderCom->Bind_Matrix(constantName, &m_worldMatrix);;
}



shared_ptr<CUITransform> CUITransform::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
    shared_ptr<CUITransform> instance(new CUITransform(device, context), [](CUITransform* p) {p->Free();delete(p);});
    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("failed prototype: CUITransform");
    }
    return instance;

}

shared_ptr<CComponent> CUITransform::Clone(void* arg)
{
    shared_ptr<CUITransform> instance(new CUITransform(*this), [](CUITransform* p) {p->Free();delete(p);});
    if (FAILED(instance->Initialize(arg)))
    {
        MSG_BOX("failed clone: CUITransform");
    }
    return instance;
}


void CUITransform::Free()
{
	__super::Free();
}
