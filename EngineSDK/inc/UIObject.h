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
	CUIObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CUIObject(const CUIObject& prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void Priority_Update(_float timeDelta) override;
	virtual void Update(_float timeDelta) override;
	virtual void Late_Update(_float timeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float						m_x{}, m_y{}, m_sizeX{}, m_sizeY{};
	_float						m_viewportWidth{}, m_viewportHeight{};
	_float4x4					m_transformationMatrices[ETOI(ED3DTS::END)];

protected:
	void Update_Transform();
	HRESULT Bind_ShaderResource(shared_ptr<class CShader> shader, const _char* constantName, ED3DTS transformState);

public:
	virtual shared_ptr<CGameObject> Clone(void* arg) = 0;
	virtual void Free() override;
};

NS_END

