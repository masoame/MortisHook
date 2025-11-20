#pragma once
#include <utils.hpp>
#include <PE_types.hpp>
#include <PEB.h>

#include <ProcessImpl.hpp>

namespace Mortis::API
{
	using namespace Mortis::TYPE;
}

namespace Mortis::API
{
	template<typename TPtr>
		requires(std::is_pointer_v<TPtr>)
	struct MoAddress 
	{
		TPtr& _address;
		explicit MoAddress(TPtr& refAddress) : _address(refAddress) { }

		MoAddress& offset(std::ptrdiff_t offset) {
			_address = OffsetAddress(_address, offset);
		}

		MoAddress& sumOffset(LPVOID offset) {
			_address = offset(_address,(std::ptrdiff_t) offset);
		}

		MoAddress& subOffset(LPVOID offset) {
			_address = offset(_address, -(std::ptrdiff_t)offset);
		}

		TPtr get() {
			return _address;
		}
		operator TPtr&() {
			return _address;
		}
	};

	TYPE::PPEB GetPEB();
	TYPE::PTEB GetTEB();

	auto GetDosAndNtHeader(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>;
	auto GetDosAndNtHeader(HMODULE BaseAddress = nullptr)
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>;

	auto GetDosHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_DOS_HEADER>;

	auto GetNtHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_NT_HEADERS>;
	auto GetNtHeader(const IMAGE_DOS_HEADER& dos)
		-> Expected<PIMAGE_NT_HEADERS>;

	auto GetFileHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_FILE_HEADER>;
	auto GetFileHeader(IMAGE_NT_HEADERS& nt)
		-> PIMAGE_FILE_HEADER;
	auto GetFileHeader(const IMAGE_NT_HEADERS& nt)
		-> const IMAGE_FILE_HEADER*;

	auto GetOptHeader(HMODULE BaseAddress = nullptr)
		-> Expected<PIMAGE_OPTIONAL_HEADER>;
	auto GetOptHeader(IMAGE_NT_HEADERS& nt)
		-> Expected<PIMAGE_OPTIONAL_HEADER>;

	auto GetSecSpan(HMODULE BaseAddress)
		-> Expected<std::span<IMAGE_SECTION_HEADER>>;
	auto GetSecSpan(const IMAGE_NT_HEADERS& nt)
		-> std::span<IMAGE_SECTION_HEADER>;

	auto GetLastSec(HMODULE BaseAddress)
		-> Expected<PIMAGE_SECTION_HEADER>;
	auto GetLastSec(const IMAGE_NT_HEADERS& nt)
		-> PIMAGE_SECTION_HEADER;

	auto GetSecByName(HMODULE BaseAddress, std::string_view sName)
		-> Expected<PIMAGE_SECTION_HEADER>;
	auto GetSecByName(const IMAGE_NT_HEADERS& nt, std::string_view sName)
		-> Expected<PIMAGE_SECTION_HEADER>;

	auto AlignMent(std::size_t size, std::size_t alignment)
		-> std::size_t;
	auto IsDebuggerPresent() 
		-> bool;
}