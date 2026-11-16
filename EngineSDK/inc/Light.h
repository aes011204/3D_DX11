#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CLight final :
    public CBase
{
private:
    CLight();
public:
    virtual ~CLight();

public:
    HRESULT Initialize(const LightDesc& lightDesc);

    const LightDesc* Get_LightDesc() const { return &m_lightDesc; }
    HRESULT Render(shared_ptr<class CShader> shader, shared_ptr<class CVIBuffer_Rect> VIBuffer);
    void Set_LightDesc(LightDesc desc){m_lightDesc = desc;}

    _bool Get_Active() const{return m_active;}

    void Set_Active(_bool value){m_active = value;}
    void Set_Position(const XMFLOAT4& pos)
    {
        m_lightDesc.position = pos;
    }
	void Set_Position(float x, float y, float z, float w = 1.f)
    {
        m_lightDesc.position = XMFLOAT4(x, y, z, w);
    }
private:
    LightDesc m_lightDesc = {};
    _bool m_active = { true };
public:


    static shared_ptr<CLight> Create(const LightDesc& lightDesc);
    virtual void Free() override;

};

NS_END