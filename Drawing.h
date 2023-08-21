#pragma once
#include "Nt.h"
#include <wdm.h>
#include <stdio.h>

bool isWin32Thread = false;

INT WriteText(HDC hdc, LPWSTR txt, int fontsize, int x, int y)
{
	//---------------------
HFONT font = CreateFontW(
    fontsize,                      // nHeight
    0,                       // nWidth
    0,                       // nEscapement
    0,                       // nOrientation
    FW_BOLD,                 // nWeight
    FALSE,                   // bItalic
    FALSE,                   // bUnderline
    FALSE,                   // bStrikeOut
    DEFAULT_CHARSET,         // nCharSet
	OUT_STROKE_PRECIS,           // nOutPrecision
    CLIP_DEFAULT_PRECIS,     // nClipPrecision
    CLEARTYPE_QUALITY,       // nQuality
    FF_ROMAN,                // nPitchAndFamily
    L"Times New Roman"       // lpFaceName
);	HFONT oldfont = NtGdiSelectFont(hdc, font);
	//SetBkMode(hdc, TRANSPARENT);
	bool textSucess = extTextOutW(hdc, x, y, 0, NULL, txt, wcslen(txt), 0);
	NtGdiDeleteObjectApp(font);

	//----------------------
	return 1;
}

PETHREAD GetValidWin32Thread(PVOID* win32Thread)
{
	int currentThreadId = 1;
	NTSTATUS status = STATUS_SUCCESS;
	do
	{
		PETHREAD currentEthread = 0;
		status = PsLookupThreadByThreadId((HANDLE)currentThreadId, &currentEthread);

		if (!NT_SUCCESS(status) || !currentEthread)
		{
			currentThreadId++;
			continue;
		}

		if (PsIsThreadTerminating(currentEthread))
		{
			currentThreadId++;
			continue;
		}

		PVOID Win32Thread = PsGetThreadWin32Thread(currentEthread);
		//memcpy(&Win32Thread, (PVOID)((UINT64)currentEthread + win32ThreadOffset), sizeof(PVOID));

		if (Win32Thread)
		{
			PEPROCESS threadOwner = PsGetThreadProcess(currentEthread);
			char procName[15];
			memcpy(&procName, PsGetProcessImageFileName(threadOwner), sizeof(procName));
			if (!strcmp(procName, "explorer.exe"))
			{
				*win32Thread = Win32Thread;
				return currentEthread;
			}
		}
		currentThreadId++;
	} while (0x3000 > currentThreadId);

	return 0;
}

inline void SpoofWin32Thread(PVOID newWin32Value, PEPROCESS newProcess, CLIENT_ID newClientId)
{
	PKTHREAD currentThread = KeGetCurrentThread();
	
	PsSetThreadWin32Thread(currentThread, newWin32Value, PsGetThreadWin32Thread(currentThread));

	PVOID processPtr = (PVOID)((char*)currentThread + processOffset);
	memcpy(processPtr, &newProcess, sizeof(PEPROCESS));

	PVOID clientIdPtr = (PVOID)((char*)currentThread + cidOffset);
	memcpy(clientIdPtr, &newClientId, sizeof(CLIENT_ID));
}

KAPC_STATE apc = { 0 };

PVOID currentWin32Thread = 0;
PEPROCESS currentProcess = 0;
PETHREAD currentThread = 0;
CLIENT_ID currentCid = { 0 };

HDC hdc;
HDC hdc2;
HBRUSH brush;

bool SpoofWin32Thread()
{
	if (isWin32Thread)
	{
		//Print("already win32\n");
		return true;
	}

	PVOID targetWin32Thread = 0;
	PETHREAD targetThread = GetValidWin32Thread(&targetWin32Thread);
	if (!targetWin32Thread || !targetThread)
	{
		Print("failed to find win32thread");
		return false;
	}
	PEPROCESS targetProcess = PsGetThreadProcess(targetThread);

	CLIENT_ID targetCid = { 0 };
	memcpy(&targetCid, (PVOID)((char*)targetThread + cidOffset), sizeof(CLIENT_ID));

	KeStackAttachProcess(targetProcess, &apc);
	SpoofWin32Thread(targetWin32Thread, targetProcess, targetCid);

	const unsigned short* t = L"Running";
	LPWSTR msg = LPWSTR(t);
	hdc2 = NtUserGetDC(NULL);
	WriteText(hdc2, msg, -10, 1, 0);

	isWin32Thread = true;
}

void UnspoofWin32Thread()
{
	if (!isWin32Thread)
		return;
	NtUserReleaseDC(hdc2);
	SpoofWin32Thread(currentWin32Thread, currentProcess, currentCid);
	KeUnstackDetachProcess(&apc);
	isWin32Thread = false;
}

NTSTATUS InitDrawing()
{
	PVOID win32kBase = (PVOID)GetKernelModuleBase("win32kbase.sys");
	PVOID win32kfullBase = (PVOID)GetKernelModuleBase("win32kfull.sys");

	if (!win32kBase || !win32kfullBase)
	{
		Print("Could not find kernel module bases");
		return STATUS_UNSUCCESSFUL;
	}

	NtUserGetDCPtr = RtlFindExportedRoutineByName(win32kBase, "NtUserGetDC");
	NtGdiPatBltPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiPatBlt");
	NtGdiSelectBrushPtr = RtlFindExportedRoutineByName(win32kBase, "GreSelectBrush");
	NtUserReleaseDCPtr = RtlFindExportedRoutineByName(win32kBase, "NtUserReleaseDC");
	NtGdiCreateSolidBrushPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiCreateSolidBrush");
	NtGdiDeleteObjectAppPtr = RtlFindExportedRoutineByName(win32kBase, "NtGdiDeleteObjectApp");
	NtGdiExtTextOutWPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiExtTextOutW");
	NtGdiHfontCreatePtr = RtlFindExportedRoutineByName(win32kfullBase, "hfontCreate");
	NtGdiSelectFontPtr = RtlFindExportedRoutineByName(win32kfullBase, "NtGdiSelectFont");
	if (!NtUserGetDCPtr || !NtGdiPatBltPtr || !NtGdiSelectBrushPtr ||
		!NtUserReleaseDCPtr || !NtGdiCreateSolidBrushPtr || !NtGdiDeleteObjectAppPtr
		|| !NtGdiExtTextOutWPtr || !NtGdiHfontCreatePtr || !NtGdiSelectFontPtr)
	{
		Print("Could not find kernel functions required for drawing");
		return STATUS_UNSUCCESSFUL;
	}

	Print("found everything");
	return STATUS_SUCCESS;
}

inline bool InWindow(const RECT* rect)
{
	float windowLeft = targetWindowPosX;
	float windowRight = targetWindowPosX + targetWindowWidth;
	float windowTop = targetWindowPosY;
	float windowBottom = targetWindowPosY + targetWindowHeight;

	if (rect->right > windowRight || rect->left < windowLeft || rect->bottom > windowBottom || rect->top < windowTop)
		return false;

	return true;
}

INT FrameRect(HDC hdc, CONST RECT* rect, HBRUSH hbr, int thickness, int health, bool txt, const wchar_t* actualtext = L"NULL")
{
	if (!InWindow(rect))
		return 0;

	int green = (int)(255.0 * ((float)health / 100.0));
	int red = 255 - green;

	// Create the health brush with the calculated color
	HBRUSH healthBrush = NtGdiCreateSolidBrush(RGB(red, green, 0), NULL);
	HBRUSH oldHealthBrush = NtGdiSelectBrush(hdc, healthBrush);
	RECT r = *rect;

	// Calculate the width of the health bar
	int healthWidth = (r.right - r.left) * health / 100;

	if (txt && health > 0) {
		wchar_t healthstr[20];
		swprintf(healthstr, L"Health: %d", health);
		WriteText(hdc, healthstr, r.bottom / 45, r.left, r.bottom);
	}

	if (wcscmp(actualtext, L"NULL") != 0) {
		WriteText(hdc, LPWSTR(actualtext), r.bottom / 45, r.left, r.bottom + (r.bottom / 45));
	}

	// Draw the frame rectangle and borders
	NtGdiPatBlt(hdc, r.left, r.top, thickness, r.bottom - r.top, PATCOPY);
	NtGdiPatBlt(hdc, r.right - thickness, r.top, thickness, r.bottom - r.top, PATCOPY);
	NtGdiPatBlt(hdc, r.left, r.top, r.right - r.left, thickness, PATCOPY); // top
	NtGdiPatBlt(hdc, r.left, r.bottom - thickness, r.right - r.left, thickness, PATCOPY);

	NtGdiSelectBrush(hdc, oldHealthBrush);
	NtGdiDeleteObjectApp(healthBrush);

	return 1;
}