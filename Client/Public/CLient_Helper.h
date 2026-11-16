#pragma once
#include "Engine_Define.h"

template <typename T>
_wstring EnumToWString(T value)
{
	auto name = magic_enum::enum_name(value);
	std::string str(name);
	return std::wstring(str.begin(), str.end());
}
