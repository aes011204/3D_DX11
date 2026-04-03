#pragma once
#include "Component.h"



NS_BEGIN(Engine)


class ENGINE_DLL CUITransform final:
    public CComponent
{
public:
    struct UITRANSFORM_DESC
    {
        Vector2 vAnchorPoint = { 0.5f, 0.5f }; // 중앙 기준
        Vector2 vPivot = { 0.5f, 0.5f }; // 중앙 기준
        Vector2 vSizeDelta = { 100.f, 100.f };
        Vector2 vAnchoredPos = { 0.f, 0.f };
        Vector2 vScale = { 1.f, 1.f };

        _bool bSetParentSize = { false };
    };
        

private:
    CUITransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUITransform(const CUITransform& prototype);
public:
    virtual ~CUITransform() = default;
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* Arg) override;
            
    // 부모 자식  get set
    void SetParent(weak_ptr<CUITransform> newParent, bool keepWorldRect);
    weak_ptr<CUITransform> GetParent() { return m_Parent; }
    vector<shared_ptr<CUITransform>>& GetChildren() { return m_Children; }

    // 캔버스 설정
    void SetCanvas(class UICanvas* canvas) { m_Canvas = canvas; MarkDirtyRecursive(); }

    //레이아웃 Set
    void SetAnchorPoint(Vector2 _AnchorPoint) { m_AnchorPoint = _AnchorPoint; MarkDirtyRecursive(); }
    void SetPivot(Vector2 _Pivot) { m_Pivot = _Pivot; MarkDirtyRecursive(); }
    void SetSizeDelta(Vector2 _SizeDelta) { m_SizeDelta = _SizeDelta; MarkDirtyRecursive(); }
    void SetAnchoredPos(Vector2 _AnchoredPos) { m_AnchoredPos = _AnchoredPos; MarkDirtyRecursive(); }
    void SetLocalScale(Vector2 _localScale) { m_LocalScale = _localScale; MarkDirtyRecursive(); }
    void SetRotation(_float _fDegree) { m_RotationDegreeView = _fDegree; m_RotationRadian = XMConvertToRadians(_fDegree); MarkDirtyRecursive(); }
   // void SetTurn(_float fTimeDelta) { m_LocalScale = _localScale; MarkDirtyRecursive(); }



    Rect Get_WorldRect() { UpdateLayoutIfDirty(); return m_WorldRect; }
    XMMATRIX Get_Mat() { return XMLoadFloat4x4(&m_WorldMatrix); }
    _float2 Get_FinalSize() { return _float2(m_SizeDelta * m_LocalScale); }
    _float2 Get_SizeDelta() { return m_SizeDelta; }
    _float2 Get_LocalScale() { return m_LocalScale; }
    _float2 Get_AnchorPoint() { return m_AnchorPoint; }
    _float Get_Rotation() { return m_RotationDegreeView; }
    _float2 Get_AnchoredPos() { return m_AnchoredPos; }
 
    void UpdateLayoutIfDirty();

    void OnGui()override;

    HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShaderCom, const _char* pConstantName);
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
    weak_ptr<CUITransform> m_Parent = {};
    vector<shared_ptr<CUITransform>> m_Children;

    // 행렬 회전용
    _float4x4				m_WorldMatrix = {};

    //루트 용 캔버스
    class UICanvas* m_Canvas = {};

    //레이아웃
    //_vec2 m_AnchorMin;
    //_vec2 m_AnchorMax;

    Vector2 m_AnchorPoint = {};
    Vector2 m_Pivot = {};

    Vector2 m_SizeDelta = {}; // 실제 사이즈 픽셀 기준
    Vector2 m_AnchoredPos = {}; // 레이아웃용 오프셋 (Anchor(부모 기준점)에서 얼마나 떨어질지)

    Vector2 m_LocalScale = {}; // pivot 기준으로 UI를 배율로 키우거나 줄이는 연출용 배율 // 일단 안쓸듯

    float m_RotationRadian = { 0.f };// 내부 계산용
    float m_RotationDegreeView = {0.f}; // 수치 조절욜

    _bool m_bSetParentSize = { false };

    bool m_bKeepSizeRatio = false;
    bool m_bFreePivot = false;
    // 결과
    Rect m_WorldRect = {};
    bool m_Dirty = true;
public:
    static shared_ptr<CUITransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CComponent> Clone(void* pArg) override;
    void Free() override;
};

NS_END
