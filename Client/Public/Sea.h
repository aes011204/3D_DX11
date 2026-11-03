#pragma once
#include "GameObject.h"
#include "Client_Define.h"

NS_BEGIN(Engine)

class CVIBuffer_Sea;
	class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CSea final:
    public CGameObject
{
public:
    

public:
    struct SeaDesc final : public CGameObject::GameObjectDesc
    {

    };
private:
    CSea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSea(const CSea& prototype);
public:
    virtual ~CSea() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    float Calculate_GerstnerWave_Overlap(_float3 Pos);
    virtual HRESULT Render() override;
    virtual _vector Get_WorldPos();
    virtual void OnGui() override;

protected:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();
private:
    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CVIBuffer> m_VIBufferCom = { nullptr };
    shared_ptr<CVIBuffer_Sea> m_cachedVIBufferCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };

    shared_ptr<class CSky_Controller> m_skyControl = { nullptr };
   weak_ptr<class CSea_Manager> m_seaManager = {};
 /*   float m_AccTime = {};
    int m_WaveCount = {};
    Wave_Desc m_waveDesc[10] = {};
    float m_depthMask01 = {1};*/
public:
    static shared_ptr<CSea> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;

};

NS_END