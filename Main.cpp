#include "Csgo.h"


void MainThread()
{
	Print("Success");
	currentProcess = IoGetCurrentProcess();
	currentThread = KeGetCurrentThread();
	memcpy(&currentCid, (PVOID)((char*)currentThread + cidOffset), sizeof(CLIENT_ID));

	NTSTATUS status = STATUS_SUCCESS;
	Print("Waiting for CSGO...");

	while (SaveWhileLoop())
	{
		status = GetProcByName("csgo.exe", &targetApplication, 0);
		if (NT_SUCCESS(status))
			break;

		Sleep(300);
	}

	Print("Found CSGO. Getting PID...");

	HANDLE procId = PsGetProcessId(targetApplication);

	if (!procId)
	{
		Print("Failed to find proc id");
		ExitThread();
	}
	pid = (ULONG)procId;
	Print("Got pid %i", pid);

	int count = 0;
	while (!GetModuleBasex86(targetApplication, L"serverbrowser.dll"))
	{
		SaveWhileLoop();
		if (count >= 30) //wait 30 sec then abort
		{
			Print("failed to get serverbrowser dll\n");
			ExitThread();
		}
		count++;
		Sleep(1000);
	}

	clientBase = GetModuleBasex86(targetApplication, L"client.dll");
	if (!clientBase)
	{
		Print("Failed to get clientBase");
		ExitThread();
	}

	engineBase = GetModuleBasex86(targetApplication, L"engine.dll");
	if (!engineBase)
	{
		Print("Failed to get engineBase");
		ExitThread();
	}
	CsgoMain();
	ExitThread();
}

extern "C" NTSTATUS DriverEntry()
{
	Print("Initializing driver...");
	NTSTATUS status = STATUS_SUCCESS;

	status = InitWindowUtils();
	if (!NT_SUCCESS(status))
	{
		Print("failed to init window utils");
		return STATUS_UNSUCCESSFUL;
	}

	status = InitKeyMap();
	if (!NT_SUCCESS(status))
	{
		Print("failed to init keymap");
		return STATUS_UNSUCCESSFUL;
	}

	status = InitMouse(&mouseObject);
	if (!NT_SUCCESS(status))
	{
		Print("failed to init mouse");
		return STATUS_UNSUCCESSFUL;
	}

	status = InitDrawing();
	if (!NT_SUCCESS(status))
	{
		Print("failed to init drawing");
		return STATUS_UNSUCCESSFUL;
	}

	
	status = StartThread(MainThread);
	if (!NT_SUCCESS(status))
	{
		Print("failed to start thread");
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}
