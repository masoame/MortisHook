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

	auto IsDebuggerPresent() 
		-> bool;
}