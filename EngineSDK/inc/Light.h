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
    HRESULT Initialize(const LIGHT_DESC& LightDesc);

    const LIGHT_DESC* Get_LightDesc() const { return &m_LightDesc; }
    HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
    void Set_LightDesc(LIGHT_DESC Desc){m_LightDesc = Desc;}

    _bool Get_Active() const{return m_Active;}

    void Set_Active(_bool value){m_Active = value;}
    void Set_Position(const XMFLOAT4& vPos)
    {
        m_LightDesc.vPosition = vPos;
    }
	void Set_Position(float x, float y, float z, float w = 1.f)
    {
        m_LightDesc.vPosition = XMFLOAT4(x, y, z, w);
    }
private:
    LIGHT_DESC m_LightDesc = {};
    _bool m_Active = { true };
public:


    static shared_ptr<CLight> Create(const LIGHT_DESC& LightDesc);
    virtual void Free() override;

};

NS_END