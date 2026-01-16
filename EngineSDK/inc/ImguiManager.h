#pragma once

#include <d3d11.h> 
//// ImGui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class CImguiManager
{
private:
	explicit CImguiManager();
	~CImguiManager();


public:
	void Initialize(HWND _hWnd, ID3D11Device* _Device, ID3D11DeviceContext* _Context);
	int Update();
	void LateUpdate();
	void Render();

public:

	static CImguiManager* Create();

	void Free();
private:

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


public:
	static CImguiManager* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new CImguiManager;

		return m_pInstance;
	}

	static void DestroyInstance()
	{
		delete m_pInstance;

		m_pInstance = nullptr;
	}
private:
	static CImguiManager* m_pInstance;
};

