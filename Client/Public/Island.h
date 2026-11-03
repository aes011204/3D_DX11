#pragma once

#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CIsland :
    public CGameObject
{
private:
    CIsland(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CIsland(const CIsland& prototype);
public:
    virtual ~CIsland() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

    virtual void Save_ToJson(nlohmann::json& j) override;
    virtual void Load_FromJson(nlohmann::json& j) override;
protected:
    HRESULT Ready_Components();
private:

    float m_colorLamp1={};
    float m_colorLamp2={};

    _float4 m_sandColor={};
    _float4 m_rockColor={};
    _float4 m_grassColor={};
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CModel> m_modelCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };

public:
    static shared_ptr<CIsland> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END