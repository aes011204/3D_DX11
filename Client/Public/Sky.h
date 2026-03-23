#pragma once
#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
NS_END
NS_BEGIN(Engine)
class CSky :
    public CGameObject
{
public:
	typedef struct tagTerrainDesc final : public CGameObject::GAMEOBJECT_DESC
	{

	}TERRAIN_DESC;
private:
	CSky(ComPtr<ID3D11Device> pDevice, ComPtr< ID3D11DeviceContext> pContext);
	CSky(const CSky& Prototype);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:


	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CVIBuffer> m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static shared_ptr<CSky> Create(ComPtr<ID3D11Device> pDevice, ComPtr < ID3D11DeviceContext>);
	virtual shared_ptr < CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END