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
		TPtr _address;
		explicit MoAddress(TPtr refAddress) : _address(refAddress) { }

		MoAddress& offset(std::ptrdiff_t nOffset) {
			_address = OffsetAddress(_address, nOffset);
			return *this;
		}

		MoAddress& sumOffset(LPVOID nOffset) {
			return offset(reinterpret_cast<std::ptrdiff_t>(nOffset));
		}

		MoAddress& subOffset(LPVOID nOffset) {
			return offset(-reinterpret_cast<std::ptrdiff_t>(nOffset));
		}

		template<typename TR>
		TR get() {
			return reinterpret_cast<TR>(_address);
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
		-> PIMAGE_OPTIONAL_HEADER;

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


	template<typename TDecoder>
	class BaseDecoder : StaticHelper {
		static auto Encry() { return TDecoder::Encry() }
	};

	template<typename TBaseDecoder = void>
		requires(std::is_base_of_v<BaseDecoder<TBaseDecoder>, TBaseDecoder>)
	class PEParser
	{
	protected:
		PIMAGE_DOS_HEADER _pDosHeader;
		PIMAGE_NT_HEADERS _pNtHeader;
	public:
		PEParser(HMODULE hModule);

		auto imageBase() const { return reinterpret_cast<HMODULE>(_pDosHeader); }
		auto getDosHeader() const { return _pDosHeader; }
		auto getNtHeader() const { return _pNtHeader; }
		auto getFileHeader() const { return GetFileHeader(*_pNtHeader); }
		auto getOptHeader() const { return GetOptHeader(*_pNtHeader); }
	};
}