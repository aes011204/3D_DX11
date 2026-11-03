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
	typedef struct tagSkyDesc final : public CGameObject::GameObjectDesc
	{

	}SkyDesc;
private:
	CSky(ComPtr<ID3D11Device> device, ComPtr< ID3D11DeviceContext> context);
	CSky(const CSky& prototype);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void Priority_Update(_float timeDelta) override;
	virtual void Update(_float timeDelta) override;
	virtual void Late_Update(_float timeDelta) override;
	virtual HRESULT Render() override;

protected:
	float m_acc = {};

	shared_ptr<CShader> m_shaderCom = { nullptr };
	shared_ptr<CVIBuffer> m_VIBufferCom = { nullptr };
	shared_ptr<CTexture> m_textureCom = { nullptr };
	shared_ptr<CTexture> m_textureNoiseCom = { nullptr };
	shared_ptr<CTexture> m_textureDistortionCom = { nullptr };
protected:

	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	//_float2 NightT = _float2(0.f, 0.20);	//0.00 ~0.20 : 밤
	//_float2 MidNightT = _float2(0.20f, 0.40);	//0.20 ~0.35 : 새벽(밤 → 낮)
	//_float2 DayT = _float2(0.40f, 0.65);	//0.35 ~0.65 : 낮
	//_float2 SunsetT = _float2(0.65f, 0.75);	//0.65 ~0.80 : 노을(낮 → 노을)
	//_float2 AfterSunsetT = _float2(0.75f, .85f);	//0.80 ~1.00 : 밤(노을 → 밤)
	//_float2 NightT2 = _float2(0.85f, 1.0f);

	//_float3 Night = _float3(0.05, 0.08, 0.15);
	//_float3 MidNight = _float3(0.25, 0.45, 0.65);
	//_float3 Day = _float3(0.45, 0.70, 0.90);
	//_float3 Sunset = _float3(1.0, 0.55, 0.25);
	//_float3 AfterSunset = _float3(0.35, 0.25, 0.45);
public:
	static shared_ptr<CSky> Create(ComPtr<ID3D11Device> device, ComPtr < ID3D11DeviceContext>);
	virtual shared_ptr < CGameObject> Clone(void* arg) override;
	virtual void Free() override;

};

NS_END