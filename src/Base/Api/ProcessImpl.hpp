#pragma once
#include <PE_types.hpp>

namespace Mortis::API
{
	using namespace Mortis::TYPE;

	inline auto Process32First(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
		return ::Process32FirstW(hSnapshot, lppe);
	}

	inline auto Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
		return ::Process32NextW(hSnapshot, lppe);
	}

	inline auto Module32First(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
		return ::Module32FirstW(hSnapshot, lpme);
	}

	inline auto Module32Next(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
		return ::Module32NextW(hSnapshot, lpme);
	}

	template<typename AddressType = LPVOID>
	constexpr auto OffsetAddress(auto address, std::ptrdiff_t offset) noexcept -> AddressType {
		return reinterpret_cast<AddressType>(reinterpret_cast<std::ptrdiff_t>(address) + offset);
	};

	template<typename T>
	auto OpenProcessHandle(const PROCESSENTRY32<T>& processEntry32, DWORD dwDesiredAccess = PROCESS_ALL_ACCESS, BOOL bInheritHandle = FALSE)
		-> ScopeHandle<>;

	auto OpenProcessHandle(DWORD th32ProcessID, DWORD dwDesiredAccess = PROCESS_ALL_ACCESS, BOOL bInheritHandle = FALSE)
		-> ScopeHandle<>;

	auto OpenThreadHandle(DWORD dwThreadId, DWORD dwDesiredAccess = THREAD_ALL_ACCESS, BOOL bInheritHandle = FALSE)
		-> ScopeHandle<>;


	template<typename T>
	auto SearchProcess(std::basic_string_view<T> process_name_view)
		-> std::unique_ptr<PROCESSENTRY32<T>>;

	template<typename T, typename C = BT::extract_char_type_t<T>>
		requires std::is_convertible_v<T, std::basic_string_view<C>>
	auto SearchProcess(T&& process_name) { return SearchProcess<C>(std::forward<T>(process_name)); }


	template<typename T>
	auto SearchModule(DWORD th32ProcessID, std::basic_string_view<T> module_name_view)
		-> std::unique_ptr<MODULEENTRY32<T>>;

	template<typename T, typename C = BT::extract_char_type_t<T>>
		requires std::is_convertible_v<T, std::basic_string_view<C>>
	auto SearchModule(DWORD th32ProcessID, T&& module_name_view) { return SearchModule<C>(std::forward<T>(module_name_view)); }


	template<typename T = char>
	auto ProcessInfo()
		-> std::vector<PROCESSENTRY32<T>>;

	template<typename T = char>
	auto ProcessInfoMap(EnumInfoMapType key_type)
		-> std::map<
		std::variant<ProcessId, std::basic_string<T>>,
		SingleOrMultiple<PROCESSENTRY32<T>>
		>;

	template<typename T = char>
	auto ModuleInfo(DWORD th32ProcessID)
		-> std::vector<MODULEENTRY32<T>>;

	auto ThreadInfo(DWORD th32ProcessID)
		-> std::vector<THREADENTRY32>;

	template<typename T = char>
	auto ModuleInfoMap(DWORD th32ProcessID, EnumInfoMapType key_type)
		-> std::map<std::variant<HMODULE, CaseInsensitiveStdString<T>>, MODULEENTRY32<T>>;


}
namespace Mortis::API
{
	template<typename T>
	auto OpenProcessHandle(const PROCESSENTRY32<T>& processEntry32, DWORD dwDesiredAccess, BOOL bInheritHandle)
		-> ScopeHandle<> {
		return OpenProcessHandle(processEntry32->th32ProcessID, dwDesiredAccess, bInheritHandle);
	}

	template<typename T>
	auto SearchProcess(std::basic_string_view<T> process_name_view)
		-> std::unique_ptr<PROCESSENTRY32<T>>
	{
		auto process_entry = std::make_unique<PROCESSENTRY32<T>>();
		process_entry->dwSize = sizeof(PROCESSENTRY32<T>);

		ScopeHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return nullptr;
		}
		BOOL bFound = Process32First(hProcessSnap, process_entry.get());
		while (bFound){
			if (std::basic_string_view<T>(process_entry->szExeFile) == process_name_view) {
				return process_entry;
			}
			bFound = Process32Next(hProcessSnap, process_entry.get());
		}
		return nullptr;
	}

	template<typename T>
	auto SearchModule(DWORD th32ProcessID, std::basic_string_view<T> module_name_view)
		-> std::unique_ptr<MODULEENTRY32<T>>
	{
		auto module_entry = std::make_unique<MODULEENTRY32<T>>();
		module_entry->dwSize = sizeof(MODULEENTRY32<T>);

		ScopeHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, th32ProcessID);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return nullptr;
		}

		BOOL bFound = Module32First(hProcessSnap, module_entry.get());
		while (bFound){
			if (CaseInsensitiveCompare(module_name_view,std::basic_string_view<T>(module_entry->szModule))) {
				return module_entry;
			}
			bFound = Module32Next(hProcessSnap, module_entry.get());
		};
		return nullptr;
	}

	template<typename T>
	auto ProcessInfo()
		-> std::vector<PROCESSENTRY32<T>>
	{
		std::vector<PROCESSENTRY32<T>> info{};

		ScopeHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return info;
		}

		PROCESSENTRY32<T> process_entry{};
		process_entry.dwSize = sizeof(PROCESSENTRY32<T>);

		BOOL bFound = Process32First(hProcessSnap, &process_entry);
		while (bFound){
			info.emplace_back(process_entry);
			bFound = Process32Next(hProcessSnap, &process_entry);
		};
		return info;
	}

	template<typename T>
	auto ProcessInfoMap(EnumInfoMapType key_type)
		-> std::map<
		std::variant<ProcessId, std::basic_string<T>>,
		SingleOrMultiple<PROCESSENTRY32<T>>
		>
	{
		std::map<
			std::variant<ProcessId, std::basic_string<T>>,
			SingleOrMultiple<PROCESSENTRY32<T>>
		> info_map;

		const auto info_arr = ProcessInfo<T>();

		switch (key_type)
		{
		case PROCESS_ID:

			for (const PROCESSENTRY32<T>& info : info_arr) {
				if (info_map.contains(info.th32ProcessID) == true) {
					throw std::exception("error repeat th32ProcessID");
				}
				info_map.emplace(info.th32ProcessID, info);
			}

			break;
		case PROCESS_SZExeFile:

			for (const PROCESSENTRY32<T>& info : info_arr) {
				std::basic_string<T> szExeFile(info.szExeFile);
				if (const auto iter = info_map.find(szExeFile); iter != info_map.cend()) {
					iter->second.emplace_back(info);
				}
				info_map.emplace(std::move(szExeFile), info);
			}

			break;

		default:
			throw std::exception();
		}
		return info_map;
	}

	template<typename T>
	auto ModuleInfo(DWORD th32ProcessID)
		-> std::vector<MODULEENTRY32<T>>
	{
		std::vector<MODULEENTRY32<T>> info{};
		ScopeHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, th32ProcessID);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return info;
		}

		MODULEENTRY32<T> module_entry{};
		module_entry.dwSize = sizeof(MODULEENTRY32<T>);

		BOOL bFound = Module32First(hProcessSnap, &module_entry);
		while (bFound) {
			info.emplace_back(module_entry);
			bFound = Module32Next(hProcessSnap, &module_entry);
		};
		return info;
	}

	template<typename T>
	auto ModuleInfoMap(DWORD th32ProcessID, EnumInfoMapType key_type)
		-> std::map<std::variant<HMODULE, CaseInsensitiveStdString<T>>, MODULEENTRY32<T>>
	{
		std::map<std::variant<HMODULE, CaseInsensitiveStdString<T>>, MODULEENTRY32<T>> info_map;
		auto info_arr = ModuleInfo<T>(th32ProcessID);
		switch (key_type)
		{
		case MODULE_HMODULE:
			for (const MODULEENTRY32<T>& info : info_arr)
			{
				if (info_map.contains(info.hModule) == true) {
					throw std::exception("error repeat hModule");
				}
				info_map.emplace(info.hModule, info);
			}
			break;
		case MODULE_SZModule:
			for (const MODULEENTRY32<T>& info : info_arr)
			{
				std::basic_string_view<T> szModule = info.szModule;
				if (auto iter = info_map.find(szModule); iter != info_map.cend()) {
					iter->second = info;
				}
				info_map.emplace(szModule, info);
			}
			break;
		default:
			throw std::exception();
		}
		return info_map;
	}

};