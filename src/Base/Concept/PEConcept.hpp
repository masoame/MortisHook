#pragma once
#include <base_concept.hpp>
#include <PE_types.hpp>

namespace Mortis::BC
{
	template<typename F>
	concept ConceptFunType = requires() {
		requires std::is_function_v<std::remove_pointer_t<F>> or std::is_same_v<F, TYPE::FuncPtr> or std::is_pointer_v<F>;
	};
}

