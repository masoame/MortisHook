#include <MoHook.hpp>

using namespace Mortis::API;
extern"C" __declspec(dllexport) void masoame() { }

void test1();

int main() 
{
	test1();
}

void test1()
{
	auto a = LoadLibraryA("Mo-PE-DLLMain.dll");
	if (a == nullptr) {
		spdlog::error("LoadError");
	}

	auto expStr = ShowTable(a);
	if (expStr.has_value() == false) {
		spdlog::info(expStr.error());
		return ;
	}
	spdlog::info(expStr.value());
	spdlog::info("IsDebugged : {}", Mo::API::IsDebuggerPresent());
}

//static auto TrueMessageBoxA = MessageBoxA;
//
//int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
//{
//	spdlog::info("");
//	return TrueMessageBoxA(hWnd, "???", ">>>", MB_ICONERROR);
//}
//
//using namespace Mortis;
//
//int main()
//{
//	Hooker::Instance().TransactionBegin();
//	LONG error = Hooker::Instance().Attach<HookedMessageBoxA>(&TrueMessageBoxA);
//	//LONG error = DetourAttach(&(PVOID&)TrueMessageBoxA, HookedMessageBoxA);
//	if (error == NO_ERROR) {
//		spdlog::info("[STATUS] Successfully attached MessageBoxA hook.");
//	}
//	DetourTransactionCommit();
//	MessageBoxA(NULL, "This is the original message text.", "Original Caption", MB_OKCANCEL | MB_ICONINFORMATION);
//
//
//
//	DetourTransactionBegin();
//	DetourDetach(&(PVOID&)TrueMessageBoxA, HookedMessageBoxA);
//	DetourTransactionCommit();
//	MessageBoxA(NULL, "This message is displayed AFTER detaching the hook.", "Original Function", MB_OK);
//
//	return 0;
//}