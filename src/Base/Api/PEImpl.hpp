#pragma once
#include <utils.hpp>
#include <PE_types.hpp>
#include <PEB.h>

#include <ProcessImpl.hpp>

namespace Mortis::API
{
	using namespace Mortis::TYPE;
}

namespace Mortis::API
{
	TYPE::PPEB GetPEB();
	TYPE::PTEB GetTEB();

	auto GetDosAndNtHeader(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>;
	auto GetDosAndNtHeader(HMODULE BaseAddress = nullptr)
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>;

	auto GetDosHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_DOS_HEADER>;

	auto GetNtHeader(const IMAGE_DOS_HEADER& BaseAddress)
		-> Expected<PIMAGE_NT_HEADERS>;
	auto GetNtHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_NT_HEADERS>;

	auto GetFileHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_FILE_HEADER>;
	auto GetFileHeader(IMAGE_NT_HEADERS& nt)
		-> PIMAGE_FILE_HEADER;
	auto GetFileHeader(const IMAGE_NT_HEADERS& nt)
		-> const IMAGE_FILE_HEADER*;

	auto GetOptHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_OPTIONAL_HEADER>;
	auto GetOptHeader(IMAGE_NT_HEADERS& nt)
		-> Expected<PIMAGE_OPTIONAL_HEADER>;

	auto GetSecSpan(HMODULE BaseAddress)
		-> Expected<std::span<IMAGE_SECTION_HEADER>>;
	auto GetSecSpan(const IMAGE_NT_HEADERS& BaseAddress)
		-> Expected<std::span<IMAGE_SECTION_HEADER>>;
	auto GetLastSec(HMODULE BaseAddress)
		-> Expected<PIMAGE_SECTION_HEADER>;
	auto GetSecByName(HMODULE BaseAddress, std::string_view sName)
		-> Expected<PIMAGE_SECTION_HEADER>;

	auto AddSection(HMODULE BaseAddress, std::string_view sName, DWORD dwSize, DWORD dwCharacteristics)
		-> Expected<PIMAGE_SECTION_HEADER>;


	auto IsDebuggerPresent() 
		-> bool;
}