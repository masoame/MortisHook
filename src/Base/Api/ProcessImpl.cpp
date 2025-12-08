#include <ProcessImpl.hpp>


namespace Mortis::API
{
	auto OpenProcessHandle(DWORD th32ProcessID, DWORD dwDesiredAccess, BOOL bInheritHandle)
		-> ScopeHandle<> {
		return OpenProcess(dwDesiredAccess, bInheritHandle, th32ProcessID);
	}

	auto OpenThreadHandle(DWORD dwThreadId, DWORD dwDesiredAccess, BOOL bInheritHandle)
		-> ScopeHandle<> {
		return OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
	}

	std::wstring GetModuleFilePathW(HMODULE hModule /* = nullptr*/)
	{
		auto pathBuffer = std::make_unique<wchar_t[]>(MAX_PATH);
		if (::GetModuleFileNameW(hModule, pathBuffer.get(), MAX_PATH)) {
			return pathBuffer.get();
		}
		return {};
	}

	std::wstring GetModuleFileNameW(HMODULE hModule /*= nullptr*/)
	{
		return std::filesystem::path(GetModuleFilePathW(hModule)).filename().c_str();
	}

	std::string GetModuleFilePathA(HMODULE hModule /*= nullptr*/)
	{
		auto pathBuffer = std::make_unique<char[]>(MAX_PATH);
		if (::GetModuleFileNameA(hModule, pathBuffer.get(), MAX_PATH)) {
			return pathBuffer.get();
		}
		return {};
	}

	std::string GetModuleFileNameA(HMODULE hModule /*= nullptr*/)
	{
		return UTF16ToANSI(std::filesystem::path(GetModuleFilePathA(hModule)).filename().c_str());
	}

	auto CreateProcessByPath(std::wstring_view file_path, DWORD dwCreationFlags,const std::unique_ptr<STARTUPINFOW>& si)
		-> std::optional<PROCESS_INFORMATION>
	{
		si->cb = sizeof(STARTUPINFOW);
		PROCESS_INFORMATION pi{};
		BOOL ret = CreateProcessW(file_path.data(), nullptr, nullptr, nullptr, false, dwCreationFlags, nullptr, nullptr, si.get(), &pi);
		if (ret == FALSE) {
			return std::nullopt;
		}
		return pi;
	}

	auto CreateProcessByPath(std::string_view file_path, DWORD dwCreationFlags /*= CREATE_SUSPENDED*/, const std::unique_ptr<STARTUPINFOW>& si /*= std::make_unique<STARTUPINFOW>()*/) 
		-> std::optional<PROCESS_INFORMATION>
	{
		return CreateProcessByPath(UTF8ToUTF16(file_path), dwCreationFlags, si);
	}

	auto CreateProcessByCommand(std::wstring_view file_name, DWORD dwCreationFlags /*= CREATE_SUSPENDED*/, const std::unique_ptr<STARTUPINFOW>& si /*= std::make_unique<STARTUPINFOW>()*/) 
		-> std::optional<PROCESS_INFORMATION>
	{
		si->cb = sizeof(STARTUPINFOW);
		PROCESS_INFORMATION pi{};
		std::wstring sCommand{ file_name.data() ,file_name.size()};
		BOOL ret = CreateProcessW(nullptr, sCommand.data(), nullptr, nullptr, false, dwCreationFlags, nullptr, nullptr, si.get(), &pi);
		if (ret == FALSE) {
			return std::nullopt;
		}
		return pi;
	}

	auto CreateProcessByCommand(std::string_view file_name, DWORD dwCreationFlags /*= CREATE_SUSPENDED*/, const std::unique_ptr<STARTUPINFOW>& si /*= std::make_unique<STARTUPINFOW>()*/) 
		-> std::optional<PROCESS_INFORMATION>
	{
		return CreateProcessByCommand(UTF8ToUTF16(file_name), dwCreationFlags, si);
	}



	auto ResumeThread(const ScopeHandle<>& threadHandle)
		-> bool
	{
		return threadHandle && (::ResumeThread(threadHandle) != -1);
	}

	auto GetThreadInfo(DWORD th32ProcessID)
		-> std::vector<THREADENTRY32>
	{
		ScopeHandle hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE) {
			return {};
		}
		std::vector<THREADENTRY32> info{};
		THREADENTRY32 thread_entry_cache{};
		thread_entry_cache.dwSize = sizeof(THREADENTRY32);
		BOOL bFound = Thread32First(hSnapshot, &thread_entry_cache);
		while (bFound) {
			if (thread_entry_cache.th32OwnerProcessID == th32ProcessID) {
				info.emplace_back(thread_entry_cache);
			}
			bFound = Thread32Next(hSnapshot, &thread_entry_cache);
		}
		return info;
	}


	bool InjectDLL(DWORD th32ProcessID, std::string_view dll_path)
	{
		return InjectDLL(th32ProcessID, UTF8ToUTF16(dll_path));
	}


	bool InjectDLL(HANDLE hProcess, std::string_view dll_path)
	{
		return InjectDLL(hProcess, UTF8ToUTF16(dll_path));
	}

	bool InjectDLL(DWORD th32ProcessID, std::wstring_view dll_path)
	{
		const auto process_handle = OpenProcessHandle(th32ProcessID);
		if (process_handle == nullptr) {
			return false;
		}
		return InjectDLL(process_handle.get(), dll_path);
	}

	bool InjectDLL(HANDLE hProcess, std::wstring_view dll_path)
	{
		std::wstring location = std::filesystem::path(GetModuleFilePathW()).replace_filename(dll_path);
		const auto allocated_mem = ::VirtualAllocEx(hProcess, nullptr, (location.size() + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (allocated_mem == nullptr) {
			return false;
		}
		const auto is_write_success = ::WriteProcessMemory(hProcess, allocated_mem, location.data(), (location.size() + 1) * sizeof(wchar_t), 0);
		if (is_write_success == FALSE) {
			return false;
		}
		ScopeHandle<> target_thread = ::CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryW, allocated_mem, 0, 0);
		if (target_thread == false) {
			return false;
		}

		const auto wait_result = ::WaitForSingleObject(target_thread, INFINITE);
		if (wait_result == WAIT_FAILED) {
			return false;
		}

		DWORD exitCode;
		if (::GetExitCodeThread(target_thread, &exitCode) && exitCode == FALSE) {
			return false;
		}
		const auto is_memory_freed = ::VirtualFreeEx(hProcess, allocated_mem, 0, MEM_RELEASE);
		if (is_memory_freed == false) {
			return false;
		}
		return true;
	}

	bool RemoveDLL(DWORD th32ProcessID, std::string_view dll_name)
	{
		return RemoveDLL(OpenProcessHandle(th32ProcessID).get(), dll_name);
	}

	bool RemoveDLL(DWORD th32ProcessID, std::wstring_view dll_name)
	{
		return RemoveDLL(OpenProcessHandle(th32ProcessID).get(), dll_name);
	}

	bool RemoveDLL(HANDLE hProcess, std::string_view dll_name)
	{
		return RemoveDLL(hProcess, UTF8ToUTF16(dll_name));
	}

	bool RemoveDLL(HANDLE hProcess, std::wstring_view sDLLName)
	{
		auto pModule = SearchModule(GetProcessId(hProcess),sDLLName);

		const auto allocated_memory = ::VirtualAllocEx(hProcess, nullptr, sizeof(HMODULE), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (allocated_memory == nullptr) {
			return false;
		}
		const auto is_write_success = ::WriteProcessMemory(hProcess, allocated_memory, &pModule->hModule, sizeof(HMODULE), 0);
		if (is_write_success == FALSE) {
			return false;
		}
		ScopeHandle<> target_thread = ::CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeLibrary, allocated_memory, 0, 0);
		if (target_thread == false) {
			return false;
		}

		const auto wait_result = ::WaitForSingleObject(target_thread, INFINITE);
		if (wait_result == WAIT_FAILED) {
			return false;
		}
		DWORD exitCode;
		if (::GetExitCodeThread(target_thread, &exitCode) && exitCode == FALSE) {
			return false;
		}

		const auto is_memory_freed = ::VirtualFreeEx(hProcess, allocated_memory, 0, MEM_RELEASE);
		if (is_memory_freed == false) {
			return false;
		}
		return true;

	}
}