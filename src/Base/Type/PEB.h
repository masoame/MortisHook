#pragma once
#include <utils.hpp>

namespace Mortis::TYPE
{
	struct _PEB32;
	struct _PEB64;
	struct _TEB32;
	struct _TEB64;

#ifndef _WIN64
	using PEB = _PEB32;
	using PPEB = _PEB32*;
	using TEB = _TEB32;
	using PTEB = _TEB32*;
#else
	using PEB = _PEB64;
	using PPEB = _PEB64*;
	using TEB = _TEB64;
	using PTEB = _TEB64*;
#endif

	typedef struct _STRING32 {
		USHORT   Length;
		USHORT   MaximumLength;
		ULONG  Buffer;
	} STRING32;
	typedef STRING32* PSTRING32;
	typedef STRING32 UNICODE_STRING32;
	typedef UNICODE_STRING32* PUNICODE_STRING32;
	typedef STRING32 ANSI_STRING32;
	typedef ANSI_STRING32* PANSI_STRING32;



	typedef struct _STRING64 {
		USHORT   Length;
		USHORT   MaximumLength;
		ULONGLONG  Buffer;
	} STRING64;
	typedef STRING64* PSTRING64;
	typedef STRING64 UNICODE_STRING64;
	typedef UNICODE_STRING64* PUNICODE_STRING64;
	typedef STRING64 ANSI_STRING64;
	typedef ANSI_STRING64* PANSI_STRING64;

	//0x488 bytes (sizeof)
	struct _PEB32
	{
		UCHAR InheritedAddressSpace;                                            //0x0
		UCHAR ReadImageFileExecOptions;                                         //0x1
		UCHAR BeingDebugged;                                                    //0x2
		union
		{
			UCHAR BitField;                                                     //0x3
			struct
			{
				UCHAR ImageUsesLargePages : 1;                                    //0x3
				UCHAR IsProtectedProcess : 1;                                     //0x3
				UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
				UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
				UCHAR IsPackagedProcess : 1;                                      //0x3
				UCHAR IsAppContainer : 1;                                         //0x3
				UCHAR IsProtectedProcessLight : 1;                                //0x3
				UCHAR IsLongPathAwareProcess : 1;                                 //0x3
			};
		};
		ULONG Mutant;                                                           //0x4
		ULONG ImageBaseAddress;                                                 //0x8
		ULONG Ldr;                                                              //0xc
		ULONG ProcessParameters;                                                //0x10
		ULONG SubSystemData;                                                    //0x14
		ULONG ProcessHeap;                                                      //0x18
		ULONG FastPebLock;                                                      //0x1c
		ULONG AtlThunkSListPtr;                                                 //0x20
		ULONG IFEOKey;                                                          //0x24
		union
		{
			ULONG CrossProcessFlags;                                            //0x28
			struct
			{
				ULONG ProcessInJob : 1;                                           //0x28
				ULONG ProcessInitializing : 1;                                    //0x28
				ULONG ProcessUsingVEH : 1;                                        //0x28
				ULONG ProcessUsingVCH : 1;                                        //0x28
				ULONG ProcessUsingFTH : 1;                                        //0x28
				ULONG ProcessPreviouslyThrottled : 1;                             //0x28
				ULONG ProcessCurrentlyThrottled : 1;                              //0x28
				ULONG ProcessImagesHotPatched : 1;                                //0x28
				ULONG ReservedBits0 : 24;                                         //0x28
			};
		};
		union
		{
			ULONG KernelCallbackTable;                                          //0x2c
			ULONG UserSharedInfoPtr;                                            //0x2c
		};
		ULONG SystemReserved;                                                   //0x30
		ULONG AtlThunkSListPtr32;                                               //0x34
		ULONG ApiSetMap;                                                        //0x38
		ULONG TlsExpansionCounter;                                              //0x3c
		ULONG TlsBitmap;                                                        //0x40
		ULONG TlsBitmapBits[2];                                                 //0x44
		ULONG ReadOnlySharedMemoryBase;                                         //0x4c
		ULONG SharedData;                                                       //0x50
		ULONG ReadOnlyStaticServerData;                                         //0x54
		ULONG AnsiCodePageData;                                                 //0x58
		ULONG OemCodePageData;                                                  //0x5c
		ULONG UnicodeCaseTableData;                                             //0x60
		ULONG NumberOfProcessors;                                               //0x64
		ULONG NtGlobalFlag;                                                     //0x68
		union _LARGE_INTEGER CriticalSectionTimeout;                            //0x70
		ULONG HeapSegmentReserve;                                               //0x78
		ULONG HeapSegmentCommit;                                                //0x7c
		ULONG HeapDeCommitTotalFreeThreshold;                                   //0x80
		ULONG HeapDeCommitFreeBlockThreshold;                                   //0x84
		ULONG NumberOfHeaps;                                                    //0x88
		ULONG MaximumNumberOfHeaps;                                             //0x8c
		ULONG ProcessHeaps;                                                     //0x90
		ULONG GdiSharedHandleTable;                                             //0x94
		ULONG ProcessStarterHelper;                                             //0x98
		ULONG GdiDCAttributeList;                                               //0x9c
		ULONG LoaderLock;                                                       //0xa0
		ULONG OSMajorVersion;                                                   //0xa4
		ULONG OSMinorVersion;                                                   //0xa8
		USHORT OSBuildNumber;                                                   //0xac
		USHORT OSCSDVersion;                                                    //0xae
		ULONG OSPlatformId;                                                     //0xb0
		ULONG ImageSubsystem;                                                   //0xb4
		ULONG ImageSubsystemMajorVersion;                                       //0xb8
		ULONG ImageSubsystemMinorVersion;                                       //0xbc
		ULONG ActiveProcessAffinityMask;                                        //0xc0
		ULONG GdiHandleBuffer[34];                                              //0xc4
		ULONG PostProcessInitRoutine;                                           //0x14c
		ULONG TlsExpansionBitmap;                                               //0x150
		ULONG TlsExpansionBitmapBits[32];                                       //0x154
		ULONG SessionId;                                                        //0x1d4
		union _ULARGE_INTEGER AppCompatFlags;                                   //0x1d8
		union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x1e0
		ULONG pShimData;                                                        //0x1e8
		ULONG AppCompatInfo;                                                    //0x1ec
		struct _STRING32 CSDVersion;                                            //0x1f0
		ULONG ActivationContextData;                                            //0x1f8
		ULONG ProcessAssemblyStorageMap;                                        //0x1fc
		ULONG SystemDefaultActivationContextData;                               //0x200
		ULONG SystemAssemblyStorageMap;                                         //0x204
		ULONG MinimumStackCommit;                                               //0x208
		ULONG SparePointers[2];                                                 //0x20c
		ULONG PatchLoaderData;                                                  //0x214
		ULONG ChpeV2ProcessInfo;                                                //0x218
		ULONG AppModelFeatureState;                                             //0x21c
		ULONG SpareUlongs[2];                                                   //0x220
		USHORT ActiveCodePage;                                                  //0x228
		USHORT OemCodePage;                                                     //0x22a
		USHORT UseCaseMapping;                                                  //0x22c
		USHORT UnusedNlsField;                                                  //0x22e
		ULONG WerRegistrationData;                                              //0x230
		ULONG WerShipAssertPtr;                                                 //0x234
		ULONG Spare;                                                            //0x238
		ULONG pImageHeaderHash;                                                 //0x23c
		union
		{
			ULONG TracingFlags;                                                 //0x240
			struct
			{
				ULONG HeapTracingEnabled : 1;                                     //0x240
				ULONG CritSecTracingEnabled : 1;                                  //0x240
				ULONG LibLoaderTracingEnabled : 1;                                //0x240
				ULONG SpareTracingBits : 29;                                      //0x240
			};
		};
		ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x248
		ULONG TppWorkerpListLock;                                               //0x250
		struct LIST_ENTRY32 TppWorkerpList;                                     //0x254
		ULONG WaitOnAddressHashTable[128];                                      //0x25c
		ULONG TelemetryCoverageHeader;                                          //0x45c
		ULONG CloudFileFlags;                                                   //0x460
		ULONG CloudFileDiagFlags;                                               //0x464
		CHAR PlaceholderCompatibilityMode;                                      //0x468
		CHAR PlaceholderCompatibilityModeReserved[7];                           //0x469
		ULONG LeapSecondData;                                                   //0x470
		union
		{
			ULONG LeapSecondFlags;                                              //0x474
			struct
			{
				ULONG SixtySecondEnabled : 1;                                     //0x474
				ULONG Reserved : 31;                                              //0x474
			};
		};
		ULONG NtGlobalFlag2;                                                    //0x478
		ULONGLONG ExtendedFeatureDisableMask;                                   //0x480
	};



	//0x7d0 bytes (sizeof)
	struct _PEB64
	{
		UCHAR InheritedAddressSpace;                                            //0x0
		UCHAR ReadImageFileExecOptions;                                         //0x1
		UCHAR BeingDebugged;                                                    //0x2
		union
		{
			UCHAR BitField;                                                     //0x3
			struct
			{
				UCHAR ImageUsesLargePages : 1;                                    //0x3
				UCHAR IsProtectedProcess : 1;                                     //0x3
				UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
				UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
				UCHAR IsPackagedProcess : 1;                                      //0x3
				UCHAR IsAppContainer : 1;                                         //0x3
				UCHAR IsProtectedProcessLight : 1;                                //0x3
				UCHAR IsLongPathAwareProcess : 1;                                 //0x3
			};
		};
		UCHAR Padding0[4];                                                      //0x4
		ULONGLONG Mutant;                                                       //0x8
		ULONGLONG ImageBaseAddress;                                             //0x10
		ULONGLONG Ldr;                                                          //0x18
		ULONGLONG ProcessParameters;                                            //0x20
		ULONGLONG SubSystemData;                                                //0x28
		ULONGLONG ProcessHeap;                                                  //0x30
		ULONGLONG FastPebLock;                                                  //0x38
		ULONGLONG AtlThunkSListPtr;                                             //0x40
		ULONGLONG IFEOKey;                                                      //0x48
		union
		{
			ULONG CrossProcessFlags;                                            //0x50
			struct
			{
				ULONG ProcessInJob : 1;                                           //0x50
				ULONG ProcessInitializing : 1;                                    //0x50
				ULONG ProcessUsingVEH : 1;                                        //0x50
				ULONG ProcessUsingVCH : 1;                                        //0x50
				ULONG ProcessUsingFTH : 1;                                        //0x50
				ULONG ProcessPreviouslyThrottled : 1;                             //0x50
				ULONG ProcessCurrentlyThrottled : 1;                              //0x50
				ULONG ProcessImagesHotPatched : 1;                                //0x50
				ULONG ReservedBits0 : 24;                                         //0x50
			};
		};
		UCHAR Padding1[4];                                                      //0x54
		union
		{
			ULONGLONG KernelCallbackTable;                                      //0x58
			ULONGLONG UserSharedInfoPtr;                                        //0x58
		};
		ULONG SystemReserved;                                                   //0x60
		ULONG AtlThunkSListPtr32;                                               //0x64
		ULONGLONG ApiSetMap;                                                    //0x68
		ULONG TlsExpansionCounter;                                              //0x70
		UCHAR Padding2[4];                                                      //0x74
		ULONGLONG TlsBitmap;                                                    //0x78
		ULONG TlsBitmapBits[2];                                                 //0x80
		ULONGLONG ReadOnlySharedMemoryBase;                                     //0x88
		ULONGLONG SharedData;                                                   //0x90
		ULONGLONG ReadOnlyStaticServerData;                                     //0x98
		ULONGLONG AnsiCodePageData;                                             //0xa0
		ULONGLONG OemCodePageData;                                              //0xa8
		ULONGLONG UnicodeCaseTableData;                                         //0xb0
		ULONG NumberOfProcessors;                                               //0xb8
		ULONG NtGlobalFlag;                                                     //0xbc
		union _LARGE_INTEGER CriticalSectionTimeout;                            //0xc0
		ULONGLONG HeapSegmentReserve;                                           //0xc8
		ULONGLONG HeapSegmentCommit;                                            //0xd0
		ULONGLONG HeapDeCommitTotalFreeThreshold;                               //0xd8
		ULONGLONG HeapDeCommitFreeBlockThreshold;                               //0xe0
		ULONG NumberOfHeaps;                                                    //0xe8
		ULONG MaximumNumberOfHeaps;                                             //0xec
		ULONGLONG ProcessHeaps;                                                 //0xf0
		ULONGLONG GdiSharedHandleTable;                                         //0xf8
		ULONGLONG ProcessStarterHelper;                                         //0x100
		ULONG GdiDCAttributeList;                                               //0x108
		UCHAR Padding3[4];                                                      //0x10c
		ULONGLONG LoaderLock;                                                   //0x110
		ULONG OSMajorVersion;                                                   //0x118
		ULONG OSMinorVersion;                                                   //0x11c
		USHORT OSBuildNumber;                                                   //0x120
		USHORT OSCSDVersion;                                                    //0x122
		ULONG OSPlatformId;                                                     //0x124
		ULONG ImageSubsystem;                                                   //0x128
		ULONG ImageSubsystemMajorVersion;                                       //0x12c
		ULONG ImageSubsystemMinorVersion;                                       //0x130
		UCHAR Padding4[4];                                                      //0x134
		ULONGLONG ActiveProcessAffinityMask;                                    //0x138
		ULONG GdiHandleBuffer[60];                                              //0x140
		ULONGLONG PostProcessInitRoutine;                                       //0x230
		ULONGLONG TlsExpansionBitmap;                                           //0x238
		ULONG TlsExpansionBitmapBits[32];                                       //0x240
		ULONG SessionId;                                                        //0x2c0
		UCHAR Padding5[4];                                                      //0x2c4
		union _ULARGE_INTEGER AppCompatFlags;                                   //0x2c8
		union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x2d0
		ULONGLONG pShimData;                                                    //0x2d8
		ULONGLONG AppCompatInfo;                                                //0x2e0
		struct _STRING64 CSDVersion;                                            //0x2e8
		ULONGLONG ActivationContextData;                                        //0x2f8
		ULONGLONG ProcessAssemblyStorageMap;                                    //0x300
		ULONGLONG SystemDefaultActivationContextData;                           //0x308
		ULONGLONG SystemAssemblyStorageMap;                                     //0x310
		ULONGLONG MinimumStackCommit;                                           //0x318
		ULONGLONG SparePointers[2];                                             //0x320
		ULONGLONG PatchLoaderData;                                              //0x330
		ULONGLONG ChpeV2ProcessInfo;                                            //0x338
		ULONG AppModelFeatureState;                                             //0x340
		ULONG SpareUlongs[2];                                                   //0x344
		USHORT ActiveCodePage;                                                  //0x34c
		USHORT OemCodePage;                                                     //0x34e
		USHORT UseCaseMapping;                                                  //0x350
		USHORT UnusedNlsField;                                                  //0x352
		ULONGLONG WerRegistrationData;                                          //0x358
		ULONGLONG WerShipAssertPtr;                                             //0x360
		ULONGLONG EcCodeBitMap;                                                 //0x368
		ULONGLONG pImageHeaderHash;                                             //0x370
		union
		{
			ULONG TracingFlags;                                                 //0x378
			struct
			{
				ULONG HeapTracingEnabled : 1;                                     //0x378
				ULONG CritSecTracingEnabled : 1;                                  //0x378
				ULONG LibLoaderTracingEnabled : 1;                                //0x378
				ULONG SpareTracingBits : 29;                                      //0x378
			};
		};
		UCHAR Padding6[4];                                                      //0x37c
		ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x380
		ULONGLONG TppWorkerpListLock;                                           //0x388
		struct LIST_ENTRY64 TppWorkerpList;                                     //0x390
		ULONGLONG WaitOnAddressHashTable[128];                                  //0x3a0
		ULONGLONG TelemetryCoverageHeader;                                      //0x7a0
		ULONG CloudFileFlags;                                                   //0x7a8
		ULONG CloudFileDiagFlags;                                               //0x7ac
		CHAR PlaceholderCompatibilityMode;                                      //0x7b0
		CHAR PlaceholderCompatibilityModeReserved[7];                           //0x7b1
		ULONGLONG LeapSecondData;                                               //0x7b8
		union
		{
			ULONG LeapSecondFlags;                                              //0x7c0
			struct
			{
				ULONG SixtySecondEnabled : 1;                                     //0x7c0
				ULONG Reserved : 31;                                              //0x7c0
			};
		};
		ULONG NtGlobalFlag2;                                                    //0x7c4
		ULONGLONG ExtendedFeatureDisableMask;                                   //0x7c8
	};


	//0x8 bytes (sizeof)
	struct _CLIENT_ID32
	{
		ULONG UniqueProcess;                                                    //0x0
		ULONG UniqueThread;                                                     //0x4
	};

	//0x18 bytes (sizeof)
	struct _ACTIVATION_CONTEXT_STACK32
	{
		ULONG ActiveFrame;                                                      //0x0
		struct LIST_ENTRY32 FrameListCache;                                     //0x4
		ULONG Flags;                                                            //0xc
		ULONG NextCookieSequenceNumber;                                         //0x10
		ULONG StackId;                                                          //0x14
	};

	//0x4e0 bytes (sizeof)
	struct _GDI_TEB_BATCH32
	{
		ULONG Offset : 30;                                                        //0x0
		ULONG InProcessing : 1;                                                   //0x0
		ULONG HasRenderingCommand : 1;                                            //0x0
		ULONG HDC;                                                              //0x4
		ULONG Buffer[310];                                                      //0x8
	};

	//0x1038 bytes (sizeof)
	struct _TEB32
	{
		struct _NT_TIB32 NtTib;                                                 //0x0
		ULONG EnvironmentPointer;                                               //0x1c
		struct _CLIENT_ID32 ClientId;                                           //0x20
		ULONG ActiveRpcHandle;                                                  //0x28
		ULONG ThreadLocalStoragePointer;                                        //0x2c
		ULONG ProcessEnvironmentBlock;                                          //0x30
		ULONG LastErrorValue;                                                   //0x34
		ULONG CountOfOwnedCriticalSections;                                     //0x38
		ULONG CsrClientThread;                                                  //0x3c
		ULONG Win32ThreadInfo;                                                  //0x40
		ULONG User32Reserved[26];                                               //0x44
		ULONG UserReserved[5];                                                  //0xac
		ULONG WOW32Reserved;                                                    //0xc0
		ULONG CurrentLocale;                                                    //0xc4
		ULONG FpSoftwareStatusRegister;                                         //0xc8
		ULONG ReservedForDebuggerInstrumentation[16];                           //0xcc
		ULONG SystemReserved1[21];                                              //0x10c
		ULONG HeapFlsData;                                                      //0x160
		ULONG RngState[4];                                                      //0x164
		CHAR PlaceholderCompatibilityMode;                                      //0x174
		UCHAR PlaceholderHydrationAlwaysExplicit;                               //0x175
		CHAR PlaceholderReserved[10];                                           //0x176
		ULONG ProxiedProcessId;                                                 //0x180
		struct _ACTIVATION_CONTEXT_STACK32 _ActivationStack;                    //0x184
		UCHAR WorkingOnBehalfTicket[8];                                         //0x19c
		LONG ExceptionCode;                                                     //0x1a4
		ULONG ActivationContextStackPointer;                                    //0x1a8
		ULONG InstrumentationCallbackSp;                                        //0x1ac
		ULONG InstrumentationCallbackPreviousPc;                                //0x1b0
		ULONG InstrumentationCallbackPreviousSp;                                //0x1b4
		UCHAR InstrumentationCallbackDisabled;                                  //0x1b8
		UCHAR SpareBytes[23];                                                   //0x1b9
		ULONG TxFsContext;                                                      //0x1d0
		struct _GDI_TEB_BATCH32 GdiTebBatch;                                    //0x1d4
		struct _CLIENT_ID32 RealClientId;                                       //0x6b4
		ULONG GdiCachedProcessHandle;                                           //0x6bc
		ULONG GdiClientPID;                                                     //0x6c0
		ULONG GdiClientTID;                                                     //0x6c4
		ULONG GdiThreadLocalInfo;                                               //0x6c8
		ULONG Win32ClientInfo[62];                                              //0x6cc
		ULONG glDispatchTable[233];                                             //0x7c4
		ULONG glReserved1[29];                                                  //0xb68
		ULONG glReserved2;                                                      //0xbdc
		ULONG glSectionInfo;                                                    //0xbe0
		ULONG glSection;                                                        //0xbe4
		ULONG glTable;                                                          //0xbe8
		ULONG glCurrentRC;                                                      //0xbec
		ULONG glContext;                                                        //0xbf0
		ULONG LastStatusValue;                                                  //0xbf4
		struct _STRING32 StaticUnicodeString;                                   //0xbf8
		WCHAR StaticUnicodeBuffer[261];                                         //0xc00
		ULONG DeallocationStack;                                                //0xe0c
		ULONG TlsSlots[64];                                                     //0xe10
		struct LIST_ENTRY32 TlsLinks;                                           //0xf10
		ULONG Vdm;                                                              //0xf18
		ULONG ReservedForNtRpc;                                                 //0xf1c
		ULONG DbgSsReserved[2];                                                 //0xf20
		ULONG HardErrorMode;                                                    //0xf28
		ULONG Instrumentation[9];                                               //0xf2c
		struct _GUID ActivityId;                                                //0xf50
		ULONG SubProcessTag;                                                    //0xf60
		ULONG PerflibData;                                                      //0xf64
		ULONG EtwTraceData;                                                     //0xf68
		ULONG WinSockData;                                                      //0xf6c
		ULONG GdiBatchCount;                                                    //0xf70
		union
		{
			struct _PROCESSOR_NUMBER CurrentIdealProcessor;                     //0xf74
			ULONG IdealProcessorValue;                                          //0xf74
			struct
			{
				UCHAR ReservedPad0;                                             //0xf74
				UCHAR ReservedPad1;                                             //0xf75
				UCHAR ReservedPad2;                                             //0xf76
				UCHAR IdealProcessor;                                           //0xf77
			};
		};
		ULONG GuaranteedStackBytes;                                             //0xf78
		ULONG ReservedForPerf;                                                  //0xf7c
		ULONG ReservedForOle;                                                   //0xf80
		ULONG WaitingOnLoaderLock;                                              //0xf84
		ULONG SavedPriorityState;                                               //0xf88
		ULONG ReservedForCodeCoverage;                                          //0xf8c
		ULONG ThreadPoolData;                                                   //0xf90
		ULONG TlsExpansionSlots;                                                //0xf94
		ULONG MuiGeneration;                                                    //0xf98
		ULONG IsImpersonating;                                                  //0xf9c
		ULONG NlsCache;                                                         //0xfa0
		ULONG pShimData;                                                        //0xfa4
		ULONG HeapData;                                                         //0xfa8
		ULONG CurrentTransactionHandle;                                         //0xfac
		ULONG ActiveFrame;                                                      //0xfb0
		ULONG FlsData;                                                          //0xfb4
		ULONG PreferredLanguages;                                               //0xfb8
		ULONG UserPrefLanguages;                                                //0xfbc
		ULONG MergedPrefLanguages;                                              //0xfc0
		ULONG MuiImpersonation;                                                 //0xfc4
		union
		{
			volatile USHORT CrossTebFlags;                                      //0xfc8
			USHORT SpareCrossTebBits : 16;                                        //0xfc8
		};
		union
		{
			USHORT SameTebFlags;                                                //0xfca
			struct
			{
				USHORT SafeThunkCall : 1;                                         //0xfca
				USHORT InDebugPrint : 1;                                          //0xfca
				USHORT HasFiberData : 1;                                          //0xfca
				USHORT SkipThreadAttach : 1;                                      //0xfca
				USHORT WerInShipAssertCode : 1;                                   //0xfca
				USHORT RanProcessInit : 1;                                        //0xfca
				USHORT ClonedThread : 1;                                          //0xfca
				USHORT SuppressDebugMsg : 1;                                      //0xfca
				USHORT DisableUserStackWalk : 1;                                  //0xfca
				USHORT RtlExceptionAttached : 1;                                  //0xfca
				USHORT InitialThread : 1;                                         //0xfca
				USHORT SessionAware : 1;                                          //0xfca
				USHORT LoadOwner : 1;                                             //0xfca
				USHORT LoaderWorker : 1;                                          //0xfca
				USHORT SkipLoaderInit : 1;                                        //0xfca
				USHORT SkipFileAPIBrokering : 1;                                  //0xfca
			};
		};
		ULONG TxnScopeEnterCallback;                                            //0xfcc
		ULONG TxnScopeExitCallback;                                             //0xfd0
		ULONG TxnScopeContext;                                                  //0xfd4
		ULONG LockCount;                                                        //0xfd8
		LONG WowTebOffset;                                                      //0xfdc
		ULONG ResourceRetValue;                                                 //0xfe0
		ULONG ReservedForWdf;                                                   //0xfe4
		ULONGLONG ReservedForCrt;                                               //0xfe8
		struct _GUID EffectiveContainerId;                                      //0xff0
		ULONGLONG LastSleepCounter;                                             //0x1000
		ULONG SpinCallCount;                                                    //0x1008
		ULONGLONG ExtendedFeatureDisableMask;                                   //0x1010
		ULONG SchedulerSharedDataSlot;                                          //0x1018
		ULONG HeapWalkContext;                                                  //0x101c
		struct _GROUP_AFFINITY32 PrimaryGroupAffinity;                          //0x1020
		ULONG Rcu[2];                                                           //0x102c
	};

	//0x10 bytes (sizeof)
	struct _CLIENT_ID64
	{
		ULONGLONG UniqueProcess;                                                //0x0
		ULONGLONG UniqueThread;                                                 //0x8
	};

	//0x28 bytes (sizeof)
	struct _ACTIVATION_CONTEXT_STACK64
	{
		ULONGLONG ActiveFrame;                                                  //0x0
		struct LIST_ENTRY64 FrameListCache;                                     //0x8
		ULONG Flags;                                                            //0x18
		ULONG NextCookieSequenceNumber;                                         //0x1c
		ULONG StackId;                                                          //0x20
	};

	//0x4e8 bytes (sizeof)
	struct _GDI_TEB_BATCH64
	{
		ULONG Offset : 30;                                                        //0x0
		ULONG InProcessing : 1;                                                   //0x0
		ULONG HasRenderingCommand : 1;                                            //0x0
		ULONGLONG HDC;                                                          //0x8
		ULONG Buffer[310];                                                      //0x10
	};

	//0x1878 bytes (sizeof)
	struct _TEB64
	{
		struct _NT_TIB64 NtTib;                                                 //0x0
		ULONGLONG EnvironmentPointer;                                           //0x38
		struct _CLIENT_ID64 ClientId;                                           //0x40
		ULONGLONG ActiveRpcHandle;                                              //0x50
		ULONGLONG ThreadLocalStoragePointer;                                    //0x58
		ULONGLONG ProcessEnvironmentBlock;                                      //0x60
		ULONG LastErrorValue;                                                   //0x68
		ULONG CountOfOwnedCriticalSections;                                     //0x6c
		ULONGLONG CsrClientThread;                                              //0x70
		ULONGLONG Win32ThreadInfo;                                              //0x78
		ULONG User32Reserved[26];                                               //0x80
		ULONG UserReserved[5];                                                  //0xe8
		ULONGLONG WOW32Reserved;                                                //0x100
		ULONG CurrentLocale;                                                    //0x108
		ULONG FpSoftwareStatusRegister;                                         //0x10c
		ULONGLONG ReservedForDebuggerInstrumentation[16];                       //0x110
		ULONGLONG SystemReserved1[25];                                          //0x190
		ULONGLONG HeapFlsData;                                                  //0x258
		ULONGLONG RngState[4];                                                  //0x260
		CHAR PlaceholderCompatibilityMode;                                      //0x280
		UCHAR PlaceholderHydrationAlwaysExplicit;                               //0x281
		CHAR PlaceholderReserved[10];                                           //0x282
		ULONG ProxiedProcessId;                                                 //0x28c
		struct _ACTIVATION_CONTEXT_STACK64 _ActivationStack;                    //0x290
		UCHAR WorkingOnBehalfTicket[8];                                         //0x2b8
		LONG ExceptionCode;                                                     //0x2c0
		UCHAR Padding0[4];                                                      //0x2c4
		ULONGLONG ActivationContextStackPointer;                                //0x2c8
		ULONGLONG InstrumentationCallbackSp;                                    //0x2d0
		ULONGLONG InstrumentationCallbackPreviousPc;                            //0x2d8
		ULONGLONG InstrumentationCallbackPreviousSp;                            //0x2e0
		ULONG TxFsContext;                                                      //0x2e8
		UCHAR InstrumentationCallbackDisabled;                                  //0x2ec
		UCHAR UnalignedLoadStoreExceptions;                                     //0x2ed
		UCHAR Padding1[2];                                                      //0x2ee
		struct _GDI_TEB_BATCH64 GdiTebBatch;                                    //0x2f0
		struct _CLIENT_ID64 RealClientId;                                       //0x7d8
		ULONGLONG GdiCachedProcessHandle;                                       //0x7e8
		ULONG GdiClientPID;                                                     //0x7f0
		ULONG GdiClientTID;                                                     //0x7f4
		ULONGLONG GdiThreadLocalInfo;                                           //0x7f8
		ULONGLONG Win32ClientInfo[62];                                          //0x800
		ULONGLONG glDispatchTable[233];                                         //0x9f0
		ULONGLONG glReserved1[29];                                              //0x1138
		ULONGLONG glReserved2;                                                  //0x1220
		ULONGLONG glSectionInfo;                                                //0x1228
		ULONGLONG glSection;                                                    //0x1230
		ULONGLONG glTable;                                                      //0x1238
		ULONGLONG glCurrentRC;                                                  //0x1240
		ULONGLONG glContext;                                                    //0x1248
		ULONG LastStatusValue;                                                  //0x1250
		UCHAR Padding2[4];                                                      //0x1254
		struct _STRING64 StaticUnicodeString;                                   //0x1258
		WCHAR StaticUnicodeBuffer[261];                                         //0x1268
		UCHAR Padding3[6];                                                      //0x1472
		ULONGLONG DeallocationStack;                                            //0x1478
		ULONGLONG TlsSlots[64];                                                 //0x1480
		struct LIST_ENTRY64 TlsLinks;                                           //0x1680
		ULONGLONG Vdm;                                                          //0x1690
		ULONGLONG ReservedForNtRpc;                                             //0x1698
		ULONGLONG DbgSsReserved[2];                                             //0x16a0
		ULONG HardErrorMode;                                                    //0x16b0
		UCHAR Padding4[4];                                                      //0x16b4
		ULONGLONG Instrumentation[11];                                          //0x16b8
		struct _GUID ActivityId;                                                //0x1710
		ULONGLONG SubProcessTag;                                                //0x1720
		ULONGLONG PerflibData;                                                  //0x1728
		ULONGLONG EtwTraceData;                                                 //0x1730
		ULONGLONG WinSockData;                                                  //0x1738
		ULONG GdiBatchCount;                                                    //0x1740
		union
		{
			struct _PROCESSOR_NUMBER CurrentIdealProcessor;                     //0x1744
			ULONG IdealProcessorValue;                                          //0x1744
			struct
			{
				UCHAR ReservedPad0;                                             //0x1744
				UCHAR ReservedPad1;                                             //0x1745
				UCHAR ReservedPad2;                                             //0x1746
				UCHAR IdealProcessor;                                           //0x1747
			};
		};
		ULONG GuaranteedStackBytes;                                             //0x1748
		UCHAR Padding5[4];                                                      //0x174c
		ULONGLONG ReservedForPerf;                                              //0x1750
		ULONGLONG ReservedForOle;                                               //0x1758
		ULONG WaitingOnLoaderLock;                                              //0x1760
		UCHAR Padding6[4];                                                      //0x1764
		ULONGLONG SavedPriorityState;                                           //0x1768
		ULONGLONG ReservedForCodeCoverage;                                      //0x1770
		ULONGLONG ThreadPoolData;                                               //0x1778
		ULONGLONG TlsExpansionSlots;                                            //0x1780
		ULONGLONG ChpeV2CpuAreaInfo;                                            //0x1788
		ULONGLONG Unused;                                                       //0x1790
		ULONG MuiGeneration;                                                    //0x1798
		ULONG IsImpersonating;                                                  //0x179c
		ULONGLONG NlsCache;                                                     //0x17a0
		ULONGLONG pShimData;                                                    //0x17a8
		ULONG HeapData;                                                         //0x17b0
		UCHAR Padding7[4];                                                      //0x17b4
		ULONGLONG CurrentTransactionHandle;                                     //0x17b8
		ULONGLONG ActiveFrame;                                                  //0x17c0
		ULONGLONG FlsData;                                                      //0x17c8
		ULONGLONG PreferredLanguages;                                           //0x17d0
		ULONGLONG UserPrefLanguages;                                            //0x17d8
		ULONGLONG MergedPrefLanguages;                                          //0x17e0
		ULONG MuiImpersonation;                                                 //0x17e8
		union
		{
			volatile USHORT CrossTebFlags;                                      //0x17ec
			USHORT SpareCrossTebBits : 16;                                        //0x17ec
		};
		union
		{
			USHORT SameTebFlags;                                                //0x17ee
			struct
			{
				USHORT SafeThunkCall : 1;                                         //0x17ee
				USHORT InDebugPrint : 1;                                          //0x17ee
				USHORT HasFiberData : 1;                                          //0x17ee
				USHORT SkipThreadAttach : 1;                                      //0x17ee
				USHORT WerInShipAssertCode : 1;                                   //0x17ee
				USHORT RanProcessInit : 1;                                        //0x17ee
				USHORT ClonedThread : 1;                                          //0x17ee
				USHORT SuppressDebugMsg : 1;                                      //0x17ee
				USHORT DisableUserStackWalk : 1;                                  //0x17ee
				USHORT RtlExceptionAttached : 1;                                  //0x17ee
				USHORT InitialThread : 1;                                         //0x17ee
				USHORT SessionAware : 1;                                          //0x17ee
				USHORT LoadOwner : 1;                                             //0x17ee
				USHORT LoaderWorker : 1;                                          //0x17ee
				USHORT SkipLoaderInit : 1;                                        //0x17ee
				USHORT SkipFileAPIBrokering : 1;                                  //0x17ee
			};
		};
		ULONGLONG TxnScopeEnterCallback;                                        //0x17f0
		ULONGLONG TxnScopeExitCallback;                                         //0x17f8
		ULONGLONG TxnScopeContext;                                              //0x1800
		ULONG LockCount;                                                        //0x1808
		LONG WowTebOffset;                                                      //0x180c
		ULONGLONG ResourceRetValue;                                             //0x1810
		ULONGLONG ReservedForWdf;                                               //0x1818
		ULONGLONG ReservedForCrt;                                               //0x1820
		struct _GUID EffectiveContainerId;                                      //0x1828
		ULONGLONG LastSleepCounter;                                             //0x1838
		ULONG SpinCallCount;                                                    //0x1840
		UCHAR Padding8[4];                                                      //0x1844
		ULONGLONG ExtendedFeatureDisableMask;                                   //0x1848
		ULONGLONG SchedulerSharedDataSlot;                                      //0x1850
		ULONGLONG HeapWalkContext;                                              //0x1858
		struct _GROUP_AFFINITY64 PrimaryGroupAffinity;                          //0x1860
		ULONG Rcu[2];                                                           //0x1870
	};
}