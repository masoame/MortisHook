#include <utils.hpp>
#include <MoHook.hpp>

#include<iostream>

extern BOOL __stdcall DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

void TestFunction() {
	using F = void (*)();
	auto expDosAndNt = Mo::API::GetProcAddressEx<F>("masoame");
	if (expDosAndNt.has_value() == false) {
		spdlog::info(expDosAndNt.error());
		return;
	}
	expDosAndNt.value()();
}

class DLLMaker : public Mortis::BaseDLL<DLLMaker> 
{
	friend Mortis::BaseDLL<DLLMaker>;
	friend BOOL __stdcall ::DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
protected:
	
	bool Initialize(const HMODULE& hModule, PCONTEXT pCtx) {
		TestFunction();
		return true;
	}
	bool Uninitialize(const HMODULE& hModule, PCONTEXT pCtx) {
		return true;

	}
	bool Listen_Thread_Create(const HMODULE& hModule) {
		return true;

	}
	bool Listen_Thread_Destroy(const HMODULE& hModule) {
		return true;

	}
};
DEFAULT_MAKE_DLL(DLLMaker);

extern"C" __declspec(dllexport) void masoame() { }
