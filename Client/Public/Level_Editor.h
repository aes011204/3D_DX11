#pragma once
#include "Level.h"


class CLevel_Editor :
    public CLevel
{
private:
	CLevel_Editor(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Post_Initialize()override { return S_OK; };
	virtual void Update(_float timeDelta) override;
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& layerTag);
	HRESULT Ready_Layer_Camera(const _wstring& layerTag);
	virtual HRESULT Render() override;

	void OnGui()override;
public:
	static shared_ptr<CLevel_Editor> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual void Free() override;

};

