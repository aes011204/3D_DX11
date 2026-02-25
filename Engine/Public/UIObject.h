#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct UIObject_Desc  : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fSizeX, fSizeY;
	}UI_DESC;

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
	_float						m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float						m_fViewportWidth{}, m_fViewportHeight{};
	_float4x4					m_TransformationMatrices[ETOI(D3DTS::END)];

protected:
	void Update_Transform();
	HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, D3DTS eTransformState);

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END

