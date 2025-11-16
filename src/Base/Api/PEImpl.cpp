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
		if (ReadProcessMemory(ProcessHandle, MakeAddress(BaseAddress, FileHeader->first.e_lfanew), &FileHeader->second, sizeof(IMAGE_NT_HEADERS), 0) == false
			|| (FileHeader->second.Signature != 0x4550)) {
			return nullptr;
		}
		return FileHeader;
	}

	auto GetDosAndNtHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>
	{
		auto fileHeader = std::make_pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>(nullptr, nullptr);
		auto& [pDos, pNt] = fileHeader;

		auto pPEB = GetPEB();
		pDos = BaseAddress ? reinterpret_cast<PIMAGE_DOS_HEADER>(BaseAddress) : reinterpret_cast<PIMAGE_DOS_HEADER>(pPEB->ImageBaseAddress);
		pNt = MakeAddress<PIMAGE_NT_HEADERS>(pDos, pDos->e_lfanew);

		if (pDos->e_magic != 0x5A4D || pNt->Signature != 0x4550) {
			return UnExpected("GetDosAndNtHeader Error!!!");
		}
		return fileHeader;
	}

	auto IsDebuggerPresent() -> bool {
		return GetPEB()->BeingDebugged;
	}

}