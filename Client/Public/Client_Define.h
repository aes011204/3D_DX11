#pragma once

#include <Windows.h> // 다른데서 이 디파인만 포함해도 HWND,HINSTANCE 를 쓸수 있게 하기 위해 
#include <process.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;


// 테스트용
extern WNDCLASSEXW wcex;

namespace Client
{
	const unsigned int g_iWinSizeX = { 1920 };
	const unsigned int g_iWinSizeY = { 1080 };
	//const unsigned int g_iWinSizeX = { 1280 };
	//const unsigned int g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, EDITOR, END };
	enum class TAB : unsigned int { NONE = 0, INVEN = 1 << 0, STORAGE = 1 << 1, ETC = 1 << 2 };
}


using namespace Client;

//#include "Engine_Define.h"

