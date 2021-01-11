#pragma once
#include <WinSock2.h>
#include "commonconfig.h"
#include <Windows.h>
#include <WS2spi.h>
#include <intrin.h>
#include "hooker.h"
#include "logger.h"
#include "memedit.h"

// fix returnaddress func
// https://docs.microsoft.com/en-us/cpp/intrinsics/returnaddress?view=msvc-160
#pragma intrinsic(_ReturnAddress)

// link socket library
#pragma comment(lib, "WS2_32.lib")

// deprecated api call warning
#pragma warning(disable : 4996)

/// <summary>
/// Gets called when mutex hook is triggered.
/// </summary>
typedef VOID(*PostMutexFunc_t)();
PostMutexFunc_t g_PostMutexFunc;

// ignore this region if you arent sure what you are doing
#pragma region LibraryDefs

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress
/// </summary>
typedef FARPROC(WINAPI* GetProcAddress_t)(HMODULE hModule, LPCSTR lpProcName);
GetProcAddress_t GetProcAddress_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createmutexa
/// </summary>
typedef HANDLE(WINAPI* CreateMutexA_t)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
CreateMutexA_t CreateMutexA_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-openmutexw
/// Please note: there is no microsoft doc for OpenMutexA, but OpenMutexW is the same except for the type of string passed (LPCSTR vs LPCWSTR)
/// </summary>
typedef HANDLE(WINAPI* OpenMutexA_t)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
OpenMutexA_t OpenMutexA_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/ws2spi/nf-ws2spi-wspstartup
/// </summary>
typedef int(WSPAPI* WSPStartup_t)(WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable);
WSPStartup_t WSPStartup_Original;
/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexa
/// </summary>
typedef ATOM(WINAPI* RegisterClassExA_t)(const WNDCLASSEXA* lpWc);
RegisterClassExA_t RegisterClassExA_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
/// </summary>
typedef BOOL(WINAPI* CreateProcessW_t)(
	LPCWSTR               lpApplicationName,
	LPWSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCWSTR               lpCurrentDirectory,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);
CreateProcessW_t CreateProcessW_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
/// </summary>
typedef BOOL(WINAPI* CreateProcessA_t)(
	LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);
CreateProcessA_t CreateProcessA_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
/// </summary>
typedef HANDLE(WINAPI* OpenProcess_t)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
OpenProcess_t OpenProcess_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
/// </summary>
typedef HANDLE(WINAPI* CreateThread_t)(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId
	);
CreateThread_t CreateThread_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getacp
/// </summary>
typedef UINT(WINAPI* GetACP_t)();
GetACP_t GetACP_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
/// </summary>
typedef HWND(WINAPI* CreateWindowExA_t)(
	DWORD     dwExStyle,
	LPCSTR    lpClassName,
	LPCSTR    lpWindowName,
	DWORD     dwStyle,
	int       X,
	int       Y,
	int       nWidth,
	int       nHeight,
	HWND      hWndParent,
	HMENU     hMenu,
	HINSTANCE hInstance,
	LPVOID    lpParam
	);
CreateWindowExA_t CreateWindowExA_Original;

/// <summary>
/// http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FProcess%2FNtTerminateProcess.html
/// </summary>
typedef LONG(NTAPI* NtTerminateProcess_t)(HANDLE hProcHandle, LONG ntExitStatus);
NtTerminateProcess_t NtTerminateProcess_Original;

/// <summary>
/// https://docs.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyexa
/// </summary>
/// <returns></returns>
typedef LSTATUS(WINAPI* RegCreateKeyExA_t)(
	HKEY                        hKey,
	LPCSTR                      lpSubKey,
	DWORD                       Reserved,
	LPSTR                       lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
	);
RegCreateKeyExA_t RegCreateKeyExA_Original;

#pragma endregion

bool g_bThemidaUnpacked = false;

DWORD g_dwGetProcRetAddr = 0;

SOCKET			g_GameSock;
WSPPROC_TABLE	g_ProcTable;

const char* g_sRedirectIP;
const char* g_sOriginalIP;

/// <summary>
/// Function called from library hooks.
/// Most of the time this should be triggered by the Mutex hook, however, in the case that
/// the Mutex hook does not get triggered then this will be executed by CreateWindowExA
/// for redundancy. The contents of this function will only be executed once, even if both 
/// Mutex and CreateWindow hooks are called properly.
/// </summary>
static VOID OnThemidaUnpack()
{
	if (g_bThemidaUnpacked) return;

	g_bThemidaUnpacked = TRUE;

	if (MAPLETRACKING_SLEEP_ON_UNPACK)
	{
		Log("Themida unpacked => sleeping for %d milliseconds.", MAPLETRACKING_SLEEP_ON_UNPACK);
		Sleep(MAPLETRACKING_SLEEP_ON_UNPACK);
	}

	g_PostMutexFunc();
}

/// <summary>
/// Used to map out imports used by MapleStory.
/// The log output can be used to reconstruct the _ZAPIProcAddress struct
/// ZAPI struct is the dword before the while loop when searching for aob: 68 FE 00 00 00 ?? 8D
/// </summary>
static FARPROC WINAPI GetProcAddress_Hook(HMODULE hModule, LPCSTR lpProcName)
{
	if (g_bThemidaUnpacked)
	{
		DWORD dwRetAddr = reinterpret_cast<DWORD>(_ReturnAddress());

		if (!g_dwGetProcRetAddr)
		{
			g_dwGetProcRetAddr = dwRetAddr;

			Log("[GetProcAddress] Detected library loading from %08X.", dwRetAddr);
		}

		if (g_dwGetProcRetAddr == dwRetAddr)
		{
			Log("[GetProcAddress] => %s", lpProcName);
		}
		else
		{
			Log("[GetProcAddress] End library loading. Unhooking.");

			if (!SetHook(FALSE, reinterpret_cast<void**>(&GetProcAddress_Original), GetProcAddress_Hook))
			{
				Log("Unable to unhook GetProcAddress.");
			}
		}
	}

	return GetProcAddress_Original(hModule, lpProcName);
}

/// <summary>
/// CreateMutexA is the first Windows library call after the executable unpacks itself.
/// We hook this function to do all our memory edits and hooks when it's called.
/// </summary>
static HANDLE WINAPI CreateMutexA_Hook(
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL				  bInitialOwner,
	LPCSTR				  lpName
)
{
	if (!CreateMutexA_Original)
	{
		DBGLOG("Original CreateMutex pointer corrupted. Failed to return mutex value to calling function.");

		return nullptr;
	}
	else if (lpName && strstr(lpName, MAPLE_MUTEX))
	{
		DBGLOG("Mutex spoofed, unhooking..");

		if (!SetHook(FALSE, reinterpret_cast<void**>(&CreateMutexA_Original), CreateMutexA_Hook))
		{
			DBGLOG("Failed to unhook mutex.");
		}

		OnThemidaUnpack();

		return CreateMutexA_Original(lpMutexAttributes, bInitialOwner, lpName);
	}

	return CreateMutexA_Original(lpMutexAttributes, bInitialOwner, lpName);
}

/// <summary>
/// In some versions, Maple calls this library function to check if the anticheat has started.
/// We can spoof this and return a fake handle for it to close.
/// </summary>
static HANDLE WINAPI OpenMutexA_Hook(
	DWORD  dwDesiredAccess,
	BOOL   bInitialOwner,
	LPCSTR lpName
)
{
	Log("Opening mutex %s", lpName);

	if (strstr(lpName, "Global")) // make sure we only override hackshield
	{
		// return handle to a spoofed mutex so it can close the handle
		return CreateMutexA_Original(NULL, TRUE, "FakeMutex1");
	}
	else // TODO add second mutex handling
	{
		return OpenMutexA_Original(dwDesiredAccess, bInitialOwner, lpName);
	}
}

/// <summary>
/// Used to track what maple is trying to start (mainly for anticheat modules).
/// </summary>
static BOOL WINAPI CreateProcessW_Hook(
	LPCWSTR               lpApplicationName,
	LPWSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCWSTR               lpCurrentDirectory,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	if (MAPLETRACKING_CREATE_PROCESS)
	{
		auto sAppName = lpApplicationName ? lpApplicationName : L"Null App Name";
		auto sArgs = lpCommandLine ? lpCommandLine : L"Null Args";

		Log("CreateProcessW -> %s : %s", sAppName, sArgs);
	}

	return CreateProcessW_Original(
		lpApplicationName, lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags,
		lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation
	);
}

/// <summary>
/// Used same as above and also to kill/redirect some web requests.
/// </summary>
static BOOL WINAPI CreateProcessA_Hook(
	LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	if (MAPLETRACKING_CREATE_PROCESS)
	{
		auto sAppName = lpApplicationName ? lpApplicationName : "Null App Name";
		auto sArgs = lpCommandLine ? lpCommandLine : "Null Args";

		Log("CreateProcessA -> %s : %s", sAppName, sArgs);
	}

	if (MAPLE_KILL_EXIT_WINDOW && strstr(lpCommandLine, MAPLE_KILL_EXIT_WINDOW))
	{
		DBGLOG("[CreateProcessA] [%08X] Killing web request to: %s", _ReturnAddress(), lpApplicationName);
		return FALSE; // ret value doesn't get used by maple after creating web requests as far as i can tell
	}

	return CreateProcessA_Original(
		lpApplicationName, lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags,
		lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation
	);
}

/// <summary>
/// Same as CreateProcessW
/// </summary>
static HANDLE WINAPI CreateThread_Hook(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId
)
{
	return CreateThread_Original(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}

/// <summary>
/// Used to track what processes Maple opens.
/// </summary>
static HANDLE WINAPI OpenProcess_Hook(
	DWORD dwDesiredAccess,
	BOOL  bInheritHandle,
	DWORD dwProcessId
)
{
	Log("OpenProcess -> PID: %d - CallAddy: %08X", dwProcessId, _ReturnAddress());

	return OpenProcess_Original(dwDesiredAccess, bInheritHandle, dwProcessId);
}

/// <summary>
/// This library call is used by nexon to determine the locale of the connecting clients PC. We spoof it.
/// </summary>
/// <returns></returns>
static UINT WINAPI GetACP_Hook() // AOB: FF 15 ?? ?? ?? ?? 3D ?? ?? ?? 00 00 74 <- library call inside winmain func
{
	if (!MAPLE_LOCALE_SPOOF) return GetACP_Original(); // should not happen cuz we dont hook if value is zero

	UINT uiNewLocale = MAPLE_LOCALE_SPOOF;

	// we dont wanna unhook until after themida is unpacked
	// because if themida isn't unpacked then the call we are intercepting is not from maple
	if (g_bThemidaUnpacked)
	{
		DWORD dwRetAddr = reinterpret_cast<DWORD>(_ReturnAddress());

		// return address should be a cmp eax instruction because ret value is stored in eax
		// and nothing else should happen before the cmp
		if (ReadValue<BYTE>(dwRetAddr) == x86CMPEAX)
		{
			uiNewLocale = ReadValue<DWORD>(dwRetAddr + 1); // check value is always 4 bytes

			DBGLOG("[GetACP] Found desired locale: %d", uiNewLocale);
		}
		else
		{
			DBGLOG("[GetACP] Unable to automatically determine locale, using stored locale: %d", uiNewLocale);
		}

		DBGLOG("[GetACP] Locale spoofed to %d, unhooking. Calling address: %08X", uiNewLocale, dwRetAddr);

		if (!SetHook(FALSE, reinterpret_cast<void**>(&GetACP_Original), GetACP_Hook))
		{
			DBGLOG("Failed to unhook GetACP.");
		}
	}

	return uiNewLocale;
}

/// <summary>
/// Blocks the startup patcher "Play!" window and forces the login screen to be minimized
/// </summary>
static HWND WINAPI CreateWindowExA_Hook(
	DWORD     dwExStyle,
	LPCSTR    lpClassName,
	LPCSTR    lpWindowName,
	DWORD     dwStyle,
	int       X,
	int       Y,
	int       nWidth,
	int       nHeight,
	HWND      hWndParent,
	HMENU     hMenu,
	HINSTANCE hInstance,
	LPVOID    lpParam
)
{
	if (MAPLE_PATCHER_CLASS && strstr(lpClassName, MAPLE_PATCHER_CLASS))
	{
		DBGLOG("Bypassing patcher window..");

		OnThemidaUnpack();

		return NULL;
	}
	else
	{
		if (MAPLE_FORCE_WINDOWED && MAPLE_WINDOW_CLASS && strstr(lpClassName, MAPLE_WINDOW_CLASS))
		{
			dwExStyle = 0;
			dwStyle = 0xCA0000;
		}

		OnThemidaUnpack();

		return CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}
}

/// <summary>
/// We use this function to track what memory addresses are killing the process.
/// There are more ways that Maple kills itself, but this is one of them.
/// </summary>
static LONG NTAPI NtTerminateProcess_Hook(
	HANDLE hProcHandle,
	LONG   ntExitStatus
)
{
	Log("NtTerminateProcess: %08X", unsigned(_ReturnAddress()));

	return NtTerminateProcess_Original(hProcHandle, ntExitStatus);
}

/// <summary>
/// Maplestory saves registry information (config stuff) for a number of things. This can be used to track that.
/// </summary>
static LSTATUS WINAPI RegCreateKeyExA_Hook(
	HKEY                        hKey,
	LPCSTR                      lpSubKey,
	DWORD                       Reserved,
	LPSTR                       lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
)
{
	Log("RegCreateKeyExA - Return address: %d", _ReturnAddress());

	return RegCreateKeyExA_Original(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

/// <summary>
/// 
/// </summary>
static int WSPAPI WSPConnect_Hook(
	SOCKET				   s,
	const struct sockaddr* name,
	int					   namelen,
	LPWSABUF			   lpCallerData,
	LPWSABUF			   lpCalleeData,
	LPQOS				   lpSQOS,
	LPQOS				   lpGQOS,
	LPINT				   lpErrno
)
{
	char szAddr[50];
	DWORD dwLen = 50;
	WSAAddressToStringA((sockaddr*)name, namelen, NULL, szAddr, &dwLen);

	sockaddr_in* service = (sockaddr_in*)name;

	if (MAPLETRACKING_WSPCONN_PRINT)
	{
		DBGLOG("WSPConnect IP Detected: %s", szAddr);
	}

	if (strstr(szAddr, g_sRedirectIP))
	{
		DBGLOG("Detected and rerouting socket connection to IP: %s", g_sOriginalIP);
		service->sin_addr.S_un.S_addr = inet_addr(g_sOriginalIP);
		g_GameSock = s;
	}

	return g_ProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

/// <summary>
/// 
/// </summary>
static int WSPAPI WSPGetPeerName_Hook(
	SOCKET			 s,
	struct sockaddr* name,
	LPINT			 namelen,
	LPINT			 lpErrno
)
{
	int nRet = g_ProcTable.lpWSPGetPeerName(s, name, namelen, lpErrno);

	if (nRet != SOCKET_ERROR)
	{
		char szAddr[50];
		DWORD dwLen = 50;
		WSAAddressToStringA((sockaddr*)name, *namelen, NULL, szAddr, &dwLen);

		sockaddr_in* service = (sockaddr_in*)name;

		USHORT nPort = ntohs(service->sin_port);

		if (s == g_GameSock)
		{
			char szAddr[50];
			DWORD dwLen = 50;
			WSAAddressToStringA((sockaddr*)name, *namelen, NULL, szAddr, &dwLen);

			sockaddr_in* service = (sockaddr_in*)name;

			u_short nPort = ntohs(service->sin_port);

			service->sin_addr.S_un.S_addr = inet_addr(g_sOriginalIP);

			DBGLOG("WSPGetPeerName => IP Replaced: %s -> %s", szAddr, g_sOriginalIP);
		}
		else
		{
			DBGLOG("WSPGetPeerName => IP Ignored: %s:%d", szAddr, nPort);
		}
	}
	else
	{
		Log("WSPGetPeerName Socket Error: %d", nRet);
	}

	return nRet;
}

/// <summary>
/// 
/// </summary>
/// <param name="s"></param>
/// <param name="lpErrno"></param>
/// <returns></returns>
static int WSPAPI WSPCloseSocket_Hook(
	SOCKET s,
	LPINT  lpErrno
)
{
	int nRet = g_ProcTable.lpWSPCloseSocket(s, lpErrno);

	if (s == g_GameSock)
	{
		DBGLOG("Socket closed by application.. (%d). CallAddr: %02x", nRet, _ReturnAddress());
		g_GameSock = INVALID_SOCKET;
	}

	return nRet;
}

/// <summary>
/// 
/// </summary>
static int WSPAPI WSPStartup_Hook(
	WORD				wVersionRequested,
	LPWSPDATA			lpWSPData,
	LPWSAPROTOCOL_INFOW lpProtocolInfo,
	WSPUPCALLTABLE		UpcallTable,
	LPWSPPROC_TABLE		lpProcTable
)
{
	int nRet = WSPStartup_Original(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);

	if (nRet == NO_ERROR)
	{
		DBGLOG("Overriding socket routines..");

		g_GameSock = INVALID_SOCKET;
		g_ProcTable = *lpProcTable;

		lpProcTable->lpWSPConnect = WSPConnect_Hook;
		lpProcTable->lpWSPGetPeerName = WSPGetPeerName_Hook;
		lpProcTable->lpWSPCloseSocket = WSPCloseSocket_Hook;
	}
	else
	{
		Log("WSPStartup Error Code: %d", nRet);
	}

	return nRet;
}