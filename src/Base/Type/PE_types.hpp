#pragma once
#include <utils.hpp>

namespace Mortis::TYPE
{
#ifndef _WIN64
	using HOOKWORD = DWORD32;
#else
	using HOOKWORD = DWORD64;
#endif

	constexpr static const std::array<BYTE,1> INT3{ static_cast<BYTE>(0xCC) };

	using Rva = DWORD;
	using Ordinal = WORD;
	using Hint = WORD;
	using FuncPtr = LPVOID;
	using FuncArgPos = unsigned char;

	using ProcessId = DWORD;


	enum EnumInfoMapType
	{
		PROCESS_ID,
		PROCESS_SZExeFile,

		MODULE_HMODULE,
		MODULE_SZModule,
	};

	template<typename T = char>
		requires BC::HasType<T, char, wchar_t>
	using PROCESSENTRY32 = std::conditional_t<std::is_same_v<T,char>, ::PROCESSENTRY32, ::PROCESSENTRY32W>;

	template<typename T>
		requires BC::HasType<T, char, wchar_t>
	using MODULEENTRY32 = std::conditional_t<std::is_same_v<T, char>, ::MODULEENTRY32, ::MODULEENTRY32W>;
}

#include <PEConcept.hpp>

