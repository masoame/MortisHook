#include <MoHook.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace Mortis::API;
extern"C" __declspec(dllexport) void masoame() { }


TEST_CASE("PE API","[Mo::Hook]") {

	auto hModule = LoadLibraryExA("MoHookTestDLL.dll",nullptr, DONT_RESOLVE_DLL_REFERENCES);

	CHECK_NOTHROW([]() {
		auto hModule1 = GetModuleHandleW(L"MoHookTestDLL.dll");
		auto hModule2 = GetModuleHandleByName("MoHookTestDLL.dll");
		auto hModule3 = GetModuleHandleByName(L"MoHookTestDLL.dll");
		return hModule1 == hModule2.value() && hModule1 == hModule3.value();
	}());

	SECTION("Func: GetProcAddressEx") {

		using TFTest1 = std::add_pointer_t<int()>;
		using TFTest2 = std::add_pointer_t<const char* ()>;
		using TFTest3 = std::add_pointer_t<const wchar_t* ()>;

		auto OriginTest1 = (TFTest1)GetProcAddress(hModule, "Test1");
		auto OriginTest2 = (TFTest2)GetProcAddress(hModule, "Test2");
		auto OriginTest3 = (TFTest3)GetProcAddress(hModule, "Test3");

		TFTest1 Test1 = nullptr;
		TFTest2 Test2 = nullptr;
		TFTest3 Test3 = nullptr;

		REQUIRE_NOTHROW([&] {
			Test1 = GetProcAddressEx<TFTest1>("Test1", hModule).value();
			Test2 = GetProcAddressEx<TFTest2>("Test2", hModule).value();
			Test3 = GetProcAddressEx<TFTest3>("Test3", hModule).value();
		}());


		REQUIRE([&]() {
			return OriginTest1 == Test1
				&& OriginTest2 == Test2
				&& OriginTest3 == Test3;
		}());

		CHECK([&]() {
			return OriginTest1() == Test1()
				&& std::string(OriginTest2()) == std::string(Test2())
				&& std::wstring(OriginTest3()) == std::wstring(Test3());
		}());
	}
}

TEST_CASE("PE API", "[Mo::ProcessImpl]") {
	auto pi = CreateProcessByCommand("Notepad.exe");
	CHECK(pi.has_value());

	if (pi.has_value() == false) {
		spdlog::error("CreateProcessHandle Notepad.exe Failed");
		return;
	}
	spdlog::info("CreateProcessHandle Notepad.exe Success PID={}", pi->dwProcessId);
	if (ResumeThread(pi->hThread) == -1) {
		spdlog::error("ResumeThread Failed");
		return;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(250));
	auto moduleArr =  GetModuleInfo(GetProcessId(pi->hProcess));
	for(const auto& module : moduleArr) {
		spdlog::info("Module Name:{} \t\t Size:{}",std::string_view{ module.szModule },module.modBaseSize);
	}
	auto threadArr = GetThreadInfo(GetProcessId(pi->hProcess));
	for(const auto& thread : threadArr) {
		spdlog::info("Thread ID:{} \t\t Owner Process ID:{}", thread.th32ThreadID,thread.th32OwnerProcessID);
	}

	CHECK(InjectDLL(pi->hProcess, L"MoHookTestDLL.dll"));
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
	CHECK(RemoveDLL(pi->hProcess, L"MoHookTestDLL.dll"));
}
