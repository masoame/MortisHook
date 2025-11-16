#pragma once
#include <MoHook.hpp>

namespace Mortis
{
	struct HookPackage
	{
		void* oldPtr;
	};

	class Hooker : public Singleton<Hooker>
	{
		friend Singleton<Hooker>;
	protected:
		Hooker() {};
	public:
		auto TransactionBegin() const { return DetourTransactionBegin(); };
		auto TransactionCommit() const { return DetourTransactionCommit(); };

		template<auto f, typename TFun = decltype(f), typename TFunAdapter = std::add_pointer_t<std::remove_pointer_t<TFun>>>
			requires(BC::ConceptFunType<TFun>)
		auto Attach(TFunAdapter* sf) {
			return ::DetourAttach((LPVOID*)sf, f);
		}
	};
}
