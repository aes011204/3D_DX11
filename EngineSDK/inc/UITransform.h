#pragma once
#include "Component.h"



NS_BEGIN(Engine)


class ENGINE_DLL CUITransform final:
    public CComponent
{
public:
    struct UITransformDesc
    {
        Vector2 anchorPoint = { 0.5f, 0.5f }; // 중앙 기준
        Vector2 pivot = { 0.5f, 0.5f }; // 중앙 기준
        Vector2 sizeDelta = { 100.f, 100.f };
        Vector2 anchoredPos = { 0.f, 0.f };
        Vector2 scale = { 1.f, 1.f };

        _bool isSetParentSize = { false };
    };
        

private:
    CUITransform(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUITransform(const CUITransform& prototype);
public:
    virtual ~CUITransform() = default;
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* Arg) override;
            
    // 부모 자식  get set
    void SetParent(weak_ptr<CUITransform> newParent, bool keepWorldRect);
    weak_ptr<CUITransform> GetParent() { return m_parent; }
    vector<shared_ptr<CUITransform>>& GetChildren() { return m_children; }

    // 캔버스 설정
    void SetCanvas(class UICanvas* canvas) { m_canvas = canvas; MarkDirtyRecursive(); }

    //레이아웃 Set
    void SetAnchorPoint(Vector2 anchorPoint) { m_anchorPoint = anchorPoint; MarkDirtyRecursive(); }
    void SetPivot(Vector2 pivot) { m_pivot = pivot; MarkDirtyRecursive(); }
    void SetSizeDelta(Vector2 sizeDelta) { m_sizeDelta = sizeDelta; MarkDirtyRecursive(); }
    void SetAnchoredPos(Vector2 anchoredPos) { m_anchoredPos = anchoredPos; MarkDirtyRecursive(); }
    void SetLocalScale(Vector2 localScale) { m_localScale = localScale; MarkDirtyRecursive(); }
    void SetRotation(_float degree) { m_rotationDegreeView = degree; m_rotationRadian = XMConvertToRadians(degree); MarkDirtyRecursive(); }
   // void SetTurn(_float fTimeDelta) { m_LocalScale = _localScale; MarkDirtyRecursive(); }

    _bool SetRotationTo(_float targetDegree, _float timeDelta);


    Rect Get_WorldRect() { UpdateLayoutIfDirty(); return m_worldRect; }
    XMMATRIX Get_Mat() { return XMLoadFloat4x4(&m_worldMatrix); }
    _float2 Get_FinalSize() { return _float2(m_sizeDelta * m_localScale); }
    _float2 Get_SizeDelta() { return m_sizeDelta; }
    _float2 Get_LocalScale() { return m_localScale; }
    _float2 Get_AnchorPoint() { return m_anchorPoint; }
    _float Get_Rotation() { return m_rotationDegreeView; }
    _float2 Get_AnchoredPos() { return m_anchoredPos; }
 
    void UpdateLayoutIfDirty();

    void OnGui()override;

    HRESULT Bind_ShaderResource(shared_ptr<class CShader> shaderCom, const _char* constantName);
private:

    void MarkDirtyRecursive(); // dirty 처리
    Rect GetParent_WorldRect();
    XMMATRIX GetParent_Mat();
    void Computing_WorldRect(); // 핵심 계산 함수

    Vector2 Hadamard(const Vector2& a, const Vector2& b); // _vec2 요소별 곱

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;
private:
    // 상속 구조용
    weak_ptr<CUITransform> m_parent = {};
    vector<shared_ptr<CUITransform>> m_children;

    // 행렬 회전용
    _float4x4				m_worldMatrix = {};

    //루트 용 캔버스
    class UICanvas* m_canvas = {};

    //레이아웃
    //_vec2 m_AnchorMin;
    //_vec2 m_AnchorMax;

    Vector2 m_anchorPoint = {};
    Vector2 m_pivot = {};

    Vector2 m_sizeDelta = {}; // 실제 사이즈 픽셀 기준
    Vector2 m_anchoredPos = {}; // 레이아웃용 오프셋 (Anchor(부모 기준점)에서 얼마나 떨어질지)

    Vector2 m_localScale = {}; // pivot 기준으로 UI를 배율로 키우거나 줄이는 연출용 배율 // 일단 안쓸듯

    float m_rotationRadian = { 0.f };// 내부 계산용
    float m_rotationDegreeView = {0.f}; // 수치 조절욜

    _bool m_isSetParentSize = { false };

    bool m_isKeepSizeRatio = false;
    bool m_isFreePivot = false;
    // 결과
    Rect m_worldRect = {};
    bool m_isDirty = true;
public:
    static shared_ptr<CUITransform> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    virtual shared_ptr<CComponent> Clone(void* arg) override;
    void Free() override;
};

NS_END
