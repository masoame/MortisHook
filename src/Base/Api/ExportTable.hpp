#pragma once
#include <PEImpl.hpp>

namespace Mortis::API
{
	struct ExportTable {
		Ordinal _ordinal;
		Rva _rva;
		std::string _func_name;
	};

	//获得导出表目录
	auto GetDirectory(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<IMAGE_EXPORT_DIRECTORY>;
	auto GetDirectory(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_EXPORT_DIRECTORY>;

	//获得导出函数名称
	auto GetNameOfRVAGroup(HANDLE ProcessHandle, HMODULE BaseAddress, const std::unique_ptr<IMAGE_EXPORT_DIRECTORY>& ExpDir = nullptr)
		-> std::vector<Rva>;
	auto GetNameOfRVAGroup(IMAGE_EXPORT_DIRECTORY& ExpDir, HMODULE BaseAddress = nullptr)
		-> std::span<Rva>;

	//获得导出表
	auto GetTable(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::vector<ExportTable>;
	auto GetTable(HMODULE BaseAddress = nullptr)
		-> Expected <std::vector<ExportTable>>;

	//显示导出表
	auto ShowTable(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::stringstream;
	auto ShowTable(HMODULE BaseAddress = nullptr)
		-> Expected<std::string>;

	//导出函数地址
	template<BC::ConceptFunType F = FuncPtr, typename TFuncPtr = std::add_pointer_t<std::remove_pointer_t<F>>>
	auto GetProcAddressEx(HANDLE ProcessHandle, HMODULE BaseAddress, std::string_view fcName)
		-> TFuncPtr;
	template<BC::ConceptFunType F = FuncPtr, typename TFuncPtr = std::add_pointer_t<std::remove_pointer_t<F>>>
	auto GetProcAddressEx(std::string_view fcName, HMODULE BaseAddress = nullptr)
		-> Expected<TFuncPtr>;


	//批量找出导出函数地址
	auto GetProcAddressEx(const HANDLE ProcessHandle, const HMODULE BaseAddress, const std::vector<std::string_view>& fcNameGroup)
		-> std::vector<FuncPtr>;
	auto GetProcAddressEx(std::span<std::string_view> fcNameGroup, HMODULE BaseAddress = nullptr)
		-> Expected<std::vector<FuncPtr>>;

}

namespace Mortis::API
{
	template<BC::ConceptFunType F, typename TFuncPtr>
	auto GetProcAddressEx(HANDLE ProcessHandle, HMODULE BaseAddress, std::string_view fcName)
		-> TFuncPtr
	{
		const auto ExpTable = GetTable(ProcessHandle, BaseAddress);
		for (const auto& [ordinal, addr, name] : ExpTable) {
			if (name == fcName) {
				return MakeAddress<TFuncPtr>(BaseAddress, addr);
			}
		}
		return nullptr;
	}

	template<BC::ConceptFunType F,typename TFuncPtr>
	auto GetProcAddressEx(std::string_view fcName, HMODULE BaseAddress /*= nullptr*/)
		-> Expected<TFuncPtr>
	{
		BaseAddress = BaseAddress ? BaseAddress : reinterpret_cast<HMODULE>(GetPEB()->ImageBaseAddress);
		auto expExportTable = GetTable(BaseAddress);
		if (expExportTable.has_value() == false) {
			return UnExpected(expExportTable.error());
		}
		for (const auto& [ordinal, addr, name] : expExportTable.value()) {
			if (name == fcName) {
				return OffsetAddress<TFuncPtr>(BaseAddress, addr);
			}
		}
		return UnExpected("GetProcAddressEx no found func By Name");
	}
}