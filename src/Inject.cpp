
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "base.hpp"




int wmain(int argc, wchar_t** argv) {

	if (argc != 3) {
		printf("\nusage : Inject.exe process_name dllfile.dll\n");
		exit(1);

	}


	DWORD base;
	// get base adress of kernel32.dll then loadlibraryA 
	DWORD pid = getPID(argv[1]);


	if (pid==-1) {
		printf("error retrieving process id \n");
		exit(1);
	}
	printf("************************************************\n");
	printf("************************************************\n");
	printf("\n[*] Process id : %d\n", pid);


	printf("[*] Process base address  at : 0x%x\n", get_base(argv[1]));
	HANDLE process = OpenProcess( // Open a Handle to process to inject to . 
		PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
		FALSE,
		pid
	);

	if (process == INVALID_HANDLE_VALUE) {
		printf("Some error occured code error  : %d\n", GetLastError());
		exit(0);
	}


	size_t path_len = (wcslen(argv[2]) + 1)*2 ; // NULL terminator . , strings on windows are coded unicode for each caracter 2 bytes . 

	LPVOID memory = VirtualAllocEx( // Allocate memory for the dll path 
		process,
		NULL,
		path_len,
		MEM_COMMIT,
		PAGE_EXECUTE
	);

	if (!memory) {
		printf("Allocating memmory failed \n");
		exit(1);
	}

	WriteProcessMemory(
		process,
		memory,
		argv[2],
		path_len,
		NULL
	);

	HMODULE k32 = GetModuleHandleA("kernel32.dll");
	LPVOID pointr_loadlibrary = GetProcAddress(k32, "LoadLibraryW");
	HANDLE thread = CreateRemoteThread( // Create the remote thread
		process,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)pointr_loadlibrary,
		memory,
		NULL,
		NULL

	);


	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &base);
	CloseHandle(thread);

	if (base == 0) 
	{
		printf("[X] Injection didnt work "); 
		exit(1); 
	}
	
	printf("[*] Dll injected succesfully !  Loaded at  base address  : 0x%x\n", base);

}

DWORD getPID(std::wstring name) // Get pid by name of process 
{
	DWORD PID = -1;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			std::wstring binaryPath = entry.szExeFile;
			if (binaryPath.find(name) != std::wstring::npos)
			{
				PID = entry.th32ProcessID;

				break;
			}
		}
	}

	CloseHandle(snapshot);
	return PID;
}



DWORD get_base(std::wstring name) // get base address of a process knowing its name . 
{

	
	DWORD base;
	HMODULE k32 = GetModuleHandleA("kernel32.dll");
	LPVOID get_handle = GetProcAddress(k32, "GetModuleHandleA");
	DWORD pid = getPID(name);
	HANDLE process = OpenProcess( // Open a Handle to process to inject to . 
		PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
		FALSE,
		pid
	);
	if (process == INVALID_HANDLE_VALUE) 
	{
		printf("Some error occured code error  : %d", GetLastError());
		exit(0);
	}


	HANDLE thread = CreateRemoteThread( // Create the remote thread
		process,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)get_handle,
		NULL,
		NULL,
		NULL

	);


	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &base);
	CloseHandle(thread);
	return base;
}
