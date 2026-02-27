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

private:
    LIGHT_DESC m_LightDesc = {};
public:


    static shared_ptr<CLight> Create(const LIGHT_DESC& LightDesc);
    virtual void Free() override;

};

NS_END