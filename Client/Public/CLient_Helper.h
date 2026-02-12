#pragma once
#include "Engine_Define.h"

template <typename T>
_wstring EnumToWString(T eValue)
{
	auto name = magic_enum::enum_name(eValue);
	std::string str(name);
	return std::wstring(str.begin(), str.end());
}
