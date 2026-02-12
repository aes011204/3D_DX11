#pragma once
#include"Editor_Define.h"

NS_BEGIN(Editor)

class CImguiManager
{
private:
	explicit CImguiManager();

public:
	~CImguiManager();
	void ApplyEditorDarkStyle();


public:
	void Initialize(HWND _hWnd, ComPtr<ID3D11Device>_Device, ComPtr<ID3D11DeviceContext> _Context);
	void Begin();
	void Example();

	void Render();

	ImGuiContext* GetContext() { return ImGui::GetCurrentContext(); }


public:

	static unique_ptr<CImguiManager> Create();

	void Free();
private:

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

};

NS_END