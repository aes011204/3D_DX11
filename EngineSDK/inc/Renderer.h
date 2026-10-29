#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CEntity;


class CRenderer :
    public CBase
{
private:
    CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext);
public:
	virtual ~CRenderer();

public:
    HRESULT Initialize();
    void Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<class CEntity> p_GameObject);
    void Draw();
    double Get_GpuRenderMs() const { return m_gpuRenderMs; }
    bool Has_GpuTiming() const { return m_gpuTimingReady; }
    double Get_SeaGpuMs() const { return m_seaGpuMs; }
    double Get_SeaCpuMs() const { return m_seaCpuMs; }
    bool Has_SeaGpuTiming() const { return m_seaGpuTimingReady; }
#ifdef _DEBUG
    void Add_DebugenderGroup(shared_ptr < class  CComponent > pDebugComponent);
private:
    bool m_showColliders = false;
#endif
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<CGameInstance> m_pGameInstance = { };

   // ComPtr <ID3D11DepthStencilState> m_pDepthDisableState = { nullptr };

    vector<shared_ptr<CEntity>> m_RenderObject[ETOI(RENDERGROUP::END)];

private:
    _float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
    shared_ptr<class CVIBuffer_Rect> m_pVIBuffer = { nullptr };
    shared_ptr<class CShader> m_pShader = { nullptr };
#ifdef _DEBUG
    list<shared_ptr<class CComponent>>				m_DebugComponents;
#endif

private:
    void Initialize_GpuTiming();
    int Begin_GpuTiming();
    void End_GpuTiming(int slot);
    struct GPU_TIMING_SLOT
    {
        ComPtr<ID3D11Query> disjoint;
        ComPtr<ID3D11Query> start;
        ComPtr<ID3D11Query> end;
        ComPtr<ID3D11Query> seaStart;
        ComPtr<ID3D11Query> seaEnd;
        bool pending = false;
    };
    GPU_TIMING_SLOT m_gpuTimings[4];
    unsigned int m_gpuTimingIndex = 0;
    double m_gpuRenderMs = 0.0;
    bool m_gpuTimingEnabled = false;
    bool m_gpuTimingReady = false;
    double m_seaGpuMs = 0.0;
    double m_seaCpuMs = 0.0;
    bool m_seaCpuTimingReady = false;
    bool m_seaGpuTimingReady = false;

    void Render_Priority();
    void Render_NonBlend();
    void Render_Sea();
    void Render_NonLight();
    void Render_Blend();
    void Render_Lights();
    void Render_Combined();
    void Render_UI();
    void RenderBloom();


private:
#ifdef _DEBUG

    void Render_Debug();

#endif


    _float2 m_WinSize = {};
public:
    static unique_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext);
    void Free()override;

};

NS_END
