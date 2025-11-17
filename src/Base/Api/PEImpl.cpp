#include <PEImpl.hpp>
#include <ProcessImpl.hpp>

namespace Mortis::API
{
	using namespace Mortis::TYPE;
	PPEB GetPEB() {
#ifdef _WIN64
		return (PPEB)__readgsqword(0x60);
#else
		return (PPEB)__readfsdword(0x30);
#endif
	}

	PTEB GetTEB() {
#ifdef _WIN64
		return (PTEB)__readgsqword(0x00);
#else
		return (PTEB)__readfsdword(0x00);
#endif
	}


	auto IsDebuggerPresent() -> bool {
		return GetPEB()->BeingDebugged;
	}

	auto GetDosAndNtHeader(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>
	{
		if (!ProcessHandle || !BaseAddress) {
			return nullptr;
		}

		auto FileHeader = std::make_unique<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>();

		if ((ReadProcessMemory(ProcessHandle, BaseAddress, &FileHeader->first, sizeof(IMAGE_DOS_HEADER), 0) == false)
			|| (FileHeader->first.e_magic != 0x5A4D)) {
			return nullptr;
		}
		if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, FileHeader->first.e_lfanew), &FileHeader->second, sizeof(IMAGE_NT_HEADERS), 0) == false
			|| (FileHeader->second.Signature != 0x4550)) {
			return nullptr;
		}
		return FileHeader;
	}

	auto GetDosAndNtHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>
	{
		auto dosAndNt = std::make_pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>(nullptr, nullptr);
		auto& [pDos, pNt] = dosAndNt;

		auto expDos = GetDosHeader(BaseAddress);
		if (expDos.has_value() == false) {
			return UnExpected(expDos.error());
		}
		auto expNt = GetNtHeader(*expDos.value());
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return std::make_pair(expDos.value(), expNt.value());
	}

	auto GetDosHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<PIMAGE_DOS_HEADER>
	{
		auto pDos = BaseAddress ? reinterpret_cast<PIMAGE_DOS_HEADER>(BaseAddress) : reinterpret_cast<PIMAGE_DOS_HEADER>(GetPEB()->ImageBaseAddress);
		if (pDos->e_magic != 0x5A4D) {
			return UnExpected("GetDosHeader Error!!!");
		}
		return pDos;
	}

	auto GetNtHeader(HMODULE BaseAddress /*= nullptr*/) 
		-> Expected<PIMAGE_NT_HEADERS>
	{
		auto expDos = GetDosHeader(BaseAddress);
		if (expDos.has_value() == false) {
			return UnExpected(expDos.error());
		}
		return GetNtHeader(*expDos.value());
	}
	auto GetNtHeader(const IMAGE_DOS_HEADER& dos) 
		-> Expected<PIMAGE_NT_HEADERS>
	{
		auto pNt = OffsetAddress<PIMAGE_NT_HEADERS>(&dos, dos.e_lfanew);
		if (pNt->Signature != 0x4550) {
			return UnExpected("GetNtHeader Error!!!");
		}
		return pNt;
	}

	auto GetFileHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<PIMAGE_FILE_HEADER> 
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetFileHeader(*expNt.value());
	}
	auto GetFileHeader(IMAGE_NT_HEADERS& nt) 
		-> PIMAGE_FILE_HEADER {
		return &nt.FileHeader;
	}
	auto GetFileHeader(const IMAGE_NT_HEADERS& nt) 
		-> const IMAGE_FILE_HEADER* {
		return &nt.FileHeader;
	}

	auto GetOptHeader(HMODULE BaseAddress /*= nullptr*/) -> Expected<PIMAGE_OPTIONAL_HEADER>
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetOptHeader(*expNt.value());
	}

	auto GetOptHeader(IMAGE_NT_HEADERS& nt) 
		-> Expected<PIMAGE_OPTIONAL_HEADER> {
		return &nt.OptionalHeader;
	}

	auto GetSecSpan(HMODULE BaseAddress) 
		-> Expected<std::span<IMAGE_SECTION_HEADER>>
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetSecSpan(*expNt.value());
	}

	auto GetSecSpan(const IMAGE_NT_HEADERS& nt) 
		-> Expected<std::span<IMAGE_SECTION_HEADER>>
	{
		auto pFileHeader = GetFileHeader(nt);
		auto SecNum = pFileHeader->NumberOfSections;
		return std::span<IMAGE_SECTION_HEADER>{	IMAGE_FIRST_SECTION(&nt), pFileHeader->NumberOfSections};
	}

	auto GetLastSec(HMODULE BaseAddress) 
		-> Expected<PIMAGE_SECTION_HEADER>
	{
		auto expSecSpan = GetSecSpan(BaseAddress);
		if (expSecSpan.has_value() == false) {
			return UnExpected(expSecSpan.error());
		}
		return &expSecSpan.value().back();
	}

	auto GetSecByName(HMODULE BaseAddress, std::string_view sName) 
		-> Expected<PIMAGE_SECTION_HEADER>
	{
		auto expSec = GetSecSpan(BaseAddress);
		if (expSec.has_value() == false) {
			return UnExpected(expSec.error());
		}
		for (auto& sec : expSec.value()) {
			char bufName[10] = { 0 };
			memcpy_s(bufName, 8, sec.Name, 8);
			if (std::string_view(bufName) == sName) {
				return &sec;
			}
		}
		return UnExpected("GetSecByName no found sec By Name");
	}

	std::size_t AlignMent(std::size_t size, std::size_t alignment) {
		return (size) % (alignment) == 0 ? (size) : ((size) / alignment + 1) * (alignment);
	}
}