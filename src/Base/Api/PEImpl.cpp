#include <PEImpl.hpp>
#include <ProcessImpl.hpp>

namespace Mortis::API
{
	using namespace Mortis::TYPE;
	PPEB GetPEB() {
#ifdef _WIN64
		return (PPEB)__readgsqword(0x60);
#else
		return (PPEB)__readfsdword(0x30);
#endif
	}

	PTEB GetTEB() {
#ifdef _WIN64
		return (PTEB)__readgsqword(0x00);
#else
		return (PTEB)__readfsdword(0x00);
#endif
	}


	auto IsDebuggerPresent() -> bool {
		return GetPEB()->BeingDebugged;
	}

	auto GetDosAndNtHeader(HANDLE ProcessHandle, HMODULE BaseAddress)
		-> std::unique_ptr<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>
	{
		if (!ProcessHandle || !BaseAddress) {
			return nullptr;
		}

		auto FileHeader = std::make_unique<std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS>>();

		if ((ReadProcessMemory(ProcessHandle, BaseAddress, &FileHeader->first, sizeof(IMAGE_DOS_HEADER), 0) == false)
			|| (FileHeader->first.e_magic != 0x5A4D)) {
			return nullptr;
		}
		if (ReadProcessMemory(ProcessHandle, MakeAddress(BaseAddress, FileHeader->first.e_lfanew), &FileHeader->second, sizeof(IMAGE_NT_HEADERS), 0) == false
			|| (FileHeader->second.Signature != 0x4550)) {
			return nullptr;
		}
		return FileHeader;
	}

	auto GetDosAndNtHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<std::pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>>
	{
		auto dosAndNt = std::make_pair<PIMAGE_DOS_HEADER, PIMAGE_NT_HEADERS>(nullptr, nullptr);
		auto& [pDos, pNt] = dosAndNt;

		auto pPEB = GetPEB();
		pDos = BaseAddress ? reinterpret_cast<PIMAGE_DOS_HEADER>(BaseAddress) : reinterpret_cast<PIMAGE_DOS_HEADER>(pPEB->ImageBaseAddress);
		pNt = MakeAddress<PIMAGE_NT_HEADERS>(pDos, pDos->e_lfanew);

		if (pDos->e_magic != 0x5A4D || pNt->Signature != 0x4550) {
			return UnExpected("GetDosAndNtHeader Error!!!");
		}
		return dosAndNt;
	}

	auto GetDosHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<PIMAGE_DOS_HEADER>
	{
		auto pDos = BaseAddress ? reinterpret_cast<PIMAGE_DOS_HEADER>(BaseAddress) : reinterpret_cast<PIMAGE_DOS_HEADER>(GetPEB()->ImageBaseAddress);
		if (pDos->e_magic != 0x5A4D) {
			return UnExpected("GetDosHeader Error!!!");
		}
		return pDos;
	}

	auto GetNtHeader(HMODULE BaseAddress /*= nullptr*/) 
		-> Expected<PIMAGE_NT_HEADERS>
	{
		auto expDos = GetDosHeader(BaseAddress);
		if (expDos.has_value() == false) {
			return UnExpected(expDos.error());
		}
		return GetNtHeader(*expDos.value());
	}
	auto GetNtHeader(const IMAGE_DOS_HEADER& dos) 
		-> Expected<PIMAGE_NT_HEADERS>
	{
		auto pNt = MakeAddress<PIMAGE_NT_HEADERS>(&dos, dos.e_lfanew);
		if (pNt->Signature != 0x4550) {
			return UnExpected("GetNtHeader Error!!!");
		}
		return pNt;
	}

	auto GetFileHeader(HMODULE BaseAddress /*= nullptr*/)
		-> Expected<PIMAGE_FILE_HEADER> 
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetFileHeader(*expNt.value());
	}
	auto GetFileHeader(IMAGE_NT_HEADERS& nt) 
		-> PIMAGE_FILE_HEADER {
		return &nt.FileHeader;
	}
	auto GetFileHeader(const IMAGE_NT_HEADERS& nt) 
		-> const IMAGE_FILE_HEADER* {
		return &nt.FileHeader;
	}

	auto GetOptHeader(HMODULE BaseAddress /*= nullptr*/) -> Expected<PIMAGE_OPTIONAL_HEADER>
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetOptHeader(*expNt.value());
	}

	auto GetOptHeader(IMAGE_NT_HEADERS& nt) 
		-> Expected<PIMAGE_OPTIONAL_HEADER> {
		return &nt.OptionalHeader;
	}

	auto GetSecSpan(HMODULE BaseAddress) 
		-> Expected<std::span<IMAGE_SECTION_HEADER>>
	{
		auto expNt = GetNtHeader(BaseAddress);
		if (expNt.has_value() == false) {
			return UnExpected(expNt.error());
		}
		return GetSecSpan(*expNt.value());
	}

	auto GetSecSpan(const IMAGE_NT_HEADERS& nt) 
		-> Expected<std::span<IMAGE_SECTION_HEADER>>
	{
		auto pFileHeader = GetFileHeader(nt);
		auto SecNum = pFileHeader->NumberOfSections;
		return std::span<IMAGE_SECTION_HEADER>{	IMAGE_FIRST_SECTION(&nt), pFileHeader->NumberOfSections};
	}

	auto GetLastSec(HMODULE BaseAddress) 
		-> Expected<PIMAGE_SECTION_HEADER>
	{
		auto expSecSpan = GetSecSpan(BaseAddress);
		if (expSecSpan.has_value()) {
			return UnExpected(expSecSpan.error());
		}
		return &expSecSpan.value().back();
	}

	auto GetSecByName(HMODULE BaseAddress, std::string_view sName) 
		-> Expected<PIMAGE_SECTION_HEADER>
	{
		auto expSec = GetSecSpan(BaseAddress);
		if (expSec.has_value()) {
			return UnExpected(expSec.error());
		}
		for (auto& sec : expSec.value()) {
			char bufName[10] = { 0 };
			memcpy_s(bufName, 8, sec.Name, 8);
			if (std::string_view(bufName) == sName) {
				return &sec;
			}
		}
		return UnExpected("GetSecByName no found sec By Name");
	}

	int AlignMent(int size,int alignment) {
		return (size) % (alignment) == 0 ? (size) : ((size) / alignment + 1) * (alignment);
	}


	auto AddSection(HMODULE BaseAddress, std::string_view sName, DWORD dwSize, DWORD dwCharacteristics) 
		-> Expected<PIMAGE_SECTION_HEADER>
	{

	}

	char* AddSec(HMODULE hpe, _In_ DWORD& filesize, _In_ const char* secname, _In_ const int secsize) {

		GetFileHeader(hpe).value()->NumberOfSections++;
		PIMAGE_SECTION_HEADER pesec = GetLastSec(hpe).value();
		//设置区段表属性
		memcpy(pesec->Name, secname, 8);
		pesec->Misc.VirtualSize = secsize;
		pesec->VirtualAddress = (pesec - 1)->VirtualAddress + AlignMent((pesec - 1)->SizeOfRawData, GetOptHeader(hpe).value()->SectionAlignment);
		pesec->SizeOfRawData = AlignMent(secsize, GetOptHeader(hpe).value()->FileAlignment);
		pesec->PointerToRawData = AlignMent(filesize, GetOptHeader(hpe).value()->FileAlignment);
		pesec->Characteristics = 0xE00000E0;
		//设置OPT头映像大小
		GetOptHeader(hpe).value()->SizeOfImage = pesec->VirtualAddress + pesec->SizeOfRawData;
		//扩充文件数据
		int newSize = pesec->PointerToRawData + pesec->SizeOfRawData;
		char* nhpe = new char [newSize] {0};
		//向新缓冲区录入数据
		memcpy(nhpe, hpe, filesize);
		//缓存区更替
		delete hpe;
		filesize = newSize;
		return nhpe;
	}



}

BOOL PackFile();   //将要加壳的程序加密以后加入壳子中
VOID EncpyptFile(PUCHAR pBaseAddr, DWORD dwFileSize);   //对文件内容进行加密
BOOL AddSection(PVOID pShellBase, PVOID pOrgBase, DWORD dwShellFileSize, DWORD dwOrgFileSize);  //将源文件加到壳子中
VOID ShowError(PCHAR msg);    //弹出错误信息
DWORD Align(DWORD dwSize, DWORD dwAlign);   //用于对齐，返回对齐以后的数字


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (PackFile())
	{
		MessageBox(NULL, TEXT("加壳成功"), TEXT("成功"), MB_OK);
	}

	return 0;
}

BOOL PackFile()
{
	BOOL bRet = TRUE;
	HANDLE hOrgFile = NULL, hShellFile = NULL;
	DWORD dwOrgFileSize = 0, dwShellFileSize = 0, dwRet = 0, dwFileSize = 0;
	PVOID pOrgBase = NULL, pShellBase = NULL;

	hOrgFile = CreateFile(ORGIN_FILE_NAME,
		GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hOrgFile == INVALID_HANDLE_VALUE)
	{
		ShowError("CreateFile OrigFile");
		bRet = FALSE;
		goto exit;
	}
	dwOrgFileSize = GetFileSize(hOrgFile, NULL);

	pOrgBase = VirtualAlloc(NULL, dwOrgFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pOrgBase)
	{
		ShowError("VirtualAlloc pOrgBase");
		bRet = FALSE;
		goto exit;
	}

	ZeroMemory(pOrgBase, dwOrgFileSize);
	if (!ReadFile(hOrgFile, pOrgBase, dwOrgFileSize, &dwRet, NULL))
	{
		ShowError("ReadFile pOrgBase");
		bRet = FALSE;
		goto exit;
	}

	EncpyptFile((PUCHAR)pOrgBase, dwOrgFileSize);  //对源程序进行XOR加密

	hShellFile = CreateFile(SHELL_FILE_NAME,
		GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hShellFile == INVALID_HANDLE_VALUE)
	{
		ShowError("CreateFile ShellFile");
		bRet = FALSE;
		goto exit;
	}
	dwShellFileSize = GetFileSize(hShellFile, NULL);

	pShellBase = VirtualAlloc(NULL, dwShellFileSize + dwOrgFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pShellBase)
	{
		ShowError("VirtualAlloc pShellBase");
		bRet = FALSE;
		goto exit;
	}

	ZeroMemory(pShellBase, dwShellFileSize + dwOrgFileSize);
	if (!ReadFile(hShellFile, pShellBase, dwShellFileSize, &dwRet, NULL))
	{
		ShowError("ReadFile pShellBase");
		bRet = FALSE;
		goto exit;
	}

	if (!AddSection(pShellBase, pOrgBase, dwShellFileSize, dwOrgFileSize))
	{
		bRet = FALSE;
		goto exit;
	}
exit:
	if (hOrgFile) CloseHandle(hOrgFile);
	if (hShellFile) CloseHandle(hShellFile);
	if (pOrgBase)
	{
		if (!VirtualFree(pOrgBase, 0, MEM_DECOMMIT))
		{
			ShowError("VirtualFree pOrgBase");
		}
	}

	if (pShellBase)
	{
		if (!VirtualFree(pShellBase, 0, MEM_DECOMMIT))
		{
			ShowError("VirtualFree pShellBase");
		}
	}
	return bRet;
}

BOOL AddSection(PVOID pShellBase, PVOID pOrgBase, DWORD dwShellFileSize, DWORD dwOrgFileSize)
{
	BOOL bRet = TRUE;
	DWORD dwRet = 0;
	HANDLE hFile = NULL;
	PIMAGE_DOS_HEADER pShellDosHead = (PIMAGE_DOS_HEADER)pShellBase;
	PIMAGE_FILE_HEADER pShellFileHead = (PIMAGE_FILE_HEADER)((DWORD)pShellBase + pShellDosHead->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pShellOptionHead = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pShellFileHead + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pShellSectionHead = (PIMAGE_SECTION_HEADER)((DWORD)pShellOptionHead + pShellFileHead->SizeOfOptionalHeader);
	DWORD dwFileAlignment = pShellOptionHead->FileAlignment, dwSectionAlignment = pShellOptionHead->SectionAlignment;
	DWORD dwSize = 0;
	PVOID pResBase = NULL;

	if (pShellSectionHead[0].PointerToRawData - ((DWORD)&pShellSectionHead[pShellFileHead->NumberOfSections - 1] - (DWORD)pShellBase)
		< 2 * IMAGE_SIZEOF_SECTION_HEADER)
	{
		printf("壳子最后一个节表间隙不够\n");
		bRet = FALSE;
		goto exit;
	}

	dwSize = Align(dwOrgFileSize + dwShellFileSize, dwFileAlignment);  //dwFileAlignment整数倍
	pResBase = VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pResBase)
	{
		ShowError("VirtualAlloc pResBase");
		bRet = FALSE;
		goto exit;
	}

	ZeroMemory(pResBase, dwSize);
	memcpy((PVOID)pResBase, pShellBase, dwShellFileSize); //将源文件复制到壳子中
	memcpy((PVOID)((DWORD)pResBase + dwShellFileSize), pOrgBase, dwOrgFileSize);    //将源文件复制到壳子中

	PIMAGE_DOS_HEADER pResDosHead = (PIMAGE_DOS_HEADER)pResBase;
	PIMAGE_FILE_HEADER pResFileHead = (PIMAGE_FILE_HEADER)((DWORD)pResBase + pResDosHead->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pResOptionHead = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pResFileHead + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pResSectionHead = (PIMAGE_SECTION_HEADER)((DWORD)pResOptionHead + pResFileHead->SizeOfOptionalHeader);

	//增加节表
	ZeroMemory((PVOID)&pResSectionHead[pShellFileHead->NumberOfSections], IMAGE_SIZEOF_SECTION_HEADER);
	memcpy(pResSectionHead[pResFileHead->NumberOfSections].Name, ".1900", strlen(".1900"));
	pResSectionHead[pResFileHead->NumberOfSections].PointerToRawData = pResSectionHead[pShellFileHead->NumberOfSections - 1].PointerToRawData
		+ Align(pShellSectionHead[pShellFileHead->NumberOfSections - 1].SizeOfRawData, dwFileAlignment);
	pResSectionHead[pResFileHead->NumberOfSections].SizeOfRawData = Align(dwOrgFileSize, dwFileAlignment);
	pResSectionHead[pResFileHead->NumberOfSections].VirtualAddress = pResSectionHead[pShellFileHead->NumberOfSections - 1].VirtualAddress
		+ Align(pResSectionHead[pShellFileHead->NumberOfSections - 1].Misc.VirtualSize, dwSectionAlignment);
	pResSectionHead[pResFileHead->NumberOfSections].Misc.VirtualSize = dwOrgFileSize;
	pResSectionHead[pResFileHead->NumberOfSections].Characteristics |= IMAGE_SCN_MEM_READ;

	pResFileHead->NumberOfSections++;    //增加节数量
	pResOptionHead->SizeOfImage += Align(dwOrgFileSize, dwSectionAlignment);  //增加SizeOfImage

	hFile = CreateFile(TARGET_FILE_NAME,      //打开目标文件并保存
		GENERIC_READ | GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError("CreateFile");
		bRet = FALSE;
		goto exit;
	}

	if (!WriteFile(hFile, pResBase, dwSize, &dwRet, NULL))
	{
		ShowError("WriteFile");
		bRet = FALSE;
		goto exit;
	}

exit:
	if (hFile) CloseHandle(hFile);
	return bRet;
}

DWORD Align(DWORD dwSize, DWORD dwAlign)
{
	return dwSize % dwAlign ? dwSize + dwAlign - dwSize % dwAlign : dwSize;
}

VOID EncpyptFile(PUCHAR pBaseAddr, DWORD dwFileSize)
{
	DWORD i = 0;
	UCHAR uKey = 190;    //加密密钥

	for (i = 0; i < dwFileSize; i++)
	{
		if (pBaseAddr[i] && pBaseAddr[i] != uKey) pBaseAddr[i] ^= uKey;
	}
}

VOID ShowError(PCHAR msg)
{
	CHAR szError[105] = { 0 };

	sprintf(szError, "%s Error %d", msg, GetLastError());
	MessageBox(NULL, szError, TEXT("Error"), MB_OK);
}