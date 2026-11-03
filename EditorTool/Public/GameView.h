#pragma once
#include "Editor_Win.h"

NS_BEGIN(Editor)

class CGameView :
    public CEditor_Win
{
public:
    CGameView();
    ~CGameView();

public:

    void Render() override;
    void Update(float timeDelta)override;
    bool Resize(UINT width, UINT height);
    void CaptureFromBackBuffer(ID3D11RenderTargetView* backBufferRTV);

    ID3D11ShaderResourceView* GetSRV() const { return m_SRV.Get(); }

private:
    bool CreateRT(UINT width, UINT height);

    HRESULT Initialize(const EngineDesc& engineDesc, ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context);

private:
    UINT m_width = { 0 }, m_height = {0};

    ComPtr<ID3D11Device>        m_device;
    ComPtr<ID3D11DeviceContext> m_context;

    ComPtr<ID3D11Texture2D>          m_texture; // 캡처 대상 텍스처
    ComPtr<ID3D11ShaderResourceView> m_SRV;     // ImGui Image용

    void Render_CameraToolbar();

    // 마우스 계산용
private:
    ImVec2 m_renderSize;   // 보정된 그림 크기
    ImVec2 m_contentPos;   // 그림 시작점 (절대 좌표)
    bool   m_isMouseInScene; // 마우스가 장면 안에 있는지 여부

public:
    unique_ptr<CGameView> static Create(const EngineDesc& engineDesc, ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context);
    void Free();
};


NS_END