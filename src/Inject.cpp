
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "base.hpp"

// main functionality for DLL injector implemented here 


int wmain(int argc, wchar_t** argv) {


	if (argc != 3) {
		printf("usage : Inject.exe process_name dllfile.dll");
		exit(1);

	}


	DWORD base;
	// get base adress of kernel32.dll then loadlibraryA 

	



	DWORD pid = getPID(argv[1]);


	if (pid==-1) {
		printf("error retrieving process id \n");
		exit(1);
	}


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

	WriteProcessMemory( // write to process memory the DLL path 
		process,
		memory,
		argv[2],
		path_len,
		NULL
	);

	HMODULE k32 = GetModuleHandleA("kernel32.dll");

	LPVOID handl_loadlibrary = GetProcAddress(k32, "LoadLibraryW");

	HANDLE thread = CreateRemoteThread( // Create the remote thread
		process,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)handl_loadlibrary,
		memory,
		NULL,
		NULL

	);


	WaitForSingleObject(thread, INFINITE);

	GetExitCodeThread(thread, &base);


	CloseHandle(thread);

	printf("[*] Dll injected succesfully !  Loaded at  base address  : 0x%x\n", base);

}
