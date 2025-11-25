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
		explicit MoAddress(TPtr refAddress) : _address(refAddress) {}

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
		operator TPtr& () {
			return _address;
		}
	};

	PPEB GetPEB() noexcept;
	PTEB GetTEB() noexcept;

	auto GeLDRNameMap()
		-> Expected<std::map<std::wstring_view, PLDR_DATA_TABLE_ENTRY>>;

	auto GetModuleHandleByName(std::wstring_view sModuleName)
		-> Expected<HMODULE>;

	auto GetModuleHandleByName(std::string_view sModuleName)
		-> Expected<HMODULE>;

	auto GetDosAndNtHeader(HANDLE ProcessHandle, HMODULE BaseAddress) noexcept
		-> std::unique_ptr<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>;
	auto GetDosAndNtHeader(HMODULE BaseAddress = nullptr) noexcept
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>;

	auto GetDosHeader(HMODULE BaseAddress = nullptr) noexcept
		-> Expected<PIMAGE_DOS_HEADER>;

	auto GetNtHeader(HMODULE BaseAddress = nullptr) noexcept
		-> Expected<PIMAGE_NT_HEADERS>;
	auto GetNtHeader(const IMAGE_DOS_HEADER& dos) noexcept
		-> Expected<PIMAGE_NT_HEADERS>;

	auto GetFileHeader(HMODULE BaseAddress = nullptr) noexcept
		-> Expected<PIMAGE_FILE_HEADER>;
	auto GetFileHeader(IMAGE_NT_HEADERS& nt) noexcept
		-> PIMAGE_FILE_HEADER;
	auto GetFileHeader(const IMAGE_NT_HEADERS& nt) noexcept
		-> const IMAGE_FILE_HEADER*;

	auto GetOptHeader(HMODULE BaseAddress = nullptr) noexcept
		-> Expected<PIMAGE_OPTIONAL_HEADER>;
	auto GetOptHeader(IMAGE_NT_HEADERS& nt) noexcept
		-> PIMAGE_OPTIONAL_HEADER;

	auto GetSecSpan(HMODULE BaseAddress) noexcept
		-> Expected<std::span<IMAGE_SECTION_HEADER>>;
	auto GetSecSpan(const IMAGE_NT_HEADERS& nt) noexcept
		-> std::span<IMAGE_SECTION_HEADER>;

	auto GetLastSec(HMODULE BaseAddress) noexcept
		-> Expected<PIMAGE_SECTION_HEADER>;
	auto GetLastSec(const IMAGE_NT_HEADERS& nt) noexcept
		-> PIMAGE_SECTION_HEADER;

	auto GetSecByName(HMODULE BaseAddress, std::string_view sName) noexcept
		-> Expected<PIMAGE_SECTION_HEADER>;
	auto GetSecByName(const IMAGE_NT_HEADERS& nt, std::string_view sName) noexcept
		-> Expected<PIMAGE_SECTION_HEADER>;

	auto AlignMent(std::size_t size, std::size_t alignment) noexcept
		-> std::size_t;
	auto IsDebuggerPresent() noexcept
		-> bool;


	template<typename TDecoder>
	class BaseDecoder : StaticHelper {
		static std::vector<char> Decrypt(std::span<char> imagefile) { return TDecoder::Decrypt(imagefile); }
	};
	class NoneDecoder : public BaseDecoder<NoneDecoder>
	{
		static std::vector<char> Decrypt(std::span<char> imagefile) { return { imagefile.cbegin(), imagefile.cend() }; }
	};


	class BasePEParser
	{
		std::vector<char> _cache;
	protected:
		PIMAGE_DOS_HEADER _pDosHeader = nullptr;
		PIMAGE_NT_HEADERS _pNtHeader = nullptr;
	public:
		static std::vector<char> LoadFile(std::string_view peFilePath);

		template<typename TBaseDecoder>
			requires(std::is_base_of_v<BaseDecoder<TBaseDecoder>, TBaseDecoder>)
		BasePEParser(HMODULE hModule, std::size_t file_size);
		template<typename TBaseDecoder>
			requires(std::is_base_of_v<BaseDecoder<TBaseDecoder>, TBaseDecoder>)
		BasePEParser(std::string_view peFilePath) :
			BasePEParser(LoadFile(peFilePath))
		{ }
		template<typename TBaseDecoder>
			requires(std::is_base_of_v<BaseDecoder<TBaseDecoder>, TBaseDecoder>)
		BasePEParser(const std::vector<char>& peFilePath) :
			BasePEParser(reinterpret_cast<HMODULE>(const_cast<char*>(peFilePath.data())), peFilePath.size())
		{ }

		auto imageBase() const { return reinterpret_cast<HMODULE>(_pDosHeader); }
		auto getDosHeader() const { return _pDosHeader; }
		auto getNtHeader() const { return _pNtHeader; }
		auto getFileHeader() const { return GetFileHeader(*_pNtHeader); }
		auto getOptHeader() const { return GetOptHeader(*_pNtHeader); }
	};

	template<typename TBaseDecoder>
		requires(std::is_base_of_v<BaseDecoder<TBaseDecoder>, TBaseDecoder>)
	BasePEParser::BasePEParser(HMODULE hModule, std::size_t file_size)
	{
		_cache = TBaseDecoder::Decrypt({ reinterpret_cast<char*>(hModule) , file_size });
		auto expDosAndNt = GetDosAndNtHeader(_cache.empty() ? hModule : reinterpret_cast<HMODULE>(_cache.data()));
		if (expDosAndNt.has_value() == false) {
			throw std::runtime_error(expDosAndNt.error().data());
		}
		auto& [pDos, pNt] = expDosAndNt.value();
		_pDosHeader = pDos;
		_pNtHeader = pNt;
	}


}