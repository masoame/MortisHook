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

	auto CreateProcessHandle(std::wstring_view file_path, DWORD dwCreationFlags = CREATE_SUSPENDED)
		-> std::pair<ScopeHandle<>, ScopeHandle<>>
	{
		STARTUPINFOW si{};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi{};
		BOOL ret = CreateProcessW(file_path.data(), nullptr, nullptr, nullptr, false, dwCreationFlags, nullptr, nullptr, &si, &pi);
		return ret ? make_pair(ScopeHandle<>(pi.hProcess), ScopeHandle<>(pi.hThread)) : make_pair(ScopeHandle<>(nullptr), ScopeHandle<>(nullptr));
	}

	auto resumeThread(const ScopeHandle<>& threadHandle)
		-> bool
	{
		return threadHandle && (ResumeThread(threadHandle) != static_cast<DWORD>(-1));
	}

	auto ThreadInfo(DWORD th32ProcessID)
		-> std::vector<THREADENTRY32>
	{
		ScopeHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, th32ProcessID);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return {};
		}

		std::vector<THREADENTRY32> info{};
		THREADENTRY32 thread_entry_cache{};
		thread_entry_cache.dwSize = sizeof(THREADENTRY32);

		BOOL bFound = Thread32First(hProcessSnap, &thread_entry_cache);
		while (bFound) {
			info.emplace_back(thread_entry_cache);
			bFound = Thread32Next(hProcessSnap, &thread_entry_cache);
		}
		return info;
	}
}