#include <ExportTable.hpp>
#include<sstream>

namespace Mortis::API
{
	auto GetDirectory(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<IMAGE_EXPORT_DIRECTORY>
	{
		auto FileHeader = GetDosAndNtHeader(ProcessHandle, BaseAddress);
		if (FileHeader == nullptr) {
			return nullptr;
		}
		auto ExpDir = std::make_unique<IMAGE_EXPORT_DIRECTORY>();
		if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, FileHeader->second.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress), ExpDir.get(), sizeof(IMAGE_EXPORT_DIRECTORY), 0) == false) {
			return nullptr;
		}
		return ExpDir;
	}

	auto GetDirectory(HMODULE BaseAddress /*= nullptr*/) 
		-> Expected<PIMAGE_EXPORT_DIRECTORY>
	{
		auto expDosAndNt = GetDosAndNtHeader(BaseAddress);
		if (expDosAndNt.has_value() == false) {
			return UnExpected(expDosAndNt.error());
		}
		auto& [pDos, pNt] = expDosAndNt.value();
		return OffsetAddress<PIMAGE_EXPORT_DIRECTORY>(pDos, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	}

	auto GetNameOfRVAGroup(HANDLE ProcessHandle, HMODULE BaseAddress, const std::unique_ptr<IMAGE_EXPORT_DIRECTORY>& ExpDir)
		-> std::vector<Rva>
	{
		if (ExpDir == nullptr) {
			return {};
		}
		std::vector<Rva> Namestable;
		Namestable.resize(ExpDir->NumberOfNames);

		if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, ExpDir->AddressOfNames), &Namestable[0], ExpDir->NumberOfNames * sizeof(Rva), 0) == false) {
			return {};
		}
		return Namestable;
	}

	auto GetNameOfRVAGroup(IMAGE_EXPORT_DIRECTORY& ExpDir, HMODULE BaseAddress /*= nullptr*/)
		-> std::span<Rva>
	{
		BaseAddress = BaseAddress ? BaseAddress : reinterpret_cast<HMODULE>(GetPEB()->ImageBaseAddress);
		if (ExpDir.AddressOfNames == 0) {
			return {};
		}
		std::span<Rva> Namestable{ OffsetAddress<Rva*>(BaseAddress, ExpDir.AddressOfNames),ExpDir.NumberOfNames };
		return Namestable;
	}

	auto GetTable(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::vector<ExportTable>
	{
		auto ExpDir = GetDirectory(ProcessHandle, BaseAddress);
		if (ExpDir == nullptr) {
			return {};
		}
		auto Namestable = GetNameOfRVAGroup(ProcessHandle, BaseAddress, ExpDir);
		if (Namestable.empty()) {
			return {};
		}
		std::vector<ExportTable> ExportTable{ ExpDir->NumberOfNames,{} };

		std::array<char, 256> ProcName;

		for (DWORD i = 0; i != ExpDir->NumberOfNames; i++) {

			auto& [ordinal, rva, procName] = ExportTable[i];

			if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, Namestable[i]), ProcName.data(), ProcName.size() - 1, 0) == false) {
				return {};
			}

			procName = ProcName.data();

			if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, ExpDir->AddressOfNameOrdinals + sizeof(int16_t) * i), &ordinal, sizeof(int16_t), 0) == false) {
				return {};
			}
			if (ReadProcessMemory(ProcessHandle, OffsetAddress(BaseAddress, ExpDir->AddressOfFunctions + (ordinal - ExpDir->Base + 1) * sizeof(int32_t)), &rva, sizeof(int32_t), 0) == false) {
				return {};
			}
		}
		return ExportTable;
	}

	auto GetTable(HMODULE BaseAddress /*= nullptr*/)
		-> Expected <std::vector<ExportTable>>
	{
		BaseAddress = BaseAddress ? BaseAddress : reinterpret_cast<HMODULE>(GetPEB()->ImageBaseAddress);
		auto expExportDir = GetDirectory(BaseAddress);
		if (expExportDir.has_value() == false) {
			return UnExpected(expExportDir.error());
		}
		auto& pExportDir = expExportDir.value();
		auto nameRvaView = GetNameOfRVAGroup(*pExportDir, BaseAddress);
		if (nameRvaView.size() == 0) {
			return UnExpected("The Module No ExportTable!!!");
		}
		std::vector<ExportTable> ExportTable{ pExportDir->NumberOfNames,{}};

		for (std::size_t i = 0; i != pExportDir->NumberOfNames; i++) {
			auto& [ordinal, rva, procName] = ExportTable[i];
			procName = OffsetAddress<const char*>(BaseAddress, nameRvaView[i]);
			ordinal = *OffsetAddress<Ordinal*>(BaseAddress, pExportDir->AddressOfNameOrdinals + sizeof(Ordinal) * i);
			rva = *OffsetAddress<Rva*>(BaseAddress, pExportDir->AddressOfFunctions + (ordinal - pExportDir->Base + 1) * sizeof(Rva));
		}
		return ExportTable;
	}

	auto ShowTable(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::stringstream
	{
		std::stringstream ss;
		for (const auto& [ordinal, addr, name] : GetTable(ProcessHandle, BaseAddress)) {
			ss << std::hex << ordinal << " \t" << addr << " \t" << name << std::endl;
		}
		return ss;
	}

	auto ShowTable(HMODULE BaseAddress /*= nullptr*/) -> Expected<std::string>
	{
		BaseAddress = BaseAddress ? BaseAddress : reinterpret_cast<HMODULE>(GetPEB()->ImageBaseAddress);
		std::stringstream ss;
		auto expExportTable = GetTable(BaseAddress);
		if (expExportTable.has_value() == false) {
			return UnExpected(expExportTable.error());
		}
		for (const auto& [ordinal, addr, name] : expExportTable.value()) {
			ss << std::hex << ordinal << " \t" << addr << " \t" << name << std::endl;
		}
		return ss.str();
	}

	auto GetProcAddressEx(const HANDLE ProcessHandle, const HMODULE BaseAddress, const std::vector<std::string_view>& fcNameGroup)
		-> std::vector<FuncPtr>
	{
		const auto ExpTable = GetTable(ProcessHandle, BaseAddress);
		std::vector<FuncPtr> result;
		for (const auto& [ordinal, addr, name] : ExpTable) {
			if (std::find(fcNameGroup.begin(), fcNameGroup.end(), name) != fcNameGroup.end()) {
				result.emplace_back(OffsetAddress<FuncPtr>(BaseAddress, addr));
			}
		}
		return result;
	}

	auto GetProcAddressEx(std::span<std::string_view> fcNameGroup,HMODULE BaseAddress/* = nullptr*/) -> Expected<std::vector<FuncPtr>>
	{
		BaseAddress = BaseAddress ? BaseAddress : reinterpret_cast<HMODULE>(GetPEB()->ImageBaseAddress);
		const auto expExportTable = GetTable(BaseAddress);
		if (expExportTable.has_value() == false) {
			return UnExpected(expExportTable.error());
		}
		std::vector<FuncPtr> result;
		for (const auto& [ordinal, addr, name] : expExportTable.value()) {
			if (std::find(fcNameGroup.cbegin(), fcNameGroup.cend(), name) != fcNameGroup.cend()) {
				result.emplace_back(OffsetAddress<FuncPtr>(BaseAddress, addr));
			}
		}
		return result;
	}

}