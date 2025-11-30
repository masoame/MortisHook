#include <utils.hpp>
#include <MoHook.hpp>

#include<iostream>

MAKE_EXTERN_DLL;

extern"C" __declspec(dllexport) auto Test1() {
	return 8564;
}

extern"C" __declspec(dllexport) auto Test2() {
	return "masoame";
}

extern"C" __declspec(dllexport) auto Test3() {
	return L"masoame";
}

__declspec(dllexport) auto Test4() {
	return std::make_unique<long long>(0x569423);
}

class DLLMaker : public Mortis::BaseDLL<DLLMaker> 
{
	MAKE_DEFAULT_DLLMAKER;
	friend BOOL __stdcall ::DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
protected:
	
	bool Initialize([[maybe_unused]] const HMODULE& hModule, [[maybe_unused]] PCONTEXT pCtx) {
		return true;
	}
	bool Uninitialize([[maybe_unused]] const HMODULE& hModule, [[maybe_unused]] PCONTEXT pCtx) {
		return true;

	}
	bool Listen_Thread_Create([[maybe_unused]] const HMODULE& hModule) {
		return true;
	}
	bool Listen_Thread_Destroy([[maybe_unused]] const HMODULE& hModule) {
		return true;
	}
};
DEFAULT_MAKE_DLL(DLLMaker);

extern"C" __declspec(dllexport) void masoame() { }
