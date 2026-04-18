#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CGameInstance;
class CUI;
NS_END

NS_BEGIN(Client)

class CSky_Controller :
    public CBase
{
    DECLARE_SINGLETON(CSky_Controller)
public:

    CSky_Controller();

public:
    virtual ~CSky_Controller();

public:
    HRESULT Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    void Update(float TimeDelta);

 virtual void OnGui() override;
 const _float4& Get_Direction() const { return m_Direction; }
 const _float4& Get_Diffuse()   const { return m_Diffuse; }
 const _float4& Get_Ambient()   const { return m_Ambient; }
 const _float4& Get_Specular()  const { return m_Specular; }
 const _float3& Get_SkyColor()  const { return m_SkyColor; }

private:

    _float4 m_Direction = {};
    _float4 m_Diffuse = { 1.f, 1.f, 1.f, 1.f };//ªˆ
    _float4 m_Ambient = { 0.4f, 0.4f, 0.4f, 1.f };//»Ø∞Ê ±§ ¿¸√º π‡±‚
    _float4 m_Specular = { 1.f, 1.f, 1.f, 1.f }; // «œ¿Ã∂Û¿Ã∆Æ π„ø° 0 ≥∑¿∫ æ‡«œ∞‘ ¿÷¿Ω
    _float3 m_SkyColor = { 1.f, 1.f, 1.f};



    //


    //

    //_float2 NightT = _float2(0.f, 0.30);	//0.00 ~0.20 : π„
    //_float2 MidNightT = _float2(0.15f, 0.45);	//0.20 ~0.35 : ªı∫Æ(π„ °Ê ≥∑)
    //_float2 DayT = _float2(0.35f, 0.65);	//0.35 ~0.65 : ≥∑
    //_float2 SunsetT = _float2(0.65f, 0.85);	//0.65 ~0.80 : ≥Î¿ª(≥∑ °Ê ≥Î¿ª)
    //_float2 AfterSunsetT = _float2(0.80f, .95f);	//0.80 ~1.00 : π„(≥Î¿ª °Ê π„)
    //_float2 NightT2 = _float2(0.85f, 1.0f);
    _float2 NightT = _float2(0.00f, 0.25f);
    _float2 MidNightT = _float2(0.15f, 0.40f);
    _float2 DayT = _float2(0.35f, 0.70f);
    _float2 SunsetT = _float2(0.60f, 0.80f);
    _float2 AfterSunsetT = _float2(0.75f, 0.92f);
    _float2 NightT2 = _float2(0.85f, 1.00f);


    _float3 Night = _float3(0.05, 0.08, 0.15);
    _float3 MidNight = _float3(0.25, 0.45, 0.65);
    _float3 Day = _float3(0.45, 0.70, 0.90);
    _float3 Sunset = _float3(1.0, 0.55, 0.25);
    _float3 AfterSunset = _float3(0.35, 0.25, 0.45);
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };


public:
    inline _float3 Lerp3(const _float3& a, const _float3& b, float t)
    {
        XMVECTOR va = XMLoadFloat3(&a);
        XMVECTOR vb = XMLoadFloat3(&b);

        XMVECTOR vr = XMVectorLerp(va, vb, t);

        _float3 result;
        XMStoreFloat3(&result, vr);
        return result;
    }
public:
    static shared_ptr<CSky_Controller> Create();
    virtual void Free() override;

};



NS_END
