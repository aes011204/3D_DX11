#pragma once

#include <Windows.h> // 다른데서 이 디파인만 포함해도 HWND,HINSTANCE 를 쓸수 있게 하기 위해 

extern HWND g_hWnd;
extern HINSTANCE g_hInst;



// 테스트용
extern WNDCLASSEXW wcex;

namespace Client
{
	 const unsigned int g_iWinSizeX = { 1280 };
	 const unsigned int g_iWinSizeY = { 720 };

}
	 //extern UINT g_RsizeWidth;
	 //extern UINT g_RsizeHeight;

using namespace Client;