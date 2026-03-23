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
    void Update(float fTimeDelta)override;
    bool Resize(UINT width, UINT height);
    void CaptureFromBackBuffer(ID3D11RenderTargetView* backBufferRTV);

    ID3D11ShaderResourceView* GetSRV() const { return m_pSRV.Get(); }

private:
    bool CreateRT(UINT width, UINT height);

    HRESULT Initialize(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

private:
    UINT m_Width = { 0 }, m_Height = {0};

    ComPtr<ID3D11Device>        m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;

    ComPtr<ID3D11Texture2D>          m_pTexture; // 캡처 대상 텍스처
    ComPtr<ID3D11ShaderResourceView> m_pSRV;     // ImGui Image용

    void Render_CameraToolbar();

    // 마우스 계산용
private:
    ImVec2 m_vRenderSize;   // 보정된 그림 크기
    ImVec2 m_vContentPos;   // 그림 시작점 (절대 좌표)
    bool   m_bMouseInScene; // 마우스가 장면 안에 있는지 여부

public:
    unique_ptr<CGameView> static Create(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);
    void Free();
};


NS_END