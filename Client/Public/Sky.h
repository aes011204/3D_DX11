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
	float m_Acc = {};

	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CVIBuffer> m_pVIBufferCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CTexture> m_pTextureNoiseCom = { nullptr };
	shared_ptr<CTexture> m_pTextureDistiortionCom = { nullptr };
protected:

	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float2 NightT = _float2(0.f, 0.20);	//0.00 ~0.20 : π„
	_float2 MidNightT = _float2(0.20f, 0.35);	//0.20 ~0.35 : ªı∫Æ(π„ °Ê ≥∑)
	_float2 DayT = _float2(0.35f, 0.65);	//0.35 ~0.65 : ≥∑
	_float2 SunsetT = _float2(0.65f, 0.80);	//0.65 ~0.80 : ≥Î¿ª(≥∑ °Ê ≥Î¿ª)
	_float2 AfterSunsetT = _float2(0.80f, 1.f);	//0.80 ~1.00 : π„(≥Î¿ª °Ê π„)

	_float3 Night = _float3(0.05, 0.08, 0.15);
	_float3 MidNight = _float3(0.25, 0.45, 0.65);
	_float3 Day = _float3(0.45, 0.70, 0.90);
	_float3 Sunset = _float3(1.0, 0.55, 0.25);
	_float3 AfterSunset = _float3(0.35, 0.25, 0.45);
public:
	static shared_ptr<CSky> Create(ComPtr<ID3D11Device> pDevice, ComPtr < ID3D11DeviceContext>);
	virtual shared_ptr < CGameObject> Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END