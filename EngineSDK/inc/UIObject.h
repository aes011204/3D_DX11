#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct UIObjectDesc  : public CGameObject::GameObjectDesc
	{
		_float		x, y, sizeX, sizeY;
	}UIDesc;

protected:
	CUIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float						m_x{}, m_y{}, m_sizeX{}, m_sizeY{};
	_float						m_viewportWidth{}, m_viewportHeight{};
	_float4x4					m_transformationMatrices[ETOI(ED3DTS::END)];

protected:
	void Update_Transform();
	HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, ED3DTS eTransformState);

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END

