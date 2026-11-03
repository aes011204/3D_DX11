#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CLight_Manager final :
    public CBase
{
private:
    CLight_Manager();
public:
    virtual ~CLight_Manager();

public:
    const LightDesc* Get_LightDesc(_uint iIndex);
    void Set_LightDesc(_uint iIndex, LightDesc Desc);

    shared_ptr<class CLight> Add_Light(const LightDesc& lightDesc);

    HRESULT Render_Light(shared_ptr<class CShader> pShared, shared_ptr<class CVIBuffer_Rect> pVIBuffer);

    void OnGui() override;
    void clear_light();

private:
    list<shared_ptr<class CLight>> m_lights;
public:
    static unique_ptr<CLight_Manager> Create();
    virtual void Free() override;

};

NS_END