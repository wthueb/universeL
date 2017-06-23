// Simple LoadLibrary .dll Injector
// Made by wi1

#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <conio.h>
#include <string>


// set console text color
#define cyan SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define green SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define red SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY)
#define white SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#define exit std::cout << "press any key to exit...\n"; _getch(); return 0;


DWORD GetProcessID(const char* ProcessName)
{
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	// handle of the first program
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// iterate through all processes until process is found
	if (Process32First(hSnapshot, &pe) == TRUE)
	{
		while (Process32Next(hSnapshot, &pe) == TRUE)
		{
			if (strcmp(pe.szExeFile, ProcessName) == 0)
			{
				CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
		}
	}

	return NULL;
}

BOOL InjectDll(DWORD ProcessID, LPCSTR DllPath)
{
	// if process id or dll is null
	if (!ProcessID || !DllPath)
		return FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

	// if process cannot be opened
	if (hProcess == INVALID_HANDLE_VALUE)
		return FALSE;

	// same in every program, so no need to find where it is in other process
	LPVOID AddressToLoadLibrary = static_cast<LPVOID>(GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"));

	// allocate memory in other program for dll path to be stored
	LPVOID AddressToDll = static_cast<LPVOID>(VirtualAllocEx(hProcess, NULL, strlen(DllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

	// write the path of the dll to that memory
	WriteProcessMemory(hProcess, AddressToDll, static_cast<LPVOID>(const_cast<char*>(DllPath + '\0')), strlen(DllPath) + 1, NULL);

	// load the dll in the target process by calling the address of the dll
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, static_cast<LPTHREAD_START_ROUTINE>(AddressToLoadLibrary), static_cast<LPVOID>(AddressToDll), NULL, NULL);

	if (hRemoteThread != INVALID_HANDLE_VALUE)
	{
		// wait 10000 milliseconds for the dll to be loaded, else exit
		if (WaitForSingleObject(hRemoteThread, 10000) == WAIT_OBJECT_0)
		{
			// always close handles and unallocate memory
			VirtualFreeEx(hProcess, AddressToLoadLibrary, 0, MEM_RELEASE);
			CloseHandle(hRemoteThread);
			CloseHandle(hProcess);

			return TRUE;
		}
		else
		{
			// always close handles and unallocate memory
			VirtualFreeEx(hProcess, AddressToLoadLibrary, 0, MEM_RELEASE);
			CloseHandle(hRemoteThread);
			CloseHandle(hProcess);

			return FALSE;
		}
	}

	return FALSE;
}

void WaitForExit(void*);

int main()
{
	using namespace std;

	white;
	cout << "=====================================================================\n\n";
	cyan;
	cout << "               simple cs:go LoadLibrary .dll injector\n";
	cout << "                            made by wi1\n\n";
	white;
	cout << "=====================================================================\n\n";

	// default to csgo cause what else would they use a basic injector for anyways?
	string processname = "csgo";

	// add .exe if they did not
	if (processname.length() < 5 || processname.substr(processname.length() - 4) != ".exe")
		processname += ".exe";

	DWORD processid = GetProcessID(processname.c_str());

	// if process id cannot be found (not running/wrong name)
	if (!processid)
	{
		red;
		cout << "csgo.exe could not be found. is it running?\n\n";
		exit;
	}

	green;
	cout << processname << " has been found! process id: " << processid << "\n\n";

	string dllname = "universeL.dll";

	// get absolute path of dll file
	char dllpath[MAX_PATH];
	GetFullPathNameA(dllname.c_str(), MAX_PATH, dllpath, NULL);

	// if path does not exist or is a directory
	if (GetFileAttributesA(dllpath) == INVALID_FILE_ATTRIBUTES || GetFileAttributesA(dllpath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		red;
		cout << dllname << " is not in the same directory as this injector\n"
			           "did you rename it?\n\n";
		exit;
	}

	green;
	cout << dllname << " found! absolute path: " << dllpath << "\n\n";

	cyan;
	cout << "press any key to inject...\n";
	_getch();

	cyan;
	cout << "\nattempting to inject...\n";

	if (!InjectDll(processid, dllpath))
	{
		red;
		cout << "\ninjection failed. make sure that cs:go is running and\n"
			"that the .dll is in the same directory as this injector\n\n";
		exit;
	}
	
	white;
	cout << "\n=====================================================================\n\n";
	green;
	cout <<   "successfully injected " << dllname << " into " << processname << "!\n\n";
	white;
	cout <<   "=====================================================================\n\n";
	
	cyan;
	cout << "press any key to exit...\n\n";
	cout << "automatically exiting in 5 seconds...\n";
	CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(WaitForExit), 0, 0, 0);
	_getch();
	
	return 0;
}

void WaitForExit(void*)
{
	Sleep(5000);
#ifdef exit
#undef exit
#endif
	exit(0);
}
