#include "Globals.h"
#include "Vectors.h"
#include "Memory.h"
#include "Overlay.h"
#include <iostream>
#include <time.h>
#include <Psapi.h>

HWND gameWindow = FindWindow(NULL, L"Rainbow Six");

//Defining our globals
int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);
int bSkl;
HANDLE GameHandle = 0x0;
LPVOID BaseAddress = NULL;
Memory Mem = Memory();
Overlay Over = Overlay();

DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

void OpenHandle() {
	DWORD processID = FindProcessId(L"RainbowSix.exe");
	GameHandle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, processID);
}

int main()
{

	std::string ConsoleNameStr = "jizzware R6 by [REDACTED]";
	SetConsoleTitleA(ConsoleNameStr.c_str());
	
	std::cout << "Do you want [S]keleton or [B]ox?" << std::endl;
	bSkl = std::getchar();
	system("cls");

	if (gameWindow != 0) {
		std::cout << "[+] Attached to process" << std::endl;

		OpenHandle();

		if (GameHandle == INVALID_HANDLE_VALUE || GameHandle == NULL) {
			std::cout << "[+] Could not attach to process" << std::endl;
			system("pause");
			return 1;
		}

		Mem.SetBaseAddress();
		std::cout << "[+] Created overlay" << std::endl;

		Over.SetupWindow();
		std::cout << "[+] Started reading thread, starting rendering" << std::endl;

		//Resolve pointers
		Mem.UpdateAddresses();
		
		//Start the main loop
		Over.Loop();

		system("pause");
		return EXIT_SUCCESS;

		return 0;
	}
	else {
		std::cout << "[+] Game must be running to continue." << std::endl;
		std::cout << "" << std::endl;
		system("pause");
		return 0;
	}
}

void GetResolution(UINT horizontal, UINT vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = FindWindowA(NULL, "Rainbow Six");
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
