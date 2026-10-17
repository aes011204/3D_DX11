#pragma once
#include"Editor_Define.h"

NS_BEGIN(Editor)

class CImguiManager
{
private:
	explicit CImguiManager();

public:
	~CImguiManager();
	void ApplyEditorDarkStyle();


public:
	void Initialize(HWND _hWnd, ComPtr<ID3D11Device>_Device, ComPtr<ID3D11DeviceContext> _Context);
	void Begin();
	void Example();

	void Render();
    bool Get_GpuRenderMs(double& milliseconds) const
    {
        milliseconds = m_gpuRenderMs;
        return m_gpuTimingReady;
    }

	ImGuiContext* GetContext() { return ImGui::GetCurrentContext(); }


public:

	static unique_ptr<CImguiManager> Create();

	void Free();
private:

    void Initialize_GpuTiming(ID3D11Device* device);
    int Begin_GpuTiming();
    void End_GpuTiming(int slot);
    struct GPU_TIMING_SLOT
    {
        ComPtr<ID3D11Query> disjoint;
        ComPtr<ID3D11Query> start;
        ComPtr<ID3D11Query> end;
        bool pending = false;
    };
    GPU_TIMING_SLOT m_gpuTimings[4];
    ComPtr<ID3D11DeviceContext> m_gpuContext;
    unsigned int m_gpuTimingIndex = 0;
    double m_gpuRenderMs = 0.0;
    bool m_gpuTimingEnabled = false;
    bool m_gpuTimingReady = false;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

};

NS_END